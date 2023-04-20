//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_BASICBLOCK_H
#define YFSCC_BASICBLOCK_H

#include <list>
#include <memory>
#include "Value.h"
#include "Instruction.h"

class Function;

class BasicBlock: public Value {
public:
    using InstructionPtr = std::unique_ptr<Instruction>;
    using BlockList = std::list<BasicBlock *>;

    BasicBlock(Function *func, const std::string &lebal);

    ~BasicBlock();

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

    void addSuccessorBlock(BasicBlock *block) {
        successor_blocks_.push_front(block);
    }

    void addPredecessorBlock(BasicBlock *block) {
        predecessor_blocks_.push_front(block);
    }

    void addTerminal(BasicBlock *block) {
        terminals_.push_front(block);
    }

    Function *getFunction() const {
        return owner_function_;
    }

    std::list<InstructionPtr> &getInstructionList() {
        return instructions_;
    }

private:
    Function *owner_function_;

    std::list<InstructionPtr> instructions_;

    BlockList successor_blocks_;

    BlockList predecessor_blocks_;

    BlockList terminals_;
};

#endif //YFSCC_BASICBLOCK_H
