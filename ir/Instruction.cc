//
// Created by 杨丰硕 on 2023/4/15.
//
#include <cassert>
#include "Function.h"
#include "BasicBlock.h"
#include "Instruction.h"
#include "GlobalVariable.h"
#include "IrFactory.h"

Instruction::Instruction(InstructionType type, BasicType basic_type, bool isptr, bool isbool, BasicBlock *block, const std::string &name):
        User(ValueType::InstructionValue, isptr, isbool, name),
        inst_type_(type),
        parent_(block),
        basic_type_(basic_type){}

Instruction::~Instruction() = default;

BinaryOpInstruction::BinaryOpInstruction(InstructionType type, BasicType basic_type, BasicBlock *block, Value *left, Value *right, const std::string &name):
        Instruction(type, basic_type, false, false, block, name){
    addOperand(left);
    addOperand(right);
}

std::vector<Value *> BinaryOpInstruction::getUses() {
    std::vector<Value *> results;
    if (getLeft()->getValueType() != ConstantValue) {
        results.push_back(getLeft());
    }
    if (getRight()->getValueType() != ConstantValue) {
        results.push_back(getRight());
    }
    return results;
}

Value *BinaryOpInstruction::getLeft() const {
    return getOperand(0);
}

Value *BinaryOpInstruction::getRight() const {
    return getOperand(1);
}

BinaryOpInstruction::~BinaryOpInstruction() = default;

UnaryOpInstruction::UnaryOpInstruction(InstructionType type, BasicType basic_type, BasicBlock *block, Value *value, const std::string &name):
        Instruction(type, basic_type, false, type == InstructionType::NotType, block, name){
    addOperand(value);
}

Value *UnaryOpInstruction::getValue() const {
    return getOperand(0);
}

std::vector<Value *> UnaryOpInstruction::getUses() {
    std::vector<Value *> results;
    if (getValue()->getValueType() != ConstantValue) {
        results.push_back(getValue());
    }
    return results;
}

UnaryOpInstruction::~UnaryOpInstruction() = default;

StoreInstruction::StoreInstruction(BasicBlock *block, BasicType basic_type, Value *value, Value *ptr, const std::string &name):
        Instruction(InstructionType::StoreType, basic_type, false, false, block, name){
    addOperand(value);
    addOperand(ptr);
}

StoreInstruction::~StoreInstruction() = default;

std::vector<Value *> StoreInstruction::getUses() {
    std::vector<Value *> results;
    if (getValue()->getValueType() != ConstantValue) {
        results.push_back(getValue());
    }
    if (getPtr()->getValueType() != ConstantValue) {
        results.push_back(getPtr());
    }
    return results;
}

LoadInstruction::LoadInstruction(BasicBlock *block, BasicType basic_type, Value *ptr, BasicType type, const std::string &name):
        Instruction(InstructionType::LoadType, basic_type, false, false, block, name){
    addOperand(ptr);
    setArrayDimension();
}

LoadInstruction::~LoadInstruction() = default;

Value *LoadInstruction::getPtr() const {
    return getOperand(0);
}

std::vector<Value *> LoadInstruction::getUses() {
    if (getPtr()->getValueType() != ConstantValue) {
        return {getPtr()};
    }
    return {};
}

bool LoadInstruction::isFromSecondaryPtr() const {
    auto alloac_inst = dynamic_cast<AllocaInstruction *>(getPtr());
    if (!alloac_inst) {
        return false;
    }
    return alloac_inst->isPtrPtr();
}

const std::vector<int32_t> &LoadInstruction::getArrayDimensionSize() const {
    return array_dimension_number_;
}

void LoadInstruction::setArrayDimension() {
    auto alloca_inst_value = dynamic_cast<AllocaInstruction *>(getPtr());
    if (alloca_inst_value) {
        array_dimension_number_ = alloca_inst_value->getArrayDimensionSize();
        return;
    }
    auto gep_inst_value = dynamic_cast<GEPInstruction *>(getPtr());
    if (gep_inst_value) {
        array_dimension_number_ = gep_inst_value->getArrayDimension();
    }
}

AllocaInstruction::AllocaInstruction(BasicBlock *block, BasicType type, bool isptrptr, const std::string &name):
        Instruction(InstructionType::AllocaType, type, true, false, block, name),
        is_ptr_ptr_(isptrptr){

}

AllocaInstruction::AllocaInstruction(BasicBlock *block, BasicType type, bool isptrptr, const std::vector<int32_t> &dimension, const std::string &name):
        Instruction(InstructionType::AllocaType, type, true, false, block, name),
        is_ptr_ptr_(isptrptr),
        array_dimension_size_(dimension){

}

std::vector<Value *> AllocaInstruction::getUses() {
    return {};
}

AllocaInstruction::~AllocaInstruction() = default;


CallInstruction::CallInstruction(BasicBlock *block, Function *function, std::vector<Value *> actuals,
                                 const std::string &name):
        Instruction(InstructionType::CallType, function->getRetType(), false, false, block, name),
        function_(function){
    for (auto value : actuals) {
        addOperand(value);
    }
}

std::vector<Value *> CallInstruction::getUses() {
    std::vector<Value *> results;
    for (int i = 0; i < getOperandNum(); ++i) {
        auto operand = getOperand(i);
        if (operand->getValueType() != ConstantValue) {
            results.push_back(operand);
        }
    }
    return results;
}

CallInstruction::~CallInstruction() = default;


RetInstruction::RetInstruction(BasicBlock *block, const std::string &name):
        Instruction(InstructionType::RetType, BasicType::VOID_BTYPE, false, false, block, name){

}

RetInstruction::RetInstruction(BasicBlock *block, BasicType basic_type, Value *value, const std::string &name):
        Instruction(InstructionType::RetType, basic_type, false, false, block, name){
    addOperand(value);
}

std::vector<Value *> RetInstruction::getUses() {
    std::vector<Value *> results;
    if (getRetValue() != nullptr && getRetValue()->getValueType() != ConstantValue) {
        results.push_back(getRetValue());
    }
    return results;
}

RetInstruction::~RetInstruction() = default;

BranchInstruction::BranchInstruction(BasicBlock *block, Value *label, const std::string &name):
        Instruction(InstructionType::BrType, BasicType::VOID_BTYPE, false, false, block, name),
        is_cond_(false){
    addOperand(label);
}

BranchInstruction::BranchInstruction(BasicBlock *block, Value *cond, Value *true_label, Value *false_label,
                                     const std::string &name):
        Instruction(InstructionType::BrType, BasicType::VOID_BTYPE, false, false, block, name),
        is_cond_(true){
    addOperand(cond);
    addOperand(true_label);
    addOperand(false_label);
}

std::vector<Value *> BranchInstruction::getUses() {
    if (is_cond_) {
        return {getCond()};
    }
    return {};
}

BranchInstruction::~BranchInstruction() = default;

SetCondInstruction::SetCondInstruction(BasicBlock *block, CmpCondType cmptype, bool is_float, Value *left, Value *right,
                                       const std::string &name):
        Instruction(InstructionType::SetCondType, BasicType::INT_BTYPE,false, true, block, name),
        is_float_(is_float),
        cmp_cond_type_(cmptype){
    addOperand(left);
    addOperand(right);
}

std::vector<Value *> SetCondInstruction::getUses() {
    std::vector<Value *> results;
    results.reserve(2);
    if (getLeft()->getValueType() != ConstantValue) {
        results.push_back(getLeft());
    }
    if (getRight()->getValueType() != ConstantValue) {
        results.push_back(getRight());
    }
    return results;
}

SetCondInstruction::~SetCondInstruction() = default;

CastInstruction::CastInstruction(BasicBlock *block, bool is_i2f, Value *value, const std::string &name):
        Instruction(InstructionType::CastType,
                    is_i2f ? BasicType::FLOAT_BTYPE : BasicType::INT_BTYPE,
                    false, true, block, name),
        is_i2f_(is_i2f) {
    addOperand(value);
}

std::vector<Value *> CastInstruction::getUses() {
    if (getValue()->getValueType() != ConstantValue) {
        return {getValue()};
    }
    return {};
}

CastInstruction::~CastInstruction() = default;

PhiInstruction::PhiInstruction(BasicBlock *block, BasicType basic_type, const std::vector<Value *> &values,const std::vector<BasicBlock *> &bbs,
                               const std::string &name):
        Instruction(InstructionType::PhiType, basic_type, false, false, block, name){
    assert(values.size() == bbs.size());
    for (auto value : values) {
        addOperand(value);
    }
    for (auto bb : bbs) {
        addOperand(bb);
    }
}

std::vector<Value *> PhiInstruction::getUses() {
    return {};
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

GEPInstruction::GEPInstruction(BasicBlock *block, BasicType btype, Value *base, bool ptr_offset, const std::vector<Value *> &indexes,
                               const std::string &name):
        Instruction(InstructionType::GEPType, btype, true, false, block, name),
        is_ptr_offset_(ptr_offset){
    addOperand(base);
    setArrayDimension();
    for (auto value : indexes) {
        addOperand(value);
    }
}

std::vector<Value *> GEPInstruction::getUses() {
    std::vector<Value *> results;
    for (int i = 0; i < getOperandNum(); ++i) {
        auto operand = getOperand(i);
        if (operand->getValueType() != ConstantValue) {
            results.push_back(operand);
        }
    }
    return results;
}

GEPInstruction::~GEPInstruction() = default;

void GEPInstruction::setArrayDimension() {
    auto ptr_value = getPtr();
    if (auto ptr_gep_inst = dynamic_cast<GEPInstruction *>(ptr_value)) {
        auto tmp_dimension = ptr_gep_inst->getArrayDimension();
        if (!tmp_dimension.empty()) {
            array_dimension_numbers_ = std::vector<int32_t>(tmp_dimension.begin() + 1, tmp_dimension.end());
        }
    } else if (auto alloca_inst_value = dynamic_cast<AllocaInstruction *>(ptr_value)) {
        array_dimension_numbers_ = alloca_inst_value->getArrayDimensionSize();
    } else if (auto load_inst_value = dynamic_cast<LoadInstruction *>(ptr_value)) {
        array_dimension_numbers_ = load_inst_value->getArrayDimensionSize();
    } else if (auto global_value = dynamic_cast<GlobalVariable *>(ptr_value)) {
        auto global_const_value = dynamic_cast<ConstantArray *>(global_value->getConstInit());
        assert(global_const_value);
        array_dimension_numbers_ = global_const_value->getDimensionNumbers();
    }
    if (!array_dimension_numbers_.empty() && !is_ptr_offset_) {
        addOperand(IrFactory::createIConstantVar(0));
    }
}

MemSetInstruction::MemSetInstruction(BasicBlock *block, BasicType btype, Value *base, Value *size, Value *value):
        Instruction(InstructionType::MemSetType, btype, false, false, block){
    addOperand(base);
    addOperand(size);
    addOperand(value);
}

std::vector<Value *> MemSetInstruction::getUses() {
    std::vector<Value *> results;
    if (getBase()->getValueType() != ConstantValue) {
        results.push_back(getBase());
    }
    if (getSize()->getValueType() != ConstantValue) {
        results.push_back(getSize());
    }
    if (getValue()->getValueType() != ConstantValue) {
        results.push_back(getValue());
    }
    return results;
}

MemSetInstruction::~MemSetInstruction() = default;

ZextInstruction::ZextInstruction(BasicBlock *block, Value *left, const std::string &name):
        Instruction(InstructionType::ZextType, BasicType::INT_BTYPE, false, false, block, name){
    addOperand(left);
}

std::vector<Value *> ZextInstruction::getUses() {
    std::vector<Value *> results;
    if (getValue()->getValueType() != ConstantValue) {
        results.push_back(getValue());
    }
    return results;
}

ZextInstruction::~ZextInstruction() = default;