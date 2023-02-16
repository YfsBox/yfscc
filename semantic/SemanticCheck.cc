//
// Created by 杨丰硕 on 2023/2/15.
//
#include <unordered_set>
#include "SemanticCheck.h"

void SemanticCheck::addLibFunc() {


}

bool SemanticCheck::checkIsValidMain(const FuncDefine *funcdef) {
    return funcdef->id_->getId() == "main" && funcdef->getFormals()->getFormalSize() == 0
        && funcdef->getReturnType() == BasicType::INT_BTYPE;
}

void SemanticCheck::visit(const std::shared_ptr<CompUnit> &compunit) {
    // 首先检查main函数是否存在
    std::unordered_set<std::string> name_set;

    auto func_number = compunit->getFuncDefNumber();
    auto decl_number = compunit->getDeclNumber();

    bool have_main = false;

    for (size_t i = 0; i < func_number; ++i) {
        auto func_def = compunit->getFuncDef(i);
        std::string func_name = func_def->id_->getId();
        if (name_set.find(func_name) != name_set.end()) {       // 如果之前已经存在了
            appendError("#Function Identifier Redination,the name is " + func_name + "\n");
        }
        name_set.insert(func_name);
        func_map_.insert({func_name, func_def});
        if (checkIsValidMain(func_def)) {
            have_main = true;
        }
    }

    if (!have_main) {
        appendError("#Not have a valid main function\n");
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        for (auto &def : decl->defs_) {
            std::string name = def->id_->getId();
            if (name_set.find(name) != name_set.end()) {        // 重复过的名字
                appendError("#Identifier Redination,the name is " + name + "\n");
            }
            name_set.insert(name);
        }
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        visit(DeclarePtr(decl));
    }

}

void SemanticCheck::visit(const std::shared_ptr<Declare> &decl) {
    for (auto &def : decl->defs_) {
        if (def->getDefType() == DefType::CONSTDEF) {
            visit(std::dynamic_pointer_cast<ConstDefine>(def));
        } else if (def->getDefType() == DefType::VARDEF) {
            visit(std::dynamic_pointer_cast<VarDefine>(def));
        }
    }
}

void SemanticCheck::visit(const std::shared_ptr<ConstDefine> &def) {


}

void SemanticCheck::visit(const std::shared_ptr<VarDefine> &def) {


}

void SemanticCheck::visit(const std::shared_ptr<FuncDefine> &def) {
    // 首先创建新的作用域
    ident_systable_.enterScope();
    curr_func_scope_ = def.get();
    std::unordered_set<std::string> formal_name_set;
    for (size_t i = 0; i < def->getFormals()->getFormalSize(); ++i) {
        auto formal = def->getFormals()->getFuncFormal(i);
        // 检查是否有重名的
        std::string formal_name = formal->getFormalId()->getId();
        if (formal_name_set.find(formal_name) != formal_name_set.end()) {
            appendError("#the formal name invalid in function " + def->id_->getId() + ".\n");
        }
        formal_name_set.insert(formal_name);
        // 加入到符号表中
        SymbolEntry new_symbol(formal->getBtype(), formal->getFormalId(), false);
        ident_systable_.addIdent(new_symbol);
    }
    // 之后分析block部分的代码

    curr_func_scope_ = nullptr;
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
    visit(ExpressionPtr(expr->getExpr()));
    expr->expr_type_ = expr->getExpr()->expr_type_;
}

void SemanticCheck::visit(const std::shared_ptr<BinaryExpr> &expr) {
    auto left = expr->getLeftExpr();
    auto right = expr->getRightExpr();

    visit(ExpressionPtr(left));
    visit(ExpressionPtr(right));

    if (left->expr_type_ == right->expr_type_) {
        expr->expr_type_ = left->expr_type_;
        if (expr->expr_type_ == BasicType::VOID_BTYPE) {
            appendError("#The left and right can't be void.\n");
        }
    } else {
        expr->expr_type_ = BasicType::INT_BTYPE;
    }
}

void SemanticCheck::visit(const std::shared_ptr<AssignStatement> &stmt) {
    // 首先求出左半部分
    auto left = stmt->getLeftExpr();
    visit(ExpressionPtr(left));
    LvalExpr *lval = dynamic_cast<LvalExpr*>(left);
    std::string name = lval->getId()->getId();
    // 查找符号表,判断是否是const
    auto find_lval = ident_systable_.lookupFromAll(name);
    if (find_lval && find_lval->isConst()) {  // 如果是存在的
        appendError("#lval in assignment statement can't be const type\n");
    }
    // 然后求出右半部分的
    auto right = stmt->getRightExpr();
    visit(ExpressionPtr(right));
}

void SemanticCheck::visit(const std::shared_ptr<IfElseStatement> &stmt) {
    auto cond_expr = stmt->getCond();
    visit(ExpressionPtr(cond_expr));
    if (cond_expr->expr_type_ == BasicType::VOID_BTYPE) {
        appendError("#The condition in if-else can't be void type\n");
    }



}

void SemanticCheck::visit(const std::shared_ptr<WhileStatement> &stmt) {


}

void SemanticCheck::visit(const std::shared_ptr<BreakStatement> &stmt) {
    if (!isInWhile()) {
        appendError("#Break statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<ContinueStatement> &stmt) {
    if (!isInWhile()) {
        appendError("#Continue statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<CallFuncExpr> &expr) {
    // 首先查找该函数的identifier是否存在
    std::string func_name = expr->getFuncId()->getId();
    auto find_func = func_map_.find(func_name);
    if (find_func == func_map_.end()) {     // 如果该函数不存在
        appendError("#Call a function " + func_name + " not exist.\n");
        return;
    }
    // 检查是否是标准库中的函数，待补充


    // 首先检查参数的数量是否是匹配的
    FuncDefine *func_def = find_func->second;
    expr->expr_type_ = func_def->getReturnType();

    size_t actual_size = expr->getActualSize();
    size_t formal_size = func_def->getFormals()->getFormalSize();
    if (actual_size != formal_size) {
        appendError("#The size of actuals not equal the size of formals\n");
        return;
    }
    // 通过环境表获取到表项之后,需要检查参数是否是匹配的
    for (size_t i = 0; i < actual_size; ++i) {
        auto actual_expr = expr->getActual(i);
        visit(ExpressionPtr(actual_expr));
    }

}

void SemanticCheck::visit(const std::shared_ptr<ReturnStatement> &stmt) {
    if (!curr_func_scope_) {
        appendError("#The return statement not in a function scope\n");
    } else {
        auto ret_type = curr_func_scope_->getReturnType();
        std::string func_name = curr_func_scope_->getId()->getId();
        auto ret_expr = stmt->getExpr();
        /*if (ret_type != BasicType::VOID_BTYPE && !ret_expr) {    // 返回类型为int或者float时，表达式不应该为null
            appendError("#The function " + func_name + " can't have a return statement with null\n");
        } else*/
        if (ret_type == BasicType::VOID_BTYPE && ret_expr) {
            appendError("The function(VOID) can't have a return statement(expr)\n");
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
        if (is_array && expr->getId()->getDimensionSize() != dimension_size) {  // 如果时数组类型
            appendError("#The LvalExpr array size error\n");
        } else if (!is_array && dimension_size > 0) {      // 单变量但是存在维度
            appendError("#The LvalExpr should't have dimension\n");
        } else {    // 合法的情况
            expr->expr_type_ = symbol_entry->getType();
        }
    } else {
        appendError("#The LvalExpr named " + ident->getId() + " not declared\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<Statement> &stmt) {

}