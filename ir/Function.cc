//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Function.h"
#include "BasicBlock.h"

Function::Function(BasicType ret_type, Module *module, const std::string &name):
    Value(FunctionValue, false, false, name),
    ret_type_(ret_type),
    parent_(module) {

}

Function::~Function() = default;

void Function::addArgument(ArgumentPtr argument) {
    arguments_.emplace_back(std::move(argument));
}

void Function::addBasicBlock(Value *block) {
    blocks_.push_back(std::unique_ptr<BasicBlock>(dynamic_cast<BasicBlock *>(block)));
}


void Function::removeEmptyBasicBlock() {
    for (auto bbit = blocks_.begin(); bbit != blocks_.end();) {
        if (bbit->get()->getInstructionSize() == 0) {
            BasicBlock *bb = bbit->get();
            for (auto pre: bb->getPreDecessorBlocks()) {
                pre->removeSuccessorBlock(bb);
            }
            for (auto succ: bb->getSuccessorBlocks()) {
                succ->removePredecessorBlock(bb);
            }
            bbit = blocks_.erase(bbit);
        } else {
            ++bbit;
        }
    }
}

void Function::dump() const {

}

void Function::bindBasicBlocks() {
    for (auto &bb: blocks_) {
        BranchInstruction *br_inst = bb->getBranchInst();
        if (br_inst && !bb->getHasRet()) {
            if (!br_inst->isCondBranch()) {
                BasicBlock::bindBasicBlock(bb.get(), dynamic_cast<BasicBlock *>(br_inst->getLabel()));
            } else {
                BasicBlock::bindBasicBlock(bb.get(), dynamic_cast<BasicBlock *>(br_inst->getTrueLabel()));
                BasicBlock::bindBasicBlock(bb.get(), dynamic_cast<BasicBlock *>(br_inst->getFalseLabel()));
            }
        }
    }
}