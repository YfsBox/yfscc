//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_MODULE_H
#define YFSCC_MODULE_H

#include <vector>
#include <memory>

class GlobalVariable;
class Function;

class Module {
public:
    friend class IrDumper;

    using GlobalVariablePtr = std::unique_ptr<GlobalVariable>;

    using FunctionPtr = std::unique_ptr<Function>;

    explicit Module() = default;

    ~Module() = default;

    void addGlobalVariable(GlobalVariablePtr var) {
        global_variables_.emplace_back(std::move(var));
    }

    void addFunction(FunctionPtr func) {
        functions_.emplace_back(std::move(func));
    }

    size_t getFuncSize() const {
        return functions_.size();
    }

    Function *getFunction(int idx) {
        return functions_[idx].get();
    }

private:
    std::vector<GlobalVariablePtr> global_variables_;

    std::vector<FunctionPtr> functions_;
};


#endif //YFSCC_MODULE_H
