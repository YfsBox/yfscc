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
    module_(std::make_unique<Module>()),
    context_(std::make_unique<IrContext>(module_.get())),
    dumper_(std::make_unique<IrDumper>(out)),
    curr_decl_(nullptr),
    curr_value_(nullptr){
    IrFactory::InitContext(context_.get());
}

IrBuilder::~IrBuilder() = default;

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

void IrBuilder::visit(const std::shared_ptr<LvalExpr> &expr) {

}

void IrBuilder::visit(const std::shared_ptr<Number> &number) {

}

void IrBuilder::visit(const std::shared_ptr<ConstDefine> &def) {
    BasicType def_basic_type = curr_decl_->type_;
    std::string var_name = def->getId()->getId();
    if (def->getId()->getDimensionSize()) {

    } else {
        if (var_symbol_table_.isInGlobalScope()) {

            IrSymbolEntry new_entry(true, def_basic_type, var_name);
            var_symbol_table_.addIdent(new_entry);

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
            module_->addGlobalVariable(std::move(new_global));          // 加入到module的集合中
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
                cast_inst_value = IrFactory::createI2FCastInstruction(curr_value_);
            } else if (init_expr_type == BasicType::FLOAT_BTYPE && def_basic_type == BasicType::INT_BTYPE) {
                cast_inst_value = IrFactory::createF2ICastInstruction(curr_value_);
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
            context_->curr_bb_->addInstruction(alloca_inst_value);
            if (cast_inst_value) {
                context_->curr_bb_->addInstruction(cast_inst_value);
            }
            context_->curr_bb_->addInstruction(store_inst_value);
            // 不要忘了将变量加入符号表
            IrSymbolEntry new_entry(true, def_basic_type, var_name);
            var_symbol_table_.addIdent(new_entry);
        }

    }
}

void IrBuilder::visit(const std::shared_ptr<VarDefine> &def) {
    // 首先考虑是否是数组的情况
    BasicType def_basic_type = curr_decl_->type_;
    std::string var_name = def->getId()->getId();
    if (def->getId()->getDimensionSize()) {     // 数组类型的

    } else {        // 不是数组

        if (var_symbol_table_.isInGlobalScope()) {      // 是否处于全局作用域
            IrSymbolEntry new_entry(false, def_basic_type, var_name);
            var_symbol_table_.addIdent(new_entry);

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
                context_->curr_bb_->addInstruction(alloca_inst_value);
                if (cast_inst_value) {
                    context_->curr_bb_->addInstruction(cast_inst_value);
                }
                context_->curr_bb_->addInstruction(store_inst_value);
                // 不要忘了将变量加入符号表
            } else {
                context_->curr_bb_->addInstruction(alloca_inst_value);
            }
            IrSymbolEntry new_entry(true, def_basic_type, var_name);
            var_symbol_table_.addIdent(new_entry);
        }

    }

}

void IrBuilder::visit(const std::shared_ptr<BlockItem> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<FuncDefine> &def) {
    Value *function_value = nullptr;
    BasicType ret_type = def->getReturnType();
    std::string function_name = def->getId()->getId();

    std::vector<std::string> param_names;
    std::vector<BasicType> param_types;
    std::vector<std::vector<int32_t>> param_dimensions;
    std::vector<Value *> arguments;

    auto formals = def->getFormals();
    size_t formal_size = formals->getFormalSize();
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
    // 创建这个function(Value)
    if (ret_type == BasicType::INT_BTYPE) {
        function_value = IrFactory::createIntFunction(param_names, function_name);
    } else if (ret_type == BasicType::FLOAT_BTYPE) {
        function_value = IrFactory::createFloatFunction(param_names, function_name);
    } else {
        function_value = IrFactory::createVoidFunction(param_names, function_name);
    }
    // 将其中的argument加入到function之中
    auto function = dynamic_cast<Function *>(function_value);
    context_->curr_function_ = function;
    for (const auto argument: arguments) {
        function->addArgument(std::unique_ptr<Argument>(dynamic_cast<Argument *>(argument)));
    }
    // 进入新的一层符号表,并将参数加入到符号表
    var_symbol_table_.enterScope();
    for (int i = 0; i < formal_size; ++i) {
        if (param_dimensions.empty()) {
            IrSymbolEntry new_entry(false, param_types[i], param_names[i]);
            var_symbol_table_.addIdent(new_entry);
        } else {
            IrSymbolEntry new_entry(false, param_types[i], param_dimensions[i],param_names[i]);
            var_symbol_table_.addIdent(new_entry);
        }
    }
    // 在一个函数开始的地方,应该有新的basic block
    module_->addFunction(std::unique_ptr<Function>(function));
    auto new_bb_value = IrFactory::createBasicBlock(function_name);       // 暂时lebal作为函数名
    auto new_bb = dynamic_cast<BasicBlock *>(new_bb_value);
    context_->curr_bb_ = new_bb;        // 设置为当前的新的basic block
    function->addBasicBlock(std::unique_ptr<BasicBlock>(new_bb));
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

}

void IrBuilder::visit(const std::shared_ptr<UnaryExpr> &expr) {

}

void IrBuilder::visit(const std::shared_ptr<BinaryExpr> &expr) {

}

void IrBuilder::visit(const std::shared_ptr<BlockItems> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<Expression> &expr) {

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

}

void IrBuilder::visit(const std::shared_ptr<BreakStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<WhileStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<AssignStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<IfElseStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<ReturnStatement> &stmt) {

}

void IrBuilder::visit(const std::shared_ptr<ContinueStatement> &stmt) {

}

void IrBuilder::dump() const {
    dumper_->dump(module_.get());
}

void IrBuilder::setCurrValue(Value *value) {
    curr_value_ = value;
}
