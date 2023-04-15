//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_INSTRUCTION_H
#define YFSCC_INSTRUCTION_H

#include "User.h"
#include "../common/Types.h"

enum InstructionType {
    RetType,
    BrType,
    AddType,
    SubType,
    MulType,
    DivType,
    AndType,
    OrType,
    XorType,
    AllocaType,
    LoadType,
    StoreType,
};

class BasicBlock;

class Instruction: public User {
public:
    Instruction(InstructionType type, BasicBlock *block):
        User(ValueType::InstructionValue),
        inst_type_(type),
        parent_(block) {}

    ~Instruction() = default;

    BasicBlock *getParent() const {
        return parent_;
    }

    InstructionType getInstType() const {
        return inst_type_;
    }

protected:
    InstructionType inst_type_;
    BasicBlock *parent_;
};

class BinaryOpInstruction: public Instruction {
public:
    BinaryOpInstruction(InstructionType type, BasicBlock *block, BinaryOpType optype):
            Instruction(type, block),
            optype_(optype){}

    BinaryOpType getOpType() const {
        return optype_;
    }
private:
    BinaryOpType optype_;
};

class UnaryOpInstruction: public Instruction {
public:
    UnaryOpInstruction(InstructionType type, BasicBlock *block, UnaryOpType optype):
            Instruction(type, block),
            optype_(optype){}

    UnaryOpType getOptype() const {
        return optype_;
    }

private:
    UnaryOpType optype_;
};

#endif //YFSCC_INSTRUCTION_H
