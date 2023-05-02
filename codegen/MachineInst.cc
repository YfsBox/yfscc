//
// Created by 杨丰硕 on 2023/5/2.
//
#include "MachineInst.h"
#include "MachineOperand.h"
#include "../common/Utils.h"

MachineInst::MachineInst(MachineInstType mtype, ValueType value_type, MachineBasicBlock *parent):
    value_type_(value_type),
    machine_inst_type_(mtype),
    parent_(parent){

}

MachineInst::~MachineInst() = default;

MachineInst::ValueType MachineInst::getValueType(MachineOperand *operand) {
    if (operand->getOperandType() == MachineOperand::Undef) {
        return ValueType::Undef;
    }
    if (operand->getOperandType() == MachineOperand::Int) {
        return ValueType::Int;
    }
    return ValueType::Float;
}


MoveInst::MoveInst(MachineBasicBlock *parent, MachineOperand *src, MachineOperand *dst):
    MachineInst(Move, Undef, parent),
    dst_(GET_UNIQUEPTR(dst)),
    src_(GET_UNIQUEPTR(src)){
    setValueType(getValueType(src_.get()));
}

std::string MoveInst::dump() {
    return "";
}

MoveInst::~MoveInst() = default;

ClzInst::ClzInst(MachineBasicBlock *parent, MachineOperand *dst):
    MachineInst(Clz, Undef, parent),
    dst_(GET_UNIQUEPTR(dst)){
    setValueType(getValueType(dst_.get()));
}

std::string ClzInst::dump() {
    return "";
}

ClzInst::~ClzInst() = default;

BinaryInst::BinaryInst(MachineBasicBlock *parent, BinaryOp op, MachineOperand *dst, MachineOperand *src):
    MachineInst(Binary, Undef, parent),
    binary_op_type_(op),
    dst_(GET_UNIQUEPTR(dst)),
    src_(GET_UNIQUEPTR(src)){
    setValueType(getValueType(dst_.get()));
}

std::string BinaryInst::dump() {
    return "";
}

BinaryInst::~BinaryInst() = default;

CmpInst::CmpInst(MachineBasicBlock *parent, MachineOperand *lhs, MachineOperand *rhs):
    MachineInst(Cmp, Undef, parent),
    lhs_(GET_UNIQUEPTR(lhs)),
    rhs_(GET_UNIQUEPTR(rhs)){
    setValueType(getValueType(lhs));
}

std::string CmpInst::dump() {
    return "";
}

CmpInst::~CmpInst() = default;

CallInst::CallInst(MachineBasicBlock *parent, Label *label):
    MachineInst(Call, Undef, parent),
    call_label_(label){

}

std::string CallInst::dump() {
    return "";
}

CallInst::~CallInst() = default;

BranchInst::BranchInst(MachineBasicBlock *parent, Label *label):
    MachineInst(Branch, Undef, parent),
    br_label_(label){

}

std::string BranchInst::dump() {
    return "";
}

BranchInst::~BranchInst() = default;

RetInst::RetInst(MachineBasicBlock *parent):
    MachineInst(Ret, Undef, parent){

}

std::string RetInst::dump() {
    return "";
}

PushInst::PushInst(MachineBasicBlock *parent):
    MachineInst(Push, Undef, parent){

}

std::string PushInst::dump() {
    return "";
}

PopInst::PopInst(MachineBasicBlock *parent):
    MachineInst(Pop, Undef, parent) {

}

StoreInst::StoreInst(MemIndexType index_type, MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset):
    MachineInst(Store, Undef, parent),
    index_type_(index_type),
    dst_(GET_UNIQUEPTR(dst)),
    base_(GET_UNIQUEPTR(base)),
    offset_(GET_UNIQUEPTR(offset)){

}

std::string StoreInst::dump() {
    return "";
}

StoreInst::~StoreInst() = default;

LoadInst::LoadInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset):
    MachineInst(Load, Undef, parent),
    dst_(GET_UNIQUEPTR(dst)),
    base_(GET_UNIQUEPTR(base)),
    offset_(GET_UNIQUEPTR(offset)){
    setValueType(getValueType(dst));
}

std::string LoadInst::dump() {
    return "";
}

LoadInst::~LoadInst() = default;







