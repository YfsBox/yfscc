//
// Created by 杨丰硕 on 2023/4/15.
//
#include "BasicBlock.h"

BasicBlock::BasicBlock(Function *func, const std::string &lebal):
    Value(ValueType::BasicBlockValue, false, lebal), owner_function_(func) {}

BasicBlock::~BasicBlock() = default;


