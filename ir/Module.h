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
    using GlobalVariablePtr = std::unique_ptr<GlobalVariable>;

    using FunctionPtr = std::unique_ptr<Function>;

    explicit Module() = default;

    ~Module() = default;
private:
    std::vector<GlobalVariablePtr> global_variables_;
    std::vector<FunctionPtr> functions_;
};


#endif //YFSCC_MODULE_H
