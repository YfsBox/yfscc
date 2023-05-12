//
// Created by 杨丰硕 on 2023/5/2.
//
#include "MachineInst.h"
#include "MachineOperand.h"
#include "../common/Utils.h"

MachineInst::MachineInst(MachineInstType mtype, ValueType value_type, MachineBasicBlock *parent):
    no_(-1),
    value_type_(value_type),
    machine_inst_type_(mtype),
    parent_(parent){

}

MachineInst::~MachineInst() = default;

std::vector<MachineOperand *> MachineInst::getUses(MachineInst *inst) {
    std::vector<MachineOperand *> operands;
    switch (inst->machine_inst_type_) {
        case MachineInstType::Move:
            operands.push_back(dynamic_cast<MoveInst*>(inst)->getSrc());
            break;
        case MachineInstType::Clz:
            operands.push_back(dynamic_cast<ClzInst*>(inst)->getSrc());
            break;
        case MachineInstType::Vneg:
            operands.push_back(dynamic_cast<VnegInst*>(inst)->getSrc());
            break;
        case MachineInstType::Binary: {
            auto binary_inst = dynamic_cast<BinaryInst *>(inst);
            operands.push_back(binary_inst->getLeft());
            operands.push_back(binary_inst->getRight());
            break;
        }
        case MachineInstType::Load:{
            auto load_inst = dynamic_cast<LoadInst *>(inst);
            operands.push_back(load_inst->getOffset());
            operands.push_back(load_inst->getBase());
            break;
        }
        case MachineInstType::Cvt:
            operands.push_back(dynamic_cast<CvtInst*>(inst)->getSrc());
            break;
        case MachineInstType::Store:{
            auto store_inst = dynamic_cast<StoreInst*>(inst);
            operands.push_back(store_inst->getBase());
            operands.push_back(store_inst->getOffset());
            operands.push_back(store_inst->getValue());
            break;
        }
        case MachineInstType::Cmp: {
            auto cmp_inst = dynamic_cast<CmpInst *>(inst);
            operands.push_back(cmp_inst->getLhs());
            operands.push_back(cmp_inst->getRhs());
        }
        default:
            break;
    }
    return operands;
}

// 不考虑非Virtual寄存器的话
std::vector<MachineOperand *> MachineInst::getDefs(MachineInst *inst) {
    std::vector<MachineOperand *> operands;
    switch (inst->machine_inst_type_) {
        case MachineInstType::Move:
            operands.push_back(dynamic_cast<MoveInst*>(inst)->getDst());
            break;
        case MachineInstType::Vneg:
            operands.push_back(dynamic_cast<VnegInst*>(inst)->getDst());
            break;
        case MachineInstType::Cvt:
            operands.push_back(dynamic_cast<CvtInst*>(inst)->getDst());
            break;
        case MachineInstType::Load:
            operands.push_back(dynamic_cast<LoadInst*>(inst)->getDst());
            break;
        case MachineInstType::Binary:
            operands.push_back(dynamic_cast<BinaryInst*>(inst)->getDst());
            break;
        case MachineInstType::Clz:
            operands.push_back(dynamic_cast<ClzInst*>(inst)->getDst());
        default:
            break;
    }
    return operands;
}

MachineInst::ValueType MachineInst::getValueType(MachineOperand *operand) {
    if (operand == nullptr || operand->getValueType() == MachineOperand::Undef) {
        return ValueType::Undef;
    }
    if (operand->getValueType() == MachineOperand::Int) {
        return ValueType::Int;
    }
    return ValueType::Float;
}

MoveInst::MoveInst(MachineBasicBlock *parent, MoveType movtype, MachineOperand *src, MachineOperand *dst):
    MachineInst(Move, Undef, parent),
    mov_cond_(MoveNoCond),
    dst_(dst),
    src_(src),
    mov_type_(movtype){
    setValueType(getValueType(dst_));
}

MoveInst::MoveInst(MachineBasicBlock *parent, MachineOperand *src, MachineOperand *dst):
    MachineInst(Move, Undef, parent),
    mov_cond_(MoveNoCond),
    dst_(dst),
    src_(src){
    auto dst_value_type = getValueType(dst_);
    auto src_value_type = getValueType(src_);
    setValueType(dst_value_type);
    if (dst_value_type == src_value_type) {
        if (dst_value_type == ValueType::Int) {
            mov_type_ = MoveType::I2I;
        } else {
            mov_type_ = MoveType::F2F;
        }
    } else {
        mov_type_ = MoveType::F_I;
    }
}

MoveInst::~MoveInst() = default;

ClzInst::ClzInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *src):
    MachineInst(Clz, Undef, parent),
    dst_(dst),
    src_(src){
    setValueType(getValueType(dst_));
}

ClzInst::~ClzInst() = default;

BinaryInst::BinaryInst(MachineBasicBlock *parent, BinaryOp op, MachineOperand *dst, MachineOperand *lhs, MachineOperand *rhs):
    MachineInst(Binary, Undef, parent),
    binary_op_type_(op),
    dst_(dst),
    lhs_(lhs),
    rhs_(rhs){
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

BranchInst::BranchInst(MachineBasicBlock *parent, MachineOperand *operand, BranchCond cond, BranchType branch_type):
    MachineInst(Branch, Undef, parent),
    br_cond_(cond),
    br_type_(branch_type),
    br_operand_(operand){

}

BranchInst::~BranchInst() = default;

RetInst::RetInst(MachineBasicBlock *parent):
    MachineInst(Ret, Undef, parent){

}

RetInst::~RetInst() = default;

PushInst::PushInst(MachineBasicBlock *parent, ValueType valuetype):
    MachineInst(Push, valuetype, parent){

}

PopInst::PopInst(MachineBasicBlock *parent, ValueType valuetype):
    MachineInst(Pop, valuetype, parent) {

}

StoreInst::StoreInst(MemIndexType index_type, MachineBasicBlock *parent, MachineOperand *value, MachineOperand *base, MachineOperand *offset):
    MachineInst(Store, Undef, parent),
    index_type_(index_type),
    value_(value),
    base_(base),
    offset_(offset){

}

StoreInst::StoreInst(MachineBasicBlock *parent, MachineOperand *value, MachineOperand *base):
    MachineInst(Store, Undef, parent),
    value_(value),
    base_(base),
    offset_(nullptr){

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

VnegInst::VnegInst(MachineBasicBlock *parent, OperandPtr dst, OperandPtr src):
        MachineInst(Vneg, Float, parent),
        dst_(dst),
        src_(src){

}

VnegInst::~VnegInst() = default;

CvtInst::CvtInst(MachineBasicBlock *parent, CvtType cvt_type, OperandPtr dst, OperandPtr src):
        MachineInst(Cvt, Undef, parent),
        cvt_type_(cvt_type),
        dst_(dst),
        src_(src){
    if (cvt_type == I2F) {
        setValueType(ValueType::Float);
    } else {
        setValueType(ValueType::Int);
    }
}






