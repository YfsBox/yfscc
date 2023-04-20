//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include "BasicBlock.h"
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


CallInstruction::CallInstruction(BasicBlock *block, Function *function, std::vector<Value *> actuals,
                                 const std::string &name):
        Instruction(InstructionType::CallType, block, name),
        function_(function){
    for (auto value : actuals) {
        addOperand(value);
    }
}

CallInstruction::~CallInstruction() = default;


RetInstruction::RetInstruction(BasicBlock *block, const std::string &name):
        Instruction(InstructionType::RetType, block, name),
        ret_type_(BasicType::VOID_BTYPE){

}

RetInstruction::RetInstruction(BasicBlock *block, Value *value, BasicType btype, const std::string &name):
        Instruction(InstructionType::RetType, block, name),
        ret_type_(btype){
    addOperand(value);
}

RetInstruction::~RetInstruction() = default;

BranchInstruction::BranchInstruction(BasicBlock *block, Value *label, const std::string &name):
        Instruction(InstructionType::BrType, block, name){
    addOperand(label);
}

BranchInstruction::BranchInstruction(BasicBlock *block, Value *cond, Value *true_label, Value *false_label,
                                     const std::string &name):
        Instruction(InstructionType::BrType, block, name){
    addOperand(cond);
    addOperand(true_label);
    addOperand(false_label);
}

BranchInstruction::~BranchInstruction() = default;

SetCondInstruction::SetCondInstruction(BasicBlock *block, CmpCondType cmptype, bool is_float, Value *left, Value *right,
                                       const std::string &name):
        Instruction(InstructionType::SetCondType, block, name),
        is_float_(is_float),
        cmp_cond_type_(cmptype){
    addOperand(left);
    addOperand(right);
}

SetCondInstruction::~SetCondInstruction() = default;

CastInstruction::CastInstruction(BasicBlock *block, bool is_i2f, Value *value, const std::string &name):
        Instruction(InstructionType::CastType, block, name),
        is_i2f_(is_i2f) {
    addOperand(value);
}

CastInstruction::~CastInstruction() = default;

PhiInstruction::PhiInstruction(BasicBlock *block,const std::vector<Value *> &values,const std::vector<BasicBlock *> &bbs,
                               const std::string &name):
        Instruction(InstructionType::PhiType, block, name){
    assert(values.size() == bbs.size());
    for (auto value : values) {
        addOperand(value);
    }
    for (auto bb : bbs) {
        addOperand(bb);
    }
}

PhiInstruction::~PhiInstruction() = default;

PhiInstruction::ValueBlockPair PhiInstruction::getValueBlock(int idx) const {
    auto phi_size = getSize();
    ValueBlockPair pair;
    pair.first = getOperand(idx);
    auto bb = getOperand(idx + phi_size);
    pair.second = dynamic_cast<BasicBlock*>(bb);
    return pair;
}

BasicBlock *PhiInstruction::getBasicBlock(int idx) const {
    auto value = getOperand(idx + getSize());
    return dynamic_cast<BasicBlock*> (value);
}

GEPInstruction::GEPInstruction(BasicBlock *block, BasicType btype, Value *ptr, Value *offset, const std::string &name):
        Instruction(InstructionType::GEPType, block, name),
        type_(btype){
    addOperand(ptr);
    addOperand(offset);
}

GEPInstruction::~GEPInstruction() = default;