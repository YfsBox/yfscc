//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include <memory>
#include "../common/Ast.h"
#include "BasicBlock.h"
#include "Module.h"
#include "GlobalVariable.h"
#include "Instruction.h"
#include "IrBuilder.h"
#include "IrFactory.h"

IrBuilder::IrBuilder():
    module_(std::make_unique<Module>()),
    context_(std::make_unique<IrContext>(module_.get())),
    curr_decl_(nullptr),
    curr_value_(nullptr){

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
            Value *init_value = curr_value_;
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
    if (def->getId()->getDimensionSize()) {

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

    int decl_size = compunit->getDeclNumber();
    for (int i = 0; i < decl_size; ++i) {       // 其实内部有多个define,
        // 具体应该等到define在做分析
        auto decl = compunit->getDecl(i);
        visit(std::dynamic_pointer_cast<Declare>(decl));
    }

    int func_size = compunit->getFuncDefNumber();
    for (int i = 0; i < func_size; ++i) {
        auto func_def = compunit->getFuncDef(i);
        visit(std::dynamic_pointer_cast<FuncDefine>(func_def));
    }

    var_symbol_table_.exitScope();
}

void IrBuilder::visit(const std::shared_ptr<ConstDeclare> &decl) {
    return;
}

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

