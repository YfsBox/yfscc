//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include "BasicBlock.h"

BasicBlock::BasicBlock(Function *func, const std::string &lebal):
    Value(ValueType::BasicBlockValue, false, false, lebal), has_jump_(false), has_ret_(false), loop_depth_(0), owner_function_(func), branch_inst_(nullptr) {}

BasicBlock::~BasicBlock() = default;

void BasicBlock::insertInstruction(InstructionListIt inst_it, Instruction *inst) {
    instructions_.insert(inst_it, std::unique_ptr<Instruction>(inst));
}

void BasicBlock::bindBasicBlock(BasicBlock *pre, BasicBlock *succ) {
    pre->addSuccessorBlock(succ);
    assert(succ);
    succ->addPredecessorBlock(pre);
}


