//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Instruction.h"

Instruction::Instruction(InstructionType type, BasicBlock *block):
        User(ValueType::InstructionValue),
        inst_type_(type),
        parent_(block) {}

Instruction::~Instruction() = default;

BinaryOpInstruction::BinaryOpInstruction(InstructionType type, BasicBlock *block, BinaryOpType optype):
        Instruction(type, block),
        optype_(optype){}

BinaryOpInstruction::~BinaryOpInstruction() = default;

UnaryOpInstruction::UnaryOpInstruction(InstructionType type, BasicBlock *block, UnaryOpType optype):
        Instruction(type, block),
        optype_(optype){}

UnaryOpInstruction::~UnaryOpInstruction() = default;

