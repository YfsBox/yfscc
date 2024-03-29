//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_BASICBLOCK_H
#define YFSCC_BASICBLOCK_H

#include <list>
#include <memory>
#include <unordered_set>
#include "Value.h"
#include "Instruction.h"
#include "Function.h"

class Function;

class BasicBlock: public Value {
public:
    using InstructionPtr = std::unique_ptr<Instruction>;
    using BlockList = std::unordered_set<BasicBlock *>;
    using InstructionListIt = std::list<InstructionPtr>::iterator;

    BasicBlock(Function *func, const std::string &lebal);

    ~BasicBlock();

    static void bindBasicBlock(BasicBlock *pre, BasicBlock *succ);

    size_t getInstructionSize() const {
        return instructions_.size();
    }

    BlockList &getSuccessorBlocks() {
        return successor_blocks_;
    }

    BlockList &getPreDecessorBlocks() {
        return predecessor_blocks_;
    }

    BlockList &getTerminals() {
        return terminals_;
    }

    void addInstruction(Value *instruction) {
        instructions_.push_back(std::unique_ptr<Instruction>(dynamic_cast<Instruction *>(instruction)));
    }

    void addInstruction(InstructionPtr instruction) {
        instructions_.push_back(std::move(instruction));
    }

    void addSuccessorBlock(BasicBlock *block) {
        successor_blocks_.insert(block);
    }

    void addPredecessorBlock(BasicBlock *block) {
        predecessor_blocks_.insert(block);
    }

    void removePredecessorBlock(BasicBlock *block) {
        predecessor_blocks_.erase(block);
    }

    void removeSuccessorBlock(BasicBlock *block) {
        successor_blocks_.erase(block);
    }

    void addTerminal(BasicBlock *block) {
        terminals_.insert(block);
    }

    Function *getFunction() const {
        return owner_function_;
    }

    std::list<InstructionPtr> &getInstructionList() {
        return instructions_;
    }

    void setHasJump(bool jump) {
        has_jump_ = jump;
    }

    bool getHasJump() const {
        return has_jump_;
    }

    void setHasRet(bool ret) {
        has_ret_ = ret;
    }

    bool getHasRet() const {
        return has_ret_;
    }

    void clearSuccessors() {
        for (auto succ: successor_blocks_) {
            succ->removePredecessorBlock(this);
        }
        successor_blocks_.clear();
    }

    void clearPresuccessors() {
        for (auto pre: predecessor_blocks_) {
            pre->removeSuccessorBlock(this);
        }
        predecessor_blocks_.clear();
    }

    void setBranchInst(BranchInstruction *inst) {
        branch_inst_ = inst;
    }

    BranchInstruction *getBranchInst() {
        return branch_inst_;
    }

    bool isEnterBasicBlock() const {
        return name_ == owner_function_->getName() + "0" || name_ == ".L0";
    }

    void setWhileLoopDepth(int32_t depth) {
        loop_depth_ = depth;
    }

    int32_t getWhileLoopDepth() const {
        return loop_depth_;
    }

    void addFrontInstruction(Value *instruction) {
        instructions_.push_front(std::unique_ptr<Instruction>(dynamic_cast<Instruction *>(instruction)));
    }

    void setName(const std::string &name) {
        name_ = name;
    }

    void insertInstruction(InstructionListIt inst_it, Instruction *inst);

private:
    bool has_jump_;

    bool has_ret_;

    int32_t loop_depth_;

    Function *owner_function_;

    std::list<InstructionPtr> instructions_;

    BlockList successor_blocks_;

    BlockList predecessor_blocks_;

    BlockList terminals_;

    BranchInstruction *branch_inst_;
};

#endif //YFSCC_BASICBLOCK_H
