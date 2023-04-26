//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include <memory>
#include "../common/Ast.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Module.h"
#include "GlobalVariable.h"
#include "Instruction.h"
#include "IrBuilder.h"
#include "IrFactory.h"
#include "IrDumper.h"

IrBuilder::IrBuilder(std::ostream &out):
    is_deal_cond_(false),
    module_(std::make_unique<Module>()),
    context_(std::make_unique<IrContext>(module_.get())),
    dumper_(std::make_unique<IrDumper>(out)),
    curr_decl_(nullptr),
    curr_value_(nullptr),
    curr_local_array_(nullptr){
    IrFactory::InitContext(context_.get());
}

IrBuilder::~IrBuilder() = default;

void IrBuilder::addValueCast(Value *left, Value *right) {


}

void IrBuilder::enableDealCond() {
    is_deal_cond_ = true;
}

void IrBuilder::disableDealCond() {
    is_deal_cond_ = false;
}

void IrBuilder::initJumpMap() {
    true_jump_map_.clear();
    false_jump_map_.clear();
}

void IrBuilder::addInstruction(Value *inst) {
    context_->curr_bb_->addInstruction(inst);
}

void IrBuilder::addBasicBlock(Value *bb) {
    context_->curr_function_->addBasicBlock(bb);
}

void IrBuilder::visit(const std::shared_ptr<Declare> &decl) {
    curr_decl_ = decl.get();
    for (auto &def : decl->defs_) {
        if (def->getDefType() == DefType::CONSTDEF) {
            visit(std::dynamic_pointer_cast<ConstDefine>(def));
        } else if (def->getDefType() == DefType::VARDEF) {
            visit(std::dynamic_pointer_cast<VarDefine>(def));
        }
    }
    curr_decl_ = nullptr;
}
// 在这里的处理中，一般都是指针类型
void IrBuilder::visit(const std::shared_ptr<LvalExpr> &expr) {          // 有可能是取地址,
    auto lval_ident = expr->getId();
    auto find_entry = var_symbol_table_.lookupFromAll(lval_ident->getId());
    assert(find_entry);
    size_t dimension_size = lval_ident->getDimensionSize();
    if (dimension_size) {       // 对于数组类型的处理,不过应该区分函数参数中的数组和一般定义的数组
        // 对其各唯独的参数
        Value *dimension_value = nullptr;
        std::vector<Value *> dimension_numbers;
        dimension_numbers.reserve(dimension_size);
        printf("the dimension size is %ld\n", dimension_size);
        for (size_t i = 0; i < dimension_size; ++i) {
            visit(lval_ident->getDimensionExpr(i));
            dimension_value = curr_value_;
            if (dimension_value->isPtr()) {
                dimension_value = IrFactory::createILoadInstruction(dimension_value);
                addInstruction(dimension_value);
            }
            // printf("the dimension size is %d\n", dimension_value->)
            dimension_numbers.push_back(dimension_value);
        }
        printf("begin gep inst value");
        auto gep_inst_value = find_entry->getBasicType() == BasicType::INT_BTYPE ?
                IrFactory::createIGEPInstruction(find_entry->getValue(), dimension_numbers):
                IrFactory::createFGEPInstruction(find_entry->getValue(), dimension_numbers);
        addInstruction(gep_inst_value);
        setCurrValue(gep_inst_value);
    } else {
        Value *entry_value = find_entry->getValue();
        assert(find_entry);
        setCurrValue(entry_value);      // 这个时候得到的既有可能是指针,也有可能就是值
    }
}

void IrBuilder::visit(const std::shared_ptr<Number> &number) {
    if (number->getBtype() == BasicType::INT_BTYPE) {
        setCurrValue(IrFactory::createIConstantVar(number->getIntValue()));
    } else {
        setCurrValue(IrFactory::createFConstantVar(number->getFloatValue()));
    }
}

void IrBuilder::setGlobalArrayInitValue(const std::shared_ptr<ArrayValue> &array_init_value,
                                        ConstantArray *const_array) {
    if (curr_decl_->type_ == BasicType::INT_BTYPE) {
        std::map<int32_t, int32_t> init_values;
        array_init_value->getIndexValueMap(init_values);
        for (auto &[idx, initvalue]: init_values) {
            const_array->setInitValue(idx, initvalue);
        }
    } else {
        std::map<int32_t, float> init_values;
        array_init_value->getIndexValueMap(init_values);
        for (auto &[idx, initvalue]: init_values) {
            const_array->setInitValue(idx, initvalue);
        }
    }
}

static size_t getArrayLen(const std::vector<int32_t> &dimension_numbers) {
    size_t array_len = 1;
    for (auto dimension: dimension_numbers) {
        array_len *= dimension;
    }
    return array_len;
}

void IrBuilder::visit(const std::shared_ptr<ConstDefine> &def) {
    BasicType def_basic_type = curr_decl_->type_;
    std::string var_name = def->getId()->getId();
    if (def->getId()->getDimensionSize()) {
        std::vector<int32_t> dimension_number;
        getDimensionNumber(def, dimension_number);
        if (var_symbol_table_.isInGlobalScope()) {
            auto new_const_array = curr_decl_->type_ == BasicType::FLOAT_BTYPE
                                   ? IrFactory::createFConstantArray(dimension_number, var_name) :
                                   IrFactory::createIConstantArray(dimension_number, var_name);
            auto const_array = dynamic_cast<ConstantArray *>(new_const_array);
            assert(const_array);
            auto new_global_array = curr_decl_->type_ == BasicType::FLOAT_BTYPE
                                    ? IrFactory::createFGlobalArray(true, const_array, var_name) :
                                    IrFactory::createIGlobalArray(true, const_array, var_name);
            auto globalvar_ptr = dynamic_cast<GlobalVariable *>(new_global_array);
            // 设置init value
            auto array_init_value_expr = std::dynamic_pointer_cast<ArrayValue>(def->getInitExpr());
            assert(array_init_value_expr);
            setGlobalArrayInitValue(array_init_value_expr, const_array);
            printf("add const global to module %s\n", globalvar_ptr->getName().c_str());
            context_->curr_module_->addGlobalVariable(std::unique_ptr<GlobalVariable>(globalvar_ptr));
            assert(globalvar_ptr);
            IrSymbolEntry new_entry(true, def_basic_type, new_global_array);
            var_symbol_table_.addIdent(new_entry);
        } else {
            auto array_len = getArrayLen(dimension_number);
            Value *allac_inst_value = curr_decl_->type_ == BasicType::INT_BTYPE ?
                    IrFactory::createIArrayAllocaInstruction(dimension_number, var_name) : IrFactory::createFArrayAllocaInstruction(dimension_number, var_name);
            context_->curr_bb_->addInstruction(allac_inst_value);
            curr_local_array_ = allac_inst_value;
            // 处理初始化赋值的情况,递归地初始化列表进行处理,
            if (def->getInitExpr()) {
                visit(def->getInitExpr());
            }
            curr_value_ = allac_inst_value;
            IrSymbolEntry new_entry(true, def_basic_type, allac_inst_value);
            var_symbol_table_.addIdent(new_entry);
        }
    } else {
        if (var_symbol_table_.isInGlobalScope()) {

            std::unique_ptr<GlobalVariable> new_global = nullptr;

            auto init_value = std::dynamic_pointer_cast<Number>(def->getInitExpr());
            Value *new_value = nullptr;
            assert(init_value);

            if (def_basic_type == BasicType::INT_BTYPE) {
                new_value = IrFactory::createConstIGlobalVar(init_value->getIntValue(), var_name);
            } else if (def_basic_type == BasicType::FLOAT_BTYPE) {
                new_value = IrFactory::createConstFGlobalVar(init_value->getFloatValue(), var_name);
            }
            new_global = std::unique_ptr<GlobalVariable> (dynamic_cast<GlobalVariable *>(new_value));
            // 加入到module的集合中
            module_->addGlobalVariable(std::move(new_global));
            IrSymbolEntry new_entry(true, def_basic_type, new_value);
            var_symbol_table_.addIdent(new_entry);
        } else {
            // alloca
            Value *alloca_inst_value = nullptr;
            if (def_basic_type == BasicType::INT_BTYPE) {
                alloca_inst_value = IrFactory::createIAllocaInstruction(var_name);
            } else {
                alloca_inst_value = IrFactory::createFAllocaInstruction(var_name);
            }
            // visit求出初始值,const的局部变量一定存在不可能为null
            visit(def->init_expr_);
            // 如果类型不一样,就需要进行转化
            BasicType init_expr_type = def->init_expr_->expr_type_;
            Value *cast_inst_value = nullptr;
            Value *init_value = curr_value_;        // 该结果也正是从visit中的所求出的value
            if (init_expr_type == BasicType::INT_BTYPE && def_basic_type == BasicType::FLOAT_BTYPE) {
                cast_inst_value = IrFactory::createI2FCastInstruction(init_value);
            } else if (init_expr_type == BasicType::FLOAT_BTYPE && def_basic_type == BasicType::INT_BTYPE) {
                cast_inst_value = IrFactory::createF2ICastInstruction(init_value);
            }
            if (cast_inst_value) {
                init_value = cast_inst_value;
            }
            // store
            Value *store_inst_value = nullptr;
            if (def_basic_type == BasicType::INT_BTYPE) {
                store_inst_value = IrFactory::createIStoreInstruction(init_value, alloca_inst_value);
            } else {
                store_inst_value = IrFactory::createFStoreInstruction(init_value, alloca_inst_value);
            }
            // 最后将指令加入到当前的basic block
            addInstruction(alloca_inst_value);
            if (cast_inst_value) {
                addInstruction(cast_inst_value);
            }
            // context_->curr_bb_->addInstruction(store_inst_value);
            addInstruction(store_inst_value);
            // 不要忘了将变量加入符号表
            IrSymbolEntry new_entry(true, def_basic_type, alloca_inst_value);
            var_symbol_table_.addIdent(new_entry);
        }

    }
}

void IrBuilder::getDimensionNumber(const std::shared_ptr<Define> &def, std::vector<int32_t> &dimension_number) {
    auto dimension_size = def->getId()->getDimensionSize();
    dimension_number.resize(dimension_size);
    for (int i = 0; i < dimension_size; ++i) {
        auto number =
                std::dynamic_pointer_cast<Number>(def->getId()->getDimensionExpr(i));
        assert(number);
        dimension_number[i] = number->getIntValue();
    }
}

void IrBuilder::visit(const std::shared_ptr<VarDefine> &def) {
    // 首先考虑是否是数组的情况
    BasicType def_basic_type = curr_decl_->type_;
    std::string var_name = def->getId()->getId();
    if (def->getId()->getDimensionSize()) {     // 数组类型的
        std::vector<int32_t> dimension_number;
        getDimensionNumber(def, dimension_number);
        if (var_symbol_table_.isInGlobalScope()) {
            auto new_const_array = curr_decl_->type_ == BasicType::FLOAT_BTYPE
                    ? IrFactory::createFConstantArray(dimension_number, var_name) :
                    IrFactory::createIConstantArray(dimension_number, var_name);
            auto const_array = dynamic_cast<ConstantArray *>(new_const_array);
            assert(const_array);
            auto new_global_array = curr_decl_->type_ == BasicType::FLOAT_BTYPE
                    ? IrFactory::createFGlobalArray(false, const_array, var_name) :
                    IrFactory::createIGlobalArray(false, const_array, var_name);
            auto globalvar_ptr = dynamic_cast<GlobalVariable *>(new_global_array);
            // 设置init value
            auto array_init_value_expr = std::dynamic_pointer_cast<ArrayValue>(def->getInitExpr());
            assert(array_init_value_expr);
            setGlobalArrayInitValue(array_init_value_expr, const_array);
            printf("add global to module %s\n", globalvar_ptr->getName().c_str());
            context_->curr_module_->addGlobalVariable(std::unique_ptr<GlobalVariable>(globalvar_ptr));
            IrSymbolEntry new_entry(false, def_basic_type, new_global_array);
            var_symbol_table_.addIdent(new_entry);
        } else {
            auto array_len = getArrayLen(dimension_number);
            Value *allac_inst_value = curr_decl_->type_ == BasicType::INT_BTYPE ?
                                      IrFactory::createIArrayAllocaInstruction(dimension_number, var_name) : IrFactory::createFArrayAllocaInstruction(dimension_number, var_name);
            context_->curr_bb_->addInstruction(allac_inst_value);
            curr_local_array_ = allac_inst_value;
            // 处理初始化赋值的情况,递归地初始化列表进行处理,
            if (def->getInitExpr()) {
                visit(def->getInitExpr());
            }
            curr_value_ = allac_inst_value;
            IrSymbolEntry new_entry(false, def_basic_type, allac_inst_value);
            var_symbol_table_.addIdent(new_entry);
        }
    } else {        // 不是数组
        if (var_symbol_table_.isInGlobalScope()) {      // 是否处于全局作用域
            std::unique_ptr<GlobalVariable> new_global = nullptr;

            auto init_value = std::dynamic_pointer_cast<Number>(def->getInitExpr());
            Value *new_value = nullptr;
            assert(init_value);

            if (def_basic_type == BasicType::INT_BTYPE) {
                new_value = IrFactory::createIGlobalVar(var_name, init_value->getIntValue());
            } else if (def_basic_type == BasicType::FLOAT_BTYPE) {
                new_value = IrFactory::createFGlobalVar(var_name, init_value->getFloatValue());
            }
            new_global = std::unique_ptr<GlobalVariable> (dynamic_cast<GlobalVariable *>(new_value));
            module_->addGlobalVariable(std::move(new_global));          // 加入到module的集合中

            IrSymbolEntry new_entry(false, def_basic_type, new_value);
            var_symbol_table_.addIdent(new_entry);
        } else {
            Value *alloca_inst_value = nullptr;
            if (def_basic_type == BasicType::INT_BTYPE) {
                alloca_inst_value = IrFactory::createIAllocaInstruction(var_name);
            } else {
                alloca_inst_value = IrFactory::createFAllocaInstruction(var_name);
            }       // 所返回的东西视为一个地址

            if (def->hasInitExpr()) {
                visit(def->init_expr_);

                BasicType init_expr_type = def->init_expr_->expr_type_;
                Value *cast_inst_value = nullptr;
                Value *init_value = curr_value_;
                if (init_expr_type == BasicType::INT_BTYPE && def_basic_type == BasicType::FLOAT_BTYPE) {
                    cast_inst_value = IrFactory::createI2FCastInstruction(curr_value_);
                } else if (init_expr_type == BasicType::FLOAT_BTYPE && def_basic_type == BasicType::INT_BTYPE) {
                    cast_inst_value = IrFactory::createF2ICastInstruction(curr_value_);
                }
                if (cast_inst_value) {
                    init_value = cast_inst_value;
                }

                Value *store_inst_value = nullptr;
                if (def_basic_type == BasicType::INT_BTYPE) {
                    store_inst_value = IrFactory::createIStoreInstruction(init_value, alloca_inst_value);
                } else {
                    store_inst_value = IrFactory::createFStoreInstruction(init_value, alloca_inst_value);
                }
                // 最后将指令加入到当前的basic block
                addInstruction(alloca_inst_value);
                if (cast_inst_value) {
                    addInstruction(cast_inst_value);
                }
                addInstruction(store_inst_value);
                // 不要忘了将变量加入符号表
            } else {
                addInstruction(alloca_inst_value);
            }
            IrSymbolEntry new_entry(true, def_basic_type, alloca_inst_value);
            var_symbol_table_.addIdent(new_entry);
        }

    }

}

void IrBuilder::visit(const std::shared_ptr<BlockItem> &stmt) {

}
// TODO：还没有实现数组类型参数的处理
void IrBuilder::visit(const std::shared_ptr<FuncDefine> &def) {
    Value *function_value = nullptr;
    BasicType ret_type = def->getReturnType();
    std::string function_name = def->getId()->getId();

    std::vector<std::string> param_names;
    std::vector<BasicType> param_types;
    std::vector<std::vector<int32_t>> param_dimensions;
    std::vector<Value *> arguments;

    auto formals = def->getFormals();
    size_t formal_size = formals != nullptr ? formals->getFormalSize() : 0;
    param_names.reserve(formal_size);
    param_types.reserve(formal_size);
    param_dimensions.reserve(formal_size);
    arguments.reserve(formal_size);

    for (size_t i = 0; i < formal_size; ++i) {          // 根据Ast中的Formal解析出Argument
        auto formal = formals->getFuncFormal(i);
        std::string formal_name = formal->getFormalId()->getId();
        param_names.emplace_back(formal_name);
        param_types.emplace_back(formal->getBtype());
        std::vector<int32_t> dimension;
        auto formal_id = formal->getFormalId();
        auto dimension_size = formal_id->getDimensionSize();
        for (size_t j = 0; j < dimension_size; ++j) {
            if (j == 0) {
                dimension.emplace_back(0);
            } else {
                auto dimension_number = std::dynamic_pointer_cast<Number> (formal_id->getDimensionExpr(j));
                assert(dimension_number);
                dimension.emplace_back(dimension_number->getIntValue());
            }
        }
        param_dimensions.emplace_back(dimension);
        if (dimension.empty()) {        // 目前的function首先设置为nullptr
            arguments.push_back(IrFactory::createArgument(formal->getBtype() == BasicType::FLOAT_BTYPE, nullptr, formal_name));
        } else {
            arguments.push_back(IrFactory::createArrayArgument(formal->getBtype() == BasicType::FLOAT_BTYPE, nullptr, dimension, formal_name));
        }
    }
    for (auto &param_name : param_names) {
        printf("param name : %s in func %s\n", param_name.c_str(), function_name.c_str());
    }

    // 创建这个function(Value)
    if (ret_type == BasicType::INT_BTYPE) {
        function_value = IrFactory::createIntFunction(function_name);
    } else if (ret_type == BasicType::FLOAT_BTYPE) {
        function_value = IrFactory::createFloatFunction(function_name);
    } else {
        function_value = IrFactory::createVoidFunction(function_name);
    }
    // 将其中的argument加入到function之中
    auto function = dynamic_cast<Function *>(function_value);
    context_->curr_function_ = function;
    printf("the argument size is %lu before add\n", function->getArgumentSize());
    // 进入新的一层符号表,并将参数加入到符号表
    var_symbol_table_.enterScope();
    for (const auto argument: arguments) {
        auto to_arg = dynamic_cast<Argument *>(argument);
        function->addArgument(std::unique_ptr<Argument>(to_arg));
        IrSymbolEntry new_entry(false, to_arg->isFloat() ?
                    BasicType::FLOAT_BTYPE : BasicType::INT_BTYPE,to_arg);
        var_symbol_table_.addIdent(new_entry);
        printf("add %s to function %s, after that the size is %lu\n", argument->getName().c_str(), function->getName().c_str(), function->getArgumentSize());
    }
    // 在一个函数开始的地方,应该有新的basic block
    module_->addFunction(std::unique_ptr<Function>(function));
    context_->ResetBlockNo();
    auto new_bb_value = IrFactory::createBasicBlock(function_name);       // 暂时lebal作为函数名
    auto new_bb = dynamic_cast<BasicBlock *>(new_bb_value);
    setCurrBasicBlock(new_bb);
    function->addBasicBlock(new_bb);
    // 还需要给参数中对应的变量分配空间????
    for (int i = 0; i < formal_size; ++i) {
        auto dimension_size = param_dimensions[i].size();
        Value *alloca_ptr = nullptr;
        Value *store_inst_value = nullptr;
        if (dimension_size == 0) {      // 普通变量
            alloca_ptr = param_types[i] == BasicType::INT_BTYPE ?
                    IrFactory::createIAllocaInstruction() : IrFactory::createFAllocaInstruction();
            store_inst_value = param_types[i] == BasicType::INT_BTYPE ?
                    IrFactory::createIStoreInstruction(function->getArgument(i), alloca_ptr)
                    : IrFactory::createFStoreInstruction(function->getArgument(i), alloca_ptr);
        } else if (dimension_size == 1) {   // 一维数组

        } else {    // 多维数组

        }
        new_bb->addInstruction(alloca_ptr);
        new_bb->addInstruction(store_inst_value);
    }
    // 处理函数体
    visit(def->getBlock());
    // 退出
    var_symbol_table_.exitScope();
}

void IrBuilder::visit(const std::shared_ptr<Statement> &stmt) {
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
        case EVAL_STMTTYPE:
            visit(std::dynamic_pointer_cast<EvalStatement>(stmt)->getExpr());
        default:
            return;
    }
}

void IrBuilder::visit(const std::shared_ptr<UnaryExpr> &expr) {
    Value *value = nullptr;
    GlobalVariable *to_global = nullptr;
    ConstantVar *to_const = nullptr;
    BasicType basic_type = expr->expr_type_;
    visit(expr->getExpr());
    value = curr_value_;
    if (value->isPtr()) {
        to_global = dynamic_cast<GlobalVariable *>(value);
        if (to_global && to_global->isConst()) {
            to_const = dynamic_cast<ConstantVar *>(value);
            value = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createIConstantVar(to_const->getIValue()) : IrFactory::createFConstantVar(to_const->getFValue());
        } else {
            value = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createILoadInstruction(value) : IrFactory::createFLoadInstruction(value);
            addInstruction(value);
        }
    }
    auto optype = expr->getOpType();
    Value *unary_inst = nullptr;
    switch (optype) {
        case UnaryOpType::NEGATIVE_OPTYPE:
            unary_inst = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createINegInstruction(value) : IrFactory::createFNegInstruction(value);
            break;
        case UnaryOpType::NOTOP_OPTYPE:
            unary_inst = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createINotInstruction(value) : IrFactory::createFNotInstruction(value);
            break;
        case UnaryOpType::POSITIVE_OPTYPE:
            break;
    }
    if (unary_inst) {
        addInstruction(unary_inst);
        setCurrValue(unary_inst);
    } else {
        setCurrValue(value);
    }
}

void IrBuilder::visit(const std::shared_ptr<BinaryExpr> &expr) {
    Value *left = nullptr, *right = nullptr;
    BasicType left_type, right_type, this_type;
    GlobalVariable *left_to_global = nullptr, *right_to_global = nullptr;
    ConstantVar *left_to_const = nullptr, *right_to_const = nullptr;
    auto op_type = expr->getOpType();
    if (op_type != BinaryOpType::AND_OPTYPE && op_type != BinaryOpType::OR_OPTYPE) {
        visit(expr->getLeftExpr());     // 处理左边
        left = curr_value_;
        left_type = expr->getLeftExpr()->expr_type_;
        if (curr_value_->isPtr()) {
            left_to_global = dynamic_cast<GlobalVariable *>(curr_value_);
            if (left_to_global && left_to_global->isConst()) {
                left_to_const = dynamic_cast<ConstantVar *>(left_to_global->getConstInit());
                if (left_type == BasicType::INT_BTYPE) {
                    left = IrFactory::createIConstantVar(left_to_const->getIValue());
                } else {
                    left = IrFactory::createFConstantVar(left_to_const->getFValue());
                }
            } else {
                left = left_type == BasicType::INT_BTYPE ? IrFactory::createILoadInstruction(curr_value_) :
                        IrFactory::createFLoadInstruction(curr_value_);
                addInstruction(left);
            }
        }
        visit(expr->getRightExpr());        // 处理右边
        right = curr_value_;
        right_type = expr->getRightExpr()->expr_type_;
        if (curr_value_->isPtr()) {
            right_to_global = dynamic_cast<GlobalVariable *>(curr_value_);
            if (right_to_global && right_to_global->isConst()) {
                right_to_const = dynamic_cast<ConstantVar *>(right_to_global->getConstInit());
                if (right_type == BasicType::INT_BTYPE) {
                    right = IrFactory::createIConstantVar(right_to_const->getIValue());
                } else {
                    right = IrFactory::createFConstantVar(right_to_const->getFValue());
                }
            } else {
                right = right_type == BasicType::INT_BTYPE ? IrFactory::createILoadInstruction(curr_value_) :
                       IrFactory::createFLoadInstruction(curr_value_);
                addInstruction(right);
            }
        }
        // 类型转换
        if (left_type != right_type) {
            if (left_type == BasicType::INT_BTYPE) {
                left = IrFactory::createI2FCastInstruction(left);
                addInstruction(left);
            } else {
                right = IrFactory::createI2FCastInstruction(right);
                addInstruction(right);
            }
            this_type = BasicType::FLOAT_BTYPE;
        } else {
            this_type = left_type;
        }
    }

    Value *binaryop_inst = nullptr;
    switch (op_type) {
        case BinaryOpType::ADD_OPTYPE:
            binaryop_inst = IrFactory::createAddInstruction(left, right, this_type);
            break;
        case BinaryOpType::SUB_OPTYPE:
            binaryop_inst = IrFactory::createSubInstruction(left, right, this_type);
            break;
        case BinaryOpType::DIV_OPTYPE:
            binaryop_inst = IrFactory::createDivInstruction(left, right, this_type);
            break;
        case BinaryOpType::MUL_OPTYPE:
            binaryop_inst = IrFactory::createMulInstruction(left, right, this_type);
            break;
        case BinaryOpType::MOD_OPTYPE:
            binaryop_inst = IrFactory::createModInstruction(left, right, this_type);
            break;
        case BinaryOpType::EQ_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createEqICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createEqFCmpInstruction(left, right);
            }
            break;
        case BinaryOpType::NEQ_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createNeICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createNeFCmpInstruction(left, right);
            }
            break;
        case BinaryOpType::GTE_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createGeICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createGeFCmpInstruction(left, right);
            }
            break;
        case BinaryOpType::GT_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createGtICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createGtFCmpInstruction(left, right);
            }
            break;
        case BinaryOpType::LTE_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createLeICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createLeFCmpInstruction(left, right);
            }
            break;
        case BinaryOpType::LT_OPTYPE:
            if (this_type == BasicType::INT_BTYPE) {
                binaryop_inst = IrFactory::createLtICmpInstruction(left, right);
            } else {
                binaryop_inst = IrFactory::createLtFCmpInstruction(left, right);
            }
            break;

        case BinaryOpType::AND_OPTYPE: {
            visit(expr->getLeftExpr());
            visit(expr->getRightExpr());
            auto right_block = true_jump_map_[expr->getRightExpr()].front()->getParent();
            auto &left_jump_insts = true_jump_map_[expr->getLeftExpr()];
            for (auto jump: left_jump_insts) {
                printf("set the jump from %s to true lebal: %s\n", jump->getParent()->getName().c_str(), right_block->getName().c_str());
                auto br_inst = dynamic_cast<BranchInstruction *>(jump);
                assert(br_inst);
                br_inst->setTrueLabel(right_block);
                BasicBlock::bindBasicBlock(br_inst->getParent(), right_block);
            }
            auto false_jumps = false_jump_map_[expr->getLeftExpr()];
            false_jumps.insert(false_jumps.end(), false_jump_map_[expr->getRightExpr()].begin(),
                               false_jump_map_[expr->getRightExpr()].end());
            true_jump_map_[expr] = true_jump_map_[expr->getRightExpr()];
            false_jump_map_[expr] = false_jumps;
            break;
        }
        case BinaryOpType::OR_OPTYPE: {
            visit(expr->getLeftExpr());
            visit(expr->getRightExpr());
            auto right_block = false_jump_map_[expr->getRightExpr()].front()->getParent();
            auto &left_jump_insts = false_jump_map_[expr->getLeftExpr()];

            for (auto jump: left_jump_insts) {
                printf("set the jump from %s to false lebal: %s\n", jump->getParent()->getName().c_str(), right_block->getName().c_str());
                auto br_inst = dynamic_cast<BranchInstruction *>(jump);
                assert(br_inst);
                br_inst->setFalseLabel(right_block);
                BasicBlock::bindBasicBlock(br_inst->getParent(), right_block);
            }

            auto true_jumps = true_jump_map_[expr->getLeftExpr()];
            true_jumps.insert(true_jumps.end(), true_jump_map_[expr->getRightExpr()].begin(),
                              true_jump_map_[expr->getRightExpr()].end());
            true_jump_map_[expr] = true_jumps;
            false_jump_map_[expr] = false_jump_map_[expr->getRightExpr()];
            break;
        }
    }
    bool is_logic_op = (op_type == EQ_OPTYPE || op_type == NEQ_OPTYPE ||
            op_type == GTE_OPTYPE || op_type == GT_OPTYPE ||
            op_type == LTE_OPTYPE || op_type == LT_OPTYPE);
    // TODO还有and和or,这是比较复杂的
    if (op_type != BinaryOpType::AND_OPTYPE && op_type != BinaryOpType::OR_OPTYPE) {
        addInstruction(binaryop_inst);
        setCurrValue(binaryop_inst);
        if (is_deal_cond_) {
            auto new_br_inst = IrFactory::createCondBrInstruction(curr_value_, nullptr, nullptr);
            printf("create new cond branch %s\n", new_br_inst->getName().c_str());
            addInstruction(new_br_inst);
            auto new_block = IrFactory::createBasicBlock("lebal");
            BasicBlock::bindBasicBlock(context_->curr_bb_, dynamic_cast<BasicBlock *>(new_block));
            setCurrBasicBlock(new_block);
            addBasicBlock(new_block);
            // 设置回填用的map
            true_jump_map_[expr].push_back(dynamic_cast<Instruction *>(new_br_inst));
            false_jump_map_[expr].push_back(dynamic_cast<Instruction *>(new_br_inst));
        }
    }
}

void IrBuilder::visit(const std::shared_ptr<BlockItems> &stmt) {
    auto blocks_size = stmt->getItemSize();
    for (int i = 0; i < blocks_size; ++i) {
        auto block = stmt->getBlockItem(i);
        visit(block->getStmt());
    }
}

void IrBuilder::visit(const std::shared_ptr<Expression> &expr) {
    auto expr_type = expr->getExprNodeType();
    switch (expr_type) {
        case ExprType::BINARY_TYPE:
            visit(std::dynamic_pointer_cast<BinaryExpr>(expr));
            break;
        case ExprType::UNARY_TYPE:
            visit(std::dynamic_pointer_cast<UnaryExpr>(expr));
            break;
        case ExprType::LVAL_TYPE:
            visit(std::dynamic_pointer_cast<LvalExpr>(expr));
            break;
        case ExprType::NUMBER_TYPE:
            visit(std::dynamic_pointer_cast<Number>(expr));
            break;
        case ExprType::ARRAYVALUE_TYPE:
            visit(std::dynamic_pointer_cast<ArrayValue>(expr));
            break;
        case ExprType::CALLFUNC_TYPE:
            visit(std::dynamic_pointer_cast<CallFuncExpr>(expr));
            break;
    }
}

void IrBuilder::visit(const std::shared_ptr<VarDeclare> &decl) {

}

void IrBuilder::visit(const std::shared_ptr<CallFuncExpr> &expr) {

}

void IrBuilder::visit(const std::shared_ptr<CompUnit> &compunit) {
    var_symbol_table_.enterScope();

    size_t decl_size = compunit->getDeclNumber();
    for (int i = 0; i < decl_size; ++i) {       // 其实内部有多个define,
        // 具体应该等到define在做分析
        auto decl = compunit->getDecl(i);
        visit(std::dynamic_pointer_cast<Declare>(decl));
    }

    size_t func_size = compunit->getFuncDefNumber();
    for (int i = 0; i < func_size; ++i) {
        auto func_def = compunit->getFuncDef(i);
        visit(std::dynamic_pointer_cast<FuncDefine>(func_def));
    }

    var_symbol_table_.exitScope();
}

void IrBuilder::visit(const std::shared_ptr<ConstDeclare> &decl) {}

void IrBuilder::visit(const std::shared_ptr<ArrayValue> &arrayval) {
    if (arrayval->is_number_) {     // 如果是Number类型就对其进行visit并且将改value用store拷贝到指定的指针上
        // auto gpe_inst_value
        BasicType basic_type = curr_decl_->type_;
        auto const_offset = IrFactory::createIConstantVar(arrayval->array_idx_);
        auto gep_inst_value = basic_type == BasicType::INT_BTYPE ?
                IrFactory::createIGEPInstruction(curr_local_array_, const_offset)
                :IrFactory::createFGEPInstruction(curr_local_array_, const_offset);
        addInstruction(gep_inst_value);
        visit(arrayval->value_);
        Value *init_value = curr_value_;
        if (init_value->isPtr()) {
            auto load_inst_value = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createILoadInstruction(init_value) : IrFactory::createFLoadInstruction(init_value);
            addInstruction(load_inst_value);
            init_value = load_inst_value;
        }

        if (arrayval->value_->expr_type_ != basic_type) {       // 需要进行类型的转换
            auto cast_inst_value = basic_type == BasicType::INT_BTYPE ?
                    IrFactory::createF2ICastInstruction(init_value) : IrFactory::createI2FCastInstruction(init_value);
            addInstruction(cast_inst_value);
            init_value = cast_inst_value;
        }
        auto store_inst_value = basic_type == BasicType::INT_BTYPE ?
                IrFactory::createIStoreInstruction(init_value, gep_inst_value)
                :IrFactory::createFStoreInstruction(init_value, gep_inst_value);
        addInstruction(store_inst_value);
    } else {
        for (auto &array_value : arrayval->valueList_) {
            visit(array_value);
        }
        // 对于初始化为0的空间部分
        if (!arrayval->zero_init_intervals_.empty()) {
            auto zero_const_value = curr_decl_->type_ == BasicType::INT_BTYPE ?
                                    IrFactory::createIConstantVar(0) : IrFactory::createFConstantVar(0.0);
            for (auto &[start, end]: arrayval->zero_init_intervals_) {
                int32_t len = end - start;
                assert(len >= 0);
                auto offset_const_value = IrFactory::createIConstantVar(start);
                auto size_const_value = IrFactory::createIConstantVar(len);
                auto gep_start_inst_value = curr_decl_->type_ == BasicType::INT_BTYPE ?
                                            IrFactory::createIGEPInstruction(curr_local_array_, offset_const_value) :
                                            IrFactory::createFGEPInstruction(curr_local_array_,
                                                                             offset_const_value);        // 获取start指针
                addInstruction(gep_start_inst_value);
                auto memset_inst_value = curr_decl_->type_ == BasicType::INT_BTYPE ?
                                         IrFactory::createIMemSetInstruction(gep_start_inst_value, size_const_value,
                                                                             zero_const_value) :
                                         IrFactory::createFMemSetInstruction(gep_start_inst_value, size_const_value,
                                                                             zero_const_value);
                addInstruction(memset_inst_value);
            }
        }
    }
}

void IrBuilder::visit(const std::shared_ptr<BreakStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<WhileStatement> &stmt) {
    auto while_start_bb_value = IrFactory::createBasicBlock("while");
    BasicBlock *while_start_bb = dynamic_cast<BasicBlock *>(while_start_bb_value);
    auto while_then_bb_value = IrFactory::createBasicBlock("then");
    BasicBlock *while_then_bb = dynamic_cast<BasicBlock *>(while_then_bb_value);
    auto while_next_bb_value = IrFactory::createBasicBlock("next");
    BasicBlock *while_next_bb = dynamic_cast<BasicBlock *>(while_next_bb_value);

    BasicBlock::bindBasicBlock(context_->curr_bb_, while_start_bb);
    BasicBlock::bindBasicBlock(while_start_bb, while_then_bb);
    BasicBlock::bindBasicBlock(while_start_bb, while_next_bb);

    addInstruction(IrFactory::createBrInstruction(while_start_bb_value));
    addBasicBlock(while_start_bb_value);
    setCurrBasicBlock(while_start_bb_value);

    enableDealCond();
    visit(stmt->getCond());
    disableDealCond();

    if (stmt->getStatement()) {
        for (auto true_jump: true_jump_map_[stmt->getCond()]) {
            BranchInstruction *branch_inst = dynamic_cast<BranchInstruction *>(true_jump);
            branch_inst->setTrueLabel(while_then_bb_value);
            BasicBlock::bindBasicBlock(branch_inst->getParent(), while_then_bb);
        }

        for (auto false_jump: false_jump_map_[stmt->getCond()]) {
            BranchInstruction *branch_inst = dynamic_cast<BranchInstruction *>(false_jump);
            branch_inst->setFalseLabel(while_next_bb_value);
            BasicBlock::bindBasicBlock(branch_inst->getParent(), while_next_bb);
        }

        initJumpMap();

        addBasicBlock(while_then_bb);
        setCurrBasicBlock(while_then_bb);
        visit(stmt->getStatement());

        addInstruction(IrFactory::createBrInstruction(while_start_bb_value));
        BasicBlock::bindBasicBlock(context_->curr_bb_, while_start_bb);
    }

    addBasicBlock(while_next_bb_value);
    setCurrBasicBlock(while_next_bb_value);

}

void IrBuilder::visit(const std::shared_ptr<AssignStatement> &stmt) {
    // 首先得出来左边的,左边的值应该是指针的形式
    BasicType left_type, right_type;
    visit(stmt->getLeftExpr());
    left_type = stmt->getLeftExpr()->expr_type_;
    Value *ptr = curr_value_;
    // assert(ptr && ptr->isPtr());
    visit(stmt->getRightExpr());
    right_type = stmt->getRightExpr()->expr_type_;
    Value *right_value = curr_value_;
    if (right_value->isPtr()) {
        auto load_inst = right_type == BasicType::INT_BTYPE ?
                IrFactory::createILoadInstruction(right_value) : IrFactory::createFLoadInstruction(right_value);
        addInstruction(load_inst);
        right_value = load_inst;
    }
    if (left_type != right_type) {
        right_value = left_type == BasicType::INT_BTYPE ?
                IrFactory::createF2ICastInstruction(right_value) : IrFactory::createI2FCastInstruction(right_value);
        addInstruction(right_value);
    }
    setCurrValue(ptr);
    Value *store_inst = left_type == BasicType::INT_BTYPE ?
            IrFactory::createIStoreInstruction(right_value, ptr) : IrFactory::createFStoreInstruction(right_value, ptr);
    addInstruction(store_inst);
    // 然后得出来右边的
}

void IrBuilder::visit(const std::shared_ptr<IfElseStatement> &stmt) {
    enableDealCond();
    visit(stmt->getCond());
    disableDealCond();

    auto then_value = IrFactory::createBasicBlock("then");
    BasicBlock *then_bb = dynamic_cast<BasicBlock *>(then_value);
    auto next_value = IrFactory::createBasicBlock("next");
    BasicBlock *next_bb = dynamic_cast<BasicBlock *>(next_value);
    BasicBlock::bindBasicBlock(then_bb, next_bb);

    if (stmt->hasElse()) {
        auto else_value = IrFactory::createBasicBlock("else");
        BasicBlock *else_bb = dynamic_cast<BasicBlock *>(else_value);
        BasicBlock::bindBasicBlock(else_bb, next_bb);

        for (auto true_jump: true_jump_map_[stmt->getCond()]) {
            printf("set the jump from %s to true lebal: %s\n", true_jump->getName().c_str(), then_value->getName().c_str());
            auto branch_inst = dynamic_cast<BranchInstruction *>(true_jump);
            branch_inst->setTrueLabel(then_value);
            BasicBlock::bindBasicBlock(true_jump->getParent(), then_bb);
        }

        for (auto false_jump: false_jump_map_[stmt->getCond()]) {
            printf("set the jump from %s to false lebal: %s\n", false_jump->getName().c_str(), else_value->getName().c_str());
            auto branch_inst = dynamic_cast<BranchInstruction *>(false_jump);
            branch_inst->setFalseLabel(else_value);
            BasicBlock::bindBasicBlock(false_jump->getParent(), else_bb);
        }

        initJumpMap();

        addBasicBlock(then_value);
        setCurrBasicBlock(then_value);
        visit(stmt->getIfStmt());

        addInstruction(IrFactory::createBrInstruction(next_value));

        addBasicBlock(else_value);
        setCurrBasicBlock(else_value);
        visit(stmt->getElseStmt());

        addInstruction(IrFactory::createBrInstruction(next_value));

    } else {
        for (auto true_jump: true_jump_map_[stmt->getCond()]) {
            auto branch_inst = dynamic_cast<BranchInstruction *>(true_jump);
            branch_inst->setTrueLabel(then_value);
            BasicBlock::bindBasicBlock(true_jump->getParent(), then_bb);
        }

        for (auto false_jump: false_jump_map_[stmt->getCond()]) {
            auto branch_inst = dynamic_cast<BranchInstruction *>(false_jump);
            branch_inst->setFalseLabel(next_value);
            BasicBlock::bindBasicBlock(false_jump->getParent(), next_bb);
        }

        initJumpMap();

        addBasicBlock(then_value);
        setCurrBasicBlock(then_value);
        visit(stmt->getIfStmt());

        addInstruction(IrFactory::createBrInstruction(next_value));

    }
    addBasicBlock(next_value);
    setCurrBasicBlock(next_value);
}

void IrBuilder::visit(const std::shared_ptr<ReturnStatement> &stmt) {
    auto return_value = stmt->getExpr();
    if (return_value) {
        visit(return_value);
        Value *ret_value = curr_value_;
        if (ret_value->isPtr()) {       // 需要多一个load语句
            auto load_inst_value = return_value->expr_type_ == BasicType::INT_BTYPE ?
                    IrFactory::createILoadInstruction(ret_value) : IrFactory::createFLoadInstruction(ret_value);
            addInstruction(load_inst_value);
            ret_value = load_inst_value;
        }
        // 可能因为类型不一致而需要类型转换
        BasicType curr_func_ret_type = context_->curr_function_->getRetType();
        if (curr_func_ret_type != return_value->expr_type_) {
            auto cast_inst = curr_func_ret_type == BasicType::INT_BTYPE ?
                    IrFactory::createF2ICastInstruction(ret_value) : IrFactory::createI2FCastInstruction(ret_value);
            addInstruction(cast_inst);
            ret_value = cast_inst;
        }
        auto ret_inst = curr_func_ret_type == BasicType::INT_BTYPE ?
                IrFactory::createIRetInstruction(ret_value) : IrFactory::createFRetInstruction(ret_value);
        setCurrValue(ret_value);
        addInstruction(ret_inst);
    } else {
        addInstruction(IrFactory::createVoidRetInstruction());
    }
}

void IrBuilder::visit(const std::shared_ptr<ContinueStatement> &stmt) {

}

void IrBuilder::dump() const {
    dumper_->dump(module_.get());
}

void IrBuilder::setCurrValue(Value *value) {
    curr_value_ = value;
}

void IrBuilder::setCurrBasicBlock(Value *bb) {
    context_->curr_bb_ = dynamic_cast<BasicBlock *>(bb);
}