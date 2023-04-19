//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include <memory>
#include "../common/Ast.h"
#include "Module.h"
#include "GlobalVariable.h"
#include "IrBuilder.h"
#include "IrFactory.h"

IrBuilder::IrBuilder():
    module_(std::make_unique<Module>()),
    context_(std::make_unique<IrContext>(module_.get())),
    curr_decl_(nullptr){

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
            std::unique_ptr<Value> new_value;
            assert(init_value);

            if (def_basic_type == BasicType::INT_BTYPE) {
                new_value = IrFactory::createConstIGlobalVar(init_value->getIntValue(), var_name);
            } else if (def_basic_type == BasicType::FLOAT_BTYPE) {
                new_value = IrFactory::createConstFGlobalVar(init_value->getFloatValue(), var_name);
            }
            new_global = std::unique_ptr<GlobalVariable> (dynamic_cast<GlobalVariable *>(new_value.release()));
            module_->addGlobalVariable(std::move(new_global));          // 加入到module的集合中
        } else {


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
            std::unique_ptr<Value> new_value;
            assert(init_value);

            if (def_basic_type == BasicType::INT_BTYPE) {
                new_value = IrFactory::createIGlobalVar(var_name, init_value->getIntValue());
            } else if (def_basic_type == BasicType::FLOAT_BTYPE) {
                new_value = IrFactory::createFGlobalVar(var_name, init_value->getFloatValue());
            }
            new_global = std::unique_ptr<GlobalVariable> (dynamic_cast<GlobalVariable *>(new_value.release()));
            module_->addGlobalVariable(std::move(new_global));          // 加入到module的集合中

        } else {

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

