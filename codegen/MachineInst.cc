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
    if (operand->getValueType() == MachineOperand::Undef) {
        return ValueType::Undef;
    }
    if (operand->getValueType() == MachineOperand::Int) {
        return ValueType::Int;
    }
    return ValueType::Float;
}

MoveInst::MoveInst(MachineBasicBlock *parent, MoveType movtype, MachineOperand *src, MachineOperand *dst):
    MachineInst(Move, Undef, parent),
    dst_(dst),
    src_(src),
    mov_type_(movtype){
    setValueType(getValueType(dst_));
}

MoveInst::MoveInst(MachineBasicBlock *parent, MachineOperand *src, MachineOperand *dst):
    MachineInst(Move, Undef, parent),
    dst_(dst),
    src_(src){
    setValueType(getValueType(dst_));
}

MoveInst::~MoveInst() = default;

ClzInst::ClzInst(MachineBasicBlock *parent, MachineOperand *dst):
    MachineInst(Clz, Undef, parent),
    dst_(dst){
    setValueType(getValueType(dst_));
}

ClzInst::~ClzInst() = default;

BinaryInst::BinaryInst(MachineBasicBlock *parent, BinaryOp op, MachineOperand *dst, MachineOperand *src):
    MachineInst(Binary, Undef, parent),
    binary_op_type_(op),
    dst_(dst),
    src_(src){
    setValueType(getValueType(dst_));
}

BinaryInst::~BinaryInst() = default;

CmpInst::CmpInst(MachineBasicBlock *parent, MachineOperand *lhs, MachineOperand *rhs):
    MachineInst(Cmp, Undef, parent),
    lhs_(lhs),
    rhs_(rhs){
    setValueType(getValueType(lhs));
}

CmpInst::~CmpInst() = default;

CallInst::CallInst(MachineBasicBlock *parent, Label *label):
    MachineInst(Call, Undef, parent),
    call_label_(label){

}

CallInst::~CallInst() = default;

BranchInst::BranchInst(MachineBasicBlock *parent, Label *label):
    MachineInst(Branch, Undef, parent),
    br_label_(label){

}

BranchInst::~BranchInst() = default;

RetInst::RetInst(MachineBasicBlock *parent):
    MachineInst(Ret, Undef, parent){

}

RetInst::~RetInst() = default;

PushInst::PushInst(MachineBasicBlock *parent):
    MachineInst(Push, Undef, parent){

}

PopInst::PopInst(MachineBasicBlock *parent):
    MachineInst(Pop, Undef, parent) {

}

StoreInst::StoreInst(MemIndexType index_type, MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset):
    MachineInst(Store, Undef, parent),
    index_type_(index_type),
    dst_(dst),
    base_(base),
    offset_(offset){

}

StoreInst::~StoreInst() = default;

LoadInst::LoadInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset):
    MachineInst(Load, Undef, parent),
    dst_(dst),
    base_(base),
    offset_(offset){
    setValueType(getValueType(dst));
}

LoadInst::~LoadInst() = default;







