//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_FUNCTION_H
#define YFSCC_FUNCTION_H

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include "Value.h"

class Argument;
class Module;
class BasicBlock;

class Function: public Value {
public:
    using ArgumentPtr = std::unique_ptr<Argument>;

    using BasicBlockPtr = std::unique_ptr<BasicBlock>;

    Function(Module *module, const std::string &name);

    ~Function() = default;

    Function(const Function &function) = delete;

    Function& operator=(const Function &function) = delete;


private:
    std::string func_name_;
    Module *parent_;
    std::vector<ArgumentPtr> arguments_;
    std::list<BasicBlockPtr> blocks_;
};

#endif //YFSCC_FUNCTION_H
