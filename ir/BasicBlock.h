//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_BASICBLOCK_H
#define YFSCC_BASICBLOCK_H

#include <list>
#include <memory>
#include "Value.h"

class Instruction;
class Function;

class BasicBlock: public Value {
public:
    using InstructionPtr = std::unique_ptr<Instruction>;
    using BlockList = std::list<BasicBlock *>;

    explicit BasicBlock(Function *func);

    ~BasicBlock() = default;

    size_t getInstructionSize() const {
        return instructions_.size();
    }

    BlockList &getSuccessorBlocks() {
        return successor_blocks_;
    }

    BlockList &getPreDecessorBlocks() {
        return predecessor_blocks_;
    }

    void addInstruction(Instruction *instruction);

    void addSuccessorBlock(BasicBlock *block);

    void addPredecessorBlock(BasicBlock *block);

private:
    Function *owner_function_;

    std::list<InstructionPtr> instructions_;

    BlockList successor_blocks_;

    BlockList predecessor_blocks_;
};

#endif //YFSCC_BASICBLOCK_H
