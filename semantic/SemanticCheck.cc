//
// Created by 杨丰硕 on 2023/2/15.
//
#include <unordered_set>
#include "../common/Utils.h"
#include "SemanticCheck.h"

SemanticCheck::SemanticCheck(std::ostream &out):
AstVisitor(), out_(out), curr_while_depth_(0), error_cnt(0), curr_func_scope_(nullptr) {
    addLibFunc();
}

void SemanticCheck::dumpErrorMsg() const {
    for (auto &msg : error_msgs_) {
        std::cout << msg;
    }
}

std::shared_ptr<FuncDefine> SemanticCheck::createLibFuncDefine(BasicType btype, const std::string &funcname) {
    return std::make_shared<FuncDefine>(btype,
                                        std::make_shared<Identifier>(funcname),
                                        std::make_shared<FuncFParams>(),
                                        nullptr);
}

void SemanticCheck::addLibFunc() {

    std::unordered_map<std::string, BasicType> libmap = {
            {"getinit", BasicType::INT_BTYPE},
            {"getfloat", BasicType::FLOAT_BTYPE},
            {"getch", BasicType::INT_BTYPE},
            {"getarray", BasicType::INT_BTYPE},
            {"getfarray", BasicType::FLOAT_BTYPE},
            {"putint", BasicType::VOID_BTYPE},
            {"putfloat", BasicType::VOID_BTYPE},
            {"putch", BasicType::VOID_BTYPE},
            {"starttime", BasicType::VOID_BTYPE},
            {"stoptime", BasicType::VOID_BTYPE},
    };

    for (auto &[func_name, func_btype]: libmap) {
        auto func_def = createLibFuncDefine(func_btype, func_name);
        lib_func_map_.insert({func_name, func_def});
    }

    // 对于其中一些有参数的处理，目前考虑用相同的对象，不知有没有问题
    auto int_formal = std::make_shared<FuncFParam>(BasicType::INT_BTYPE, nullptr);

    auto int_array_formal_ident = std::make_shared<Identifier>("");
    int_array_formal_ident->addDimension(nullptr);      // 一个一维，表示的是int[]
    auto int_array_formal = std::make_shared<FuncFParam>(BasicType::INT_BTYPE, int_array_formal_ident);
    // 补充其中的参数
    lib_func_map_["getarray"]->getFormals()->addFuncFormal(int_array_formal);

    lib_func_map_["putint"]->getFormals()->addFuncFormal(int_formal);

    lib_func_map_["putch"]->getFormals()->addFuncFormal(int_formal);

    lib_func_map_["putarray"]->getFormals()->addFuncFormal(int_formal);
    lib_func_map_["putarray"]->getFormals()->addFuncFormal(int_array_formal);

}

inline void SemanticCheck::dumpSymbolTable() const {
    ident_systable_.dump(out_);
}

bool SemanticCheck::canCalculated(const std::shared_ptr<Expression> &init, double *value) {
    visit(init);
    // 用于根据表达式的不同类型计算出value
    auto expr_type = init->getExprNodeType();
    switch (expr_type) {
        case ExprType::NUMBER_TYPE: {
            auto number_expr = std::dynamic_pointer_cast<Number>(init);
            if (number_expr->getBtype() == BasicType::INT_BTYPE) {
                *value = static_cast<double>(number_expr->getIntValue());
            } else if (number_expr->getBtype() == BasicType::FLOAT_BTYPE) {
                *value = number_expr->getFloatValue();
            } else {
                return false;
            }
            return true;
        }
        case ExprType::BINARY_TYPE: {
            auto binary_expr = std::dynamic_pointer_cast<BinaryExpr>(init);
            double value_1, value_2;
            if (canCalculated(binary_expr->getLeftExpr(), &value_1)
                && canCalculated(binary_expr->getRightExpr(), &value_2)) {
                switch (binary_expr->getOpType()) {
                    case BinaryOpType::GT_OPTYPE:
                        *value = (value_1 > value_2);
                        break;
                    case BinaryOpType::OR_OPTYPE:
                        *value = (value_1 || value_2);
                        break;
                    case BinaryOpType::AND_OPTYPE:
                        *value = (value_1 && value_2);
                        break;
                    case BinaryOpType::NEQ_OPTYPE:
                        *value = (value_1 != value_2);
                        break;
                    case BinaryOpType::EQ_OPTYPE:
                        *value = (value_1 == value_2);
                        break;
                    case BinaryOpType::GTE_OPTYPE:
                        *value = (value_1 >= value_2);
                        break;
                    case BinaryOpType::LT_OPTYPE:
                        *value = (value_1 < value_2);
                        break;
                    case BinaryOpType::LTE_OPTYPE:
                        *value = (value_1 <= value_2);
                        break;
                    case BinaryOpType::MOD_OPTYPE:
                        *value = (static_cast<int32_t>(value_1) % static_cast<int32_t>(value_2));
                        break;
                    case BinaryOpType::MUL_OPTYPE:
                        *value = (value_1 * value_2);
                        break;
                    case BinaryOpType::DIV_OPTYPE:
                        *value = (value_1 / value_2);
                        break;
                    case BinaryOpType::SUB_OPTYPE:
                        *value = (value_1 - value_2);
                        break;
                    case BinaryOpType::ADD_OPTYPE:
                        *value = (value_1 + value_2);
                        break;
                    default:
                        return false;
                }
                return true;
            }
            return false;
        }
        case ExprType::UNARY_TYPE: {
            auto unary_expr = std::dynamic_pointer_cast<UnaryExpr>(init);
            std::cout << "UNARY_TYPE dynatic_cast\n";
            double tmp_value;
            if (canCalculated(unary_expr->getExpr(), &tmp_value)) {
                auto unaryop_type = unary_expr->getOpType();
                switch (unaryop_type) {
                    case UnaryOpType::POSITIVE_OPTYPE: {
                        *value = tmp_value;
                        break;
                    }
                    case UnaryOpType::NEGATIVE_OPTYPE: {
                        *value = -static_cast<int32_t>(tmp_value);
                        break;
                    }
                    case UnaryOpType::NOTOP_OPTYPE: {
                        *value = !static_cast<int32_t>(tmp_value);
                        break;
                    }
                    default: {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
        case ExprType::LVAL_TYPE: {
            // 首先从环境表中查找该id对应的项
            auto lval_expr = std::dynamic_pointer_cast<LvalExpr>(init);
            std::string lval_name = lval_expr->getId()->getId();
            auto entry = ident_systable_.lookupFromAll(lval_name);
            if (entry) {    // 如果能够找到这一项
                if (entry->canCalculated()) {
                    *value = entry->getCalValue();
                    return true;
                }
                appendError(lval_expr.get(), "#The lval expression can't have calculated value\n");
                return false;   // 对于左值无法编译期计算的情况
            }
            appendError(lval_expr.get(), "#The lval expression's identifier not find in sysbol table\n");
            return false;
        }
        default:
            return false;
    }
}

bool SemanticCheck::checkIsValidMain(const FuncDefine *funcdef) {
    return funcdef->id_->getId() == "main" && funcdef->getFormals() == nullptr
        && funcdef->getReturnType() == BasicType::INT_BTYPE;
}

void SemanticCheck::visit(const std::shared_ptr<CompUnit> &compunit) {
    // 首先检查main函数是否存在
    std::unordered_set<std::string> name_set;

    auto func_number = compunit->getFuncDefNumber();
    auto decl_number = compunit->getDeclNumber();

    bool have_main = false;

    ident_systable_.enterScope();       // 进入全局作用域

    // 将标准库里的函数也加入map
    for (auto &lib_func: lib_func_map_) {
        name_set.insert(lib_func.first);
    }
    // 检查该文件中定义过的函数是否有重名的
    for (size_t i = 0; i < func_number; ++i) {
        auto func_def = compunit->getFuncDef(i);
        std::string func_name = func_def->id_->getId();
        if (name_set.find(func_name) != name_set.end()) {       // 如果之前已经存在了
            appendError(compunit.get(), "#Function Identifier Redination,the name is " + func_name + "\n");
        }
        name_set.insert(func_name);
        func_map_.insert({func_name, func_def});
        if (checkIsValidMain(func_def.get())) {
            have_main = true;
        }
    }

    if (!have_main) {
        appendError(compunit.get(), "#Not have a valid main function\n");
    }
    // 遍历一遍，用于检查是否有全局变量的重名
    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        for (auto &def : decl->defs_) {
            std::string name = def->id_->getId();
            bool is_const = (def->getDefType() == DefType::CONSTDEF);
            if (name_set.find(name) != name_set.end()) {        // 重复过的名字
                appendError(def.get(), "#Identifier Redination,the name is " + name + "\n");
            }
            name_set.insert(name);
            // 这个时候还不应该急着加入环境表，应该等到后来decl对define进行检查时再做处理
            /*SymbolEntry new_symbol(decl->type_, false, 0, def->getId(), is_const);
            ident_systable_.addIdent(new_symbol);*/
        }
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        visit(decl);
    }

    for (auto &[func_name, func_def] : func_map_) {
        visit(func_def);
    }

    dumpSymbolTable();
    ident_systable_.exitScope();    // 退出全局作用域
}

void SemanticCheck::visit(const std::shared_ptr<Declare> &decl) {
    for (auto &def : decl->defs_) {
        if (def->getDefType() == DefType::CONSTDEF) {
            checkConstDefine(std::dynamic_pointer_cast<ConstDefine>(def), decl->type_);
        } else if (def->getDefType() == DefType::VARDEF) {
            // std::cout << def->id_->getId() << " begin checkVarDefine\n";
            checkVarDefine(std::dynamic_pointer_cast<VarDefine>(def), decl->type_);
        }
    }
}

void SemanticCheck::visit(const std::shared_ptr<ConstDeclare> &decl) {}

void SemanticCheck::visit(const std::shared_ptr<VarDeclare> &decl) {}

void SemanticCheck::visit(const std::shared_ptr<ConstDefine> &def) {}

void SemanticCheck::checkVarDefine(const std::shared_ptr<VarDefine> &def, BasicType basic_type) {
    auto def_id = def->getId();
    // 首先需要检查的是，在当前作用域之下，有没有重复定义的问题
    bool cancal = false;
    double init_value = 0;

    if (!def_id->getDimensionSize()) {      // 非数组
        auto init_expr = def->getInitExpr();
        if (init_expr) {
            cancal = canCalculated(init_expr, &init_value);
            if (init_expr->expr_type_ == BasicType::VOID_BTYPE) {       // 不合法的类型
                appendError(init_expr.get(), "#The init expression can't be void type\n");
                return;
            }
            if (cancal) {       // 如果可以编译期计算
                auto new_initexpr = (basic_type == BasicType::INT_BTYPE) ?
                                    std::make_shared<Number>(static_cast<int32_t>(init_value))
                                            :std::make_shared<Number>(static_cast<float>(init_value));
                def->init_expr_ = new_initexpr;
            }
        } else if (ident_systable_.isInGlobalScope()) {     // 如果是没有init表达式的全局变量,就将其初始化为0
            auto new_initexpr = (basic_type == BasicType::INT_BTYPE) ?
                    std::make_shared<Number>(static_cast<int32_t>(init_value)) : std::make_shared<Number>(static_cast<float>(init_value));
            def->init_expr_ = new_initexpr;
        }
        ident_systable_.addIdent(SymbolEntry(basic_type, cancal, init_value, def_id.get(), false));
    } else {        // 对于数组类型的判断
        checkArrayVarDefine(def, basic_type);
    }
}

void SemanticCheck::checkConstDefine(const std::shared_ptr<ConstDefine> &def, BasicType basic_type) {
    auto def_id = def->getId();
    bool cancal = false;
    double init_value = 0;
    if (!def_id->getDimensionSize()) {      // 非数组
        auto init_expr = def->getInitExpr();
        if (init_expr) {
            cancal = canCalculated(init_expr, &init_value);
            if (init_expr->expr_type_ == BasicType::VOID_BTYPE) {       // 不合法的类型
                appendError(init_expr.get(), "#The init expression can't be void type\n");
                return;
            }
            if (cancal) {       // 如果可以编译期计算
                auto new_initexpr = (basic_type == BasicType::INT_BTYPE) ?
                                    std::make_shared<Number>(static_cast<int32_t>(init_value))
                                            :std::make_shared<Number>(static_cast<float>(init_value));
                def->init_expr_ = new_initexpr;
            } else if (ident_systable_.isInGlobalScope()) {     // 全局变量 && 不可编译期计算的情况，这种情况应该报错
                appendError(def.get(), "#The global const var must have a calculated init expression\n");
                return;
            }
        } else {     // 对于const类型的来说，必须要有init表达式进行初始化
            appendError(def.get(), "#The global const var must have a init expression\n");
            return;
        }
        ident_systable_.addIdent(SymbolEntry(basic_type, cancal, init_value, def_id.get(), true));
    } else {
        checkArrayVarDefine(def, basic_type);
    }

}

bool SemanticCheck::checkArrayVarDefine(const std::shared_ptr<Define> &def, BasicType basic_type) {
    auto ident = def->getId();
    size_t dim_size = ident->getDimensionSize();

    for (size_t i = 0; i < dim_size; ++i) {
        auto dim_expr = ident->getDimensionExpr(i);
        double value = 0;

        if (!canCalculated(dim_expr, &value)) {
            appendError(def.get(), "#The Array define can't have dimension expression can't be calculated\n");
            return false;
        }

        if (dim_expr->expr_type_ != BasicType::INT_BTYPE) {
            appendError(def.get(), "#The Array define number not INT_TYPE\n");
            return false;
        }

        if (value < 0) {
            appendError(def.get(), "#The Array dimension number < 0\n");
            return false;
        }
        // 重新设置ident
        ident->setDimensionExpr(i, std::make_shared<Number>(static_cast<int32_t>(value)));
    }

    std::string id_name = ident->getId();
    // 然后需要查找符号表，判断是否有重复的元素
    if (ident_systable_.lookupFromCurrScope(id_name)) {
        appendError(def.get(), "#The Identifier of this array " + id_name + " has defined\n");
        return false;
    }
    // 对于const类型来说，必须要有init表达式
    if (def->getDefType() == DefType::CONSTDEF && !def->init_expr_) {
        appendError(def.get(), "#The Const ArrayVal " + id_name + " not has a init expression\n");
        return false;
    }

    if (def->init_expr_) {
        // 对于数组初始化list表达式进行的语义分析
        visit(def->init_expr_);
        if (def->init_expr_->is_error_) {
            return false;
        }
    }

    ident_systable_.addIdent(SymbolEntry(basic_type, false, 0, ident.get(), def->getDefType() == DefType::CONSTDEF));
    return true;
}

void SemanticCheck::visit(const std::shared_ptr<VarDefine> &def) {}

void SemanticCheck::visit(const std::shared_ptr<FuncDefine> &def) {
    // 首先创建新的作用域
    ident_systable_.enterScope();
    curr_func_scope_ = def.get();

    std::unordered_set<std::string> formal_name_set;
    std::string func_def_name = def->id_->getId();

    size_t formal_size = 0;
    if (def->getFormals()) {
        formal_size = def->getFormals()->getFormalSize();
    }

    for (size_t i = 0; i < formal_size; ++i) {       // 函数形参
        auto formal = def->getFormals()->getFuncFormal(i);
        // 检查是否有重名的
        std::string formal_name = formal->getFormalId()->getId();
        if (formal_name_set.find(formal_name) != formal_name_set.end()) {
            appendError(def.get(), "#the formal name invalid in function " + func_def_name + ".\n");
            return;
        }
        formal_name_set.insert(formal_name);
        // 加入到符号表中,这些值都是不可以执行编译期计算的
        auto formal_ident = formal->getFormalId();
        size_t dim_size = formal_ident->getDimensionSize();
        if (dim_size) {
            for (size_t j = 0; j < dim_size; ++j) {
                auto dim_expr = formal_ident->getDimensionExpr(j);
                if (!dim_expr) {
                    continue;
                }
                double dim_value = 0;
                if (!canCalculated(dim_expr, &dim_value)) {     // 要求必须是可以编译期计算的
                    appendError(def.get(), "#The Array Formal " + formal_name + " in Funcion " + func_def_name + " must has calculated dimension\n");
                    return;
                }
                // 重新设置这其中的dimension表达式
                formal_ident->setDimensionExpr(j, std::make_shared<Number>(static_cast<int32_t>(dim_value)));
            }
        }
        SymbolEntry new_symbol(formal->getBtype(), false, 0, formal->getFormalId().get(), false);
        ident_systable_.addIdent(new_symbol);
    }
    // 之后分析block部分的代码
    visit(def->getBlock());
    curr_func_scope_ = nullptr;
    dumpSymbolTable();
    ident_systable_.exitScope();

}

void SemanticCheck::visit(const std::shared_ptr<Expression> &expr) {
    ExprType type = expr->getExprNodeType();
    switch (type) {
        case ExprType::UNARY_TYPE:
            visit(std::dynamic_pointer_cast<UnaryExpr>(expr));
            return;
        case ExprType::BINARY_TYPE:
            visit(std::dynamic_pointer_cast<BinaryExpr>(expr));
            return;
        case ExprType::NUMBER_TYPE:
            visit(std::dynamic_pointer_cast<Number>(expr));
            return;
        case ExprType::ARRAYVALUE_TYPE:
            visit(std::dynamic_pointer_cast<ArrayValue>(expr));
            return;
        case ExprType::LVAL_TYPE:
            visit(std::dynamic_pointer_cast<LvalExpr>(expr));
            return;
        case ExprType::CALLFUNC_TYPE:
            visit(std::dynamic_pointer_cast<CallFuncExpr>(expr));
            return;
    }
}

void SemanticCheck::visit(const std::shared_ptr<UnaryExpr> &expr) {
    visit(expr->getExpr());
    expr->expr_type_ = expr->getExpr()->expr_type_;
}

void SemanticCheck::visit(const std::shared_ptr<BinaryExpr> &expr) {
    auto left = expr->getLeftExpr();
    auto right = expr->getRightExpr();

    visit(left);
    visit(right);

    if (left->expr_type_ == right->expr_type_) {
        expr->expr_type_ = left->expr_type_;
        if (expr->expr_type_ == BasicType::VOID_BTYPE) {
            appendError(expr.get(), "#The left and right can't be void.\n");
        }
    } else {
        expr->expr_type_ = BasicType::INT_BTYPE;
    }
}

void SemanticCheck::visit(const std::shared_ptr<BlockItems> &stmt) {
    size_t item_size = stmt->getItemSize();
    ident_systable_.enterScope();
    for (size_t i = 0; i < item_size; ++i) {
        auto item = stmt->getBlockItem(i);
        if (item) {
            visit(item);
        }
    }
    dumpSymbolTable();
    ident_systable_.exitScope();
}

void SemanticCheck::visit(const std::shared_ptr<BlockItem> &stmt) {
    auto item_stmt = stmt->getStmt();
    if (item_stmt) {
        visit(item_stmt);
    }
}

void SemanticCheck::visit(const std::shared_ptr<AssignStatement> &stmt) {
    // 首先求出左半部分
    auto left = stmt->getLeftExpr();
    visit(left);
    auto lval = std::dynamic_pointer_cast<LvalExpr>(left);
    std::string name = lval->getId()->getId();
    // 查找符号表,判断是否是const
    auto find_lval = ident_systable_.lookupFromAll(name);
    if (find_lval && find_lval->isConst()) {  // 如果是存在的
        appendError(stmt.get(), "#lval in assignment statement can't be const type\n");
    }
    // 然后求出右半部分的
    auto right = stmt->getRightExpr();
    visit(right);
}

void SemanticCheck::visit(const std::shared_ptr<IfElseStatement> &stmt) {
    auto cond_expr = stmt->getCond();
    if (cond_expr) {
        visit(cond_expr);
    }
    if (cond_expr->expr_type_ == BasicType::VOID_BTYPE) {
        appendError(stmt.get(), "#The condition in if-else can't be void type\n");
    }
    auto ifstmt = stmt->getIfStmt();
    if (ifstmt) {
        visit(ifstmt);
    }
    auto elsestmt = stmt->getElseStmt();
    if (elsestmt) {
        visit(elsestmt);
    }

}

void SemanticCheck::visit(const std::shared_ptr<WhileStatement> &stmt) {
    auto cond_expr = stmt->getCond();
    if (cond_expr) {
        visit(cond_expr);
    }

    if (cond_expr->expr_type_ == BasicType::VOID_BTYPE) {
        appendError(stmt.get(), "#The condition in while can't be void type");
    }

    auto blockstmt = stmt->getStatement();
    if (blockstmt) {
        // 进入一层while的作用域
        curr_while_depth_++;
        visit(blockstmt);
        curr_while_depth_--;
    }

}

void SemanticCheck::visit(const std::shared_ptr<BreakStatement> &stmt) {
    if (!isInWhile()) {
        appendError(stmt.get(), "#Break statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<ContinueStatement> &stmt) {
    if (!isInWhile()) {
        appendError(stmt.get(), "#Continue statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<CallFuncExpr> &expr) {
    // 首先查找该函数的identifier是否存在
    std::string func_name = expr->getFuncId()->getId();
    auto find_func = func_map_.find(func_name);
    if (find_func == func_map_.end()) {     // 如果该函数不存在
        // 如果从定义的函数中，找不到，就找一找库函数中有没有
        auto find_libfunc = lib_func_map_.find(func_name);
        if (find_libfunc == lib_func_map_.end()) {
            appendError(expr.get(), "#Call a function " + func_name + " not exist.\n");
            return;
            // 表明lib函数中也没有
        }
        // 说明属于lib函数,需要作出lib函数的处理，不同于一般定义的函数

        return;
    }

    // 首先检查参数的数量是否是匹配的
    auto func_def = find_func->second;
    expr->expr_type_ = func_def->getReturnType();

    size_t actual_size = expr->getActualSize();
    size_t formal_size = 0;
    if (func_def->getFormals()) {
        formal_size = func_def->getFormals()->getFormalSize();
    }
    if (actual_size != formal_size) {
        appendError(expr.get(), "#The size of actuals not equal the size of formals\n");
        return;
    }
    // 通过环境表获取到表项之后,需要检查参数是否是匹配的
    for (size_t i = 0; i < actual_size; ++i) {
        auto actual_expr = expr->getActual(i);
        visit(actual_expr);
    }

}

void SemanticCheck::visit(const std::shared_ptr<ReturnStatement> &stmt) {
    if (!curr_func_scope_) {
        appendError(stmt.get(), "#The return statement not in a function scope\n");
    } else {
        auto ret_type = curr_func_scope_->getReturnType();
        std::string func_name = curr_func_scope_->getId()->getId();
        auto ret_expr = stmt->getExpr();
        /*if (ret_type != BasicType::VOID_BTYPE && !ret_expr) {    // 返回类型为int或者double时，表达式不应该为null
            appendError("#The function " + func_name + " can't have a return statement with null\n");
        } else*/
        if (ret_type == BasicType::VOID_BTYPE && ret_expr) {
            appendError(stmt.get(), "The function(VOID) can't have a return statement(expr)\n");
        }
    }
}

void SemanticCheck::visit(const std::shared_ptr<Number> &number) {
    number->expr_type_ = number->getBtype();
}

void SemanticCheck::visit(const std::shared_ptr<LvalExpr> &expr) {
    // 首先检查其对应的name是否存在
    auto ident = expr->getId();
    auto symbol_entry = ident_systable_.lookupFromAll(ident->getId());
    if (symbol_entry) { // 如果存在
        bool is_array = symbol_entry->isArray();
        size_t dimension_size = expr->getId()->getDimensionSize();
        /*
        if (is_array && expr->getId()->getDimensionSize() != dimension_size) {  // 如果时数组类型
            appendError(expr.get(), "#The LvalExpr array size error\n");
        } else if (!is_array && dimension_size > 0) {      // 单变量但是存在维度
            appendError(expr.get(), "#The LvalExpr should't have dimension\n");
        } else {    // 合法的情况
            expr->expr_type_ = symbol_entry->getType();
        }*/
        if (is_array) {
            if (expr->getId()->getDimensionSize() != dimension_size) {
                appendError(expr.get(), "#The LvalExpr array size error\n");
                return;
            }
            // 检查其数组的纬度数是否是可以计算的
            size_t array_size = expr->getId()->getDimensionSize();
            for (size_t i = 0; i < array_size; ++i) {
                auto array_dim_expr = expr->getId()->getDimensionExpr(i);
                double dim_value = 0;
                if (canCalculated(array_dim_expr, &dim_value)) {
                    // 重新设置
                    expr->getId()->setDimensionExpr(i, std::make_shared<Number>(static_cast<int32_t>(dim_value)));
                }
            }
            expr->expr_type_ = symbol_entry->getType();

        } else if (dimension_size > 0) {
            appendError(expr.get(), "#The LvalExpr should't have dimension\n");
            return;
        }

    } else {
        appendError(expr.get(), "#The LvalExpr named " + ident->getId() + " not declared\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<Statement> &stmt) {
    auto stmt_type = stmt->getStmtType();
    switch (stmt_type) {
        case ASSIGN_STMTTYPE:
            visit(std::dynamic_pointer_cast<AssignStatement>(stmt));
            return;
        case BLOCKITEMS_STMTTYPE:
            visit(std::dynamic_pointer_cast<BlockItems>(stmt));
            return;
        case IFELSE_STMTTYPE:
            visit(std::dynamic_pointer_cast<IfElseStatement>(stmt));
            return;
        case WHILE_STMTTYPE:
            visit(std::dynamic_pointer_cast<WhileStatement>(stmt));
            return;
        case BREAK_STMTTYPE:
            visit(std::dynamic_pointer_cast<BreakStatement>(stmt));
            return;
        case CONTINUE_STMTTYPE:
            visit(std::dynamic_pointer_cast<ContinueStatement>(stmt));
            return;
        case RETURN_STMTTYPE:
            visit(std::dynamic_pointer_cast<ReturnStatement>(stmt));
            return;
        case DECL_STMTTYPE:
            visit(std::dynamic_pointer_cast<Declare>(stmt));
            return;
        default:
            return;
    }
}

void SemanticCheck::visit(const std::shared_ptr<ArrayValue> &arrayval) {

}

// 千万不要从裸指针生成shared_ptr之后带入函数，这样容易造成意外地内存释放
// 因此一旦用了shared_ptr很容易需要都用shared_ptr