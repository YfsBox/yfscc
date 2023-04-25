//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Function.h"
#include "BasicBlock.h"

Function::Function(BasicType ret_type, Module *module, const std::string &name): Value(FunctionValue, false, name), ret_type_(ret_type), parent_(module) {}

Function::~Function() = default;

void Function::addArgument(ArgumentPtr argument) {
    arguments_.emplace_back(std::move(argument));
}

void Function::addArgument(bool is_float, const std::string &argument_name) {
    arguments_.push_back(std::make_unique<Argument>(is_float, this, argument_name));
}

void Function::addBasicBlock(Value *block) {
    blocks_.push_back(std::unique_ptr<BasicBlock>(dynamic_cast<BasicBlock *>(block)));
}

void Function::dump() const {

}