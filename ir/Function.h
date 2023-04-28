//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_FUNCTION_H
#define YFSCC_FUNCTION_H

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <list>
#include "Value.h"
#include "Argument.h"
#include "../common/Types.h"

class Module;
class BasicBlock;

class Function: public Value {
public:
    using ArgumentPtr = std::unique_ptr<Argument>;

    using BasicBlockPtr = std::unique_ptr<BasicBlock>;

    Function(BasicType ret_type, Module *module, const std::string &name);

    ~Function();

    void addArgument(ArgumentPtr argument);

    void addArgument(BasicType basic_type, const std::string &argument_name);

    void addBasicBlock(Value *block);

    void removeEmptyBasicBlock();

    Argument *getArgument(int idx) const {
        assert(idx < arguments_.size());
        return arguments_[idx].get();
    }

    size_t getArgumentSize() const {
        return arguments_.size();
    }

    Module *getParent() const {
        return parent_;
    }

    BasicType getRetType() const {
        return ret_type_;
    }

    void dump() const;

    std::list<BasicBlockPtr> &getBlocks() {
        return blocks_;
    }

private:
    Module *parent_;
    BasicType ret_type_;
    std::vector<ArgumentPtr> arguments_;
    std::list<BasicBlockPtr> blocks_;
};

#endif //YFSCC_FUNCTION_H
