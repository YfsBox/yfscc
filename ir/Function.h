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
#include "Argument.h"

class Module;
class BasicBlock;

class Function: public Value {
public:
    using ArgumentPtr = std::unique_ptr<Argument>;

    using BasicBlockPtr = std::unique_ptr<BasicBlock>;

    Function(Module *module, const std::string &name);

    ~Function();

    void addArgument(bool is_float, const std::string &argument_name);

    void addBasicBlock(BasicBlockPtr block);

    Argument *getArgument(int idx) const {
        return arguments_[idx].get();
    }

    Module *getParent() const {
        return parent_;
    }

private:
    Module *parent_;
    std::vector<ArgumentPtr> arguments_;
    std::list<BasicBlockPtr> blocks_;
};

#endif //YFSCC_FUNCTION_H
