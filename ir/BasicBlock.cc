//
// Created by 杨丰硕 on 2023/4/15.
//
#include "BasicBlock.h"

BasicBlock::BasicBlock(Function *func, const std::string &lebal):
    Value(ValueType::BasicBlockValue, false, lebal), has_jump_(false), owner_function_(func) {}

BasicBlock::~BasicBlock() = default;

void BasicBlock::bindBasicBlock(BasicBlock *pre, BasicBlock *succ) {
    pre->addSuccessorBlock(succ);
    succ->addPredecessorBlock(pre);
}


