//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Function.h"
#include "BasicBlock.h"

Function::Function(BasicType ret_type, Module *module, const std::string &name): Value(FunctionValue, name), ret_type_(ret_type), parent_(module) {}

Function::~Function() = default;

void Function::addArgument(bool is_float, const std::string &argument_name) {
    int curr_idx = arguments_.size();
    arguments_.push_back(std::make_unique<Argument>(curr_idx, is_float, this, argument_name));
}

void Function::addBasicBlock(BasicBlockPtr block) {
    blocks_.push_front(std::move(block));
}

void Function::dump() const {

}