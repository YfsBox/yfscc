//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Instruction.h"

Instruction::Instruction(InstructionType type, BasicBlock *block, const std::string &name):
        User(ValueType::InstructionValue, name),
        inst_type_(type),
        parent_(block){}

Instruction::~Instruction() = default;

BinaryOpInstruction::BinaryOpInstruction(InstructionType type, BasicBlock *block, Value *left, Value *right, const std::string &name):
        Instruction(type, block, name){
    addOperand(left);
    addOperand(right);
}

Value *BinaryOpInstruction::getLeft() const {
    return getOperand(0);
}

Value *BinaryOpInstruction::getRight() const {
    return getOperand(1);
}

BinaryOpInstruction::~BinaryOpInstruction() = default;

UnaryOpInstruction::UnaryOpInstruction(InstructionType type, BasicBlock *block, Value *value, const std::string &name):
        Instruction(type, block, name){
    addOperand(value);
}

Value *UnaryOpInstruction::getValue() const {
    return getOperand(0);
}

UnaryOpInstruction::~UnaryOpInstruction() = default;

StoreInstruction::StoreInstruction(BasicBlock *block, Value *value, Value *ptr, const std::string &name):
        Instruction(InstructionType::StoreType, block, name){
    addOperand(value);
    addOperand(ptr);
}

StoreInstruction::~StoreInstruction() = default;

LoadInstruction::LoadInstruction(BasicBlock *block, Value *ptr, BasicType type, const std::string &name):
        Instruction(InstructionType::LoadType, block, name),
        value_type_(type){
    addOperand(ptr);
}

LoadInstruction::~LoadInstruction() = default;

AllocaInstruction::AllocaInstruction(BasicBlock *block, BasicType type, const std::string &name):
        Instruction(InstructionType::AllocaType, block, name),
        is_array_(false),
        value_size_(1),
        value_type_(type){

}

AllocaInstruction::AllocaInstruction(BasicBlock *block, BasicType type, size_t array_size, const std::string &name):
        Instruction(InstructionType::AllocaType, block, name),
        is_array_(true),
        value_size_(array_size),
        value_type_(type){

}

AllocaInstruction::~AllocaInstruction() = default;



