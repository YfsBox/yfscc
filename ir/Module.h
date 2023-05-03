//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_MODULE_H
#define YFSCC_MODULE_H

#include <vector>
#include <memory>
#include "Function.h"

class GlobalVariable;
class Function;

class Module {
public:
    friend class IrDumper;

    using GlobalVariablePtr = std::unique_ptr<GlobalVariable>;

    using FunctionPtr = std::unique_ptr<Function>;

    explicit Module() = default;

    ~Module() = default;

    void addGlobalVariable(GlobalVariablePtr var);

    void addFunction(FunctionPtr func) {
        functions_.emplace_back(std::move(func));
    }

    void addFunctionDecl(FunctionPtr func) {
        lib_func_decl_.emplace_back(std::move(func));
    }

    size_t getGlobalSize() const {
        return global_variables_.size();
    }

    size_t getFuncSize() const {
        return functions_.size();
    }

    size_t getLibFuncSize() const {
        return lib_func_decl_.size();
    }

    Function *getFunction(int idx) const {
        return functions_[idx].get();
    }

    Function *getLibFunction(int idx) const {
        return lib_func_decl_[idx].get();
    }

    GlobalVariable *getGlobalVariable(int idx) const {
        return global_variables_[idx].get();
    }

    std::vector<const GlobalVariable *> getGlobalSet() const {
        std::vector<const GlobalVariable *> globals;
        globals.reserve(global_variables_.size());
        for (auto &global: global_variables_) {
            globals.push_back(global.get());
        }
        return globals;
    }

private:
    std::vector<GlobalVariablePtr> global_variables_;

    std::vector<FunctionPtr> functions_;

    std::vector<FunctionPtr> lib_func_decl_;
};


#endif //YFSCC_MODULE_H
