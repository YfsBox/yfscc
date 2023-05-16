//
// Created by 杨丰硕 on 2023/5/2.
//
#include <cassert>
#include <unordered_set>
#include "Machine.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "../common/Utils.h"

MachineInst::MachineInst(MachineInstType mtype, ValueType value_type, MachineBasicBlock *parent):
    value_type_(value_type),
    machine_inst_type_(mtype),
    parent_(parent){

}

MachineInst::~MachineInst() = default;

std::unordered_set<MachineOperand *> MachineInst::getUses(MachineInst *inst, bool is_float) {
    std::unordered_set<MachineOperand *> operands;
    switch (inst->machine_inst_type_) {
        case MachineInstType::Move:
            operands.insert(dynamic_cast<MoveInst*>(inst)->getSrc());
            break;
        case MachineInstType::Clz:
            operands.insert(dynamic_cast<ClzInst*>(inst)->getSrc());
            break;
        case MachineInstType::Vneg:
            operands.insert(dynamic_cast<VnegInst*>(inst)->getSrc());
            break;
        case MachineInstType::Binary: {
            auto binary_inst = dynamic_cast<BinaryInst *>(inst);
            operands.insert(binary_inst->getLeft());
            operands.insert(binary_inst->getRight());
            break;
        }
        case MachineInstType::Load:{
            auto load_inst = dynamic_cast<LoadInst *>(inst);
            if (load_inst->getOffset()) {
                operands.insert(load_inst->getOffset());
            }
            operands.insert(load_inst->getBase());
            break;
        }
        case MachineInstType::Cvt:
            operands.insert(dynamic_cast<CvtInst*>(inst)->getSrc());
            break;
        case MachineInstType::Store:{
            auto store_inst = dynamic_cast<StoreInst*>(inst);
            operands.insert(store_inst->getBase());
            if (store_inst->getOffset()) {
                operands.insert(store_inst->getOffset());
            }
            operands.insert(store_inst->getValue());
            break;
        }
        case MachineInstType::Cmp: {
            auto cmp_inst = dynamic_cast<CmpInst *>(inst);
            operands.insert(cmp_inst->getLhs());
            operands.insert(cmp_inst->getRhs());
            break;
        }
        case MachineInstType::Branch: {
            if (dynamic_cast<BranchInst *>(inst)->getBranchType() == BranchInst::Bl) {
                auto mc_module = inst->getParent()->getParent()->getModule();       // 获取Module
                assert(mc_module);
                for (auto i = 0; i < 4; ++i) {
                    MachineReg::Reg mreg = static_cast<MachineReg::Reg>(MachineReg::r0 + i);
                    auto mc_reg = mc_module->getMachineReg(mreg);
                    operands.insert(mc_reg);
                }
            }
            break;
        }
        default:
            break;
    }
    std::unordered_set<MachineOperand *> result;
    for (auto operand: operands) {
        if (operand->getOperandType() == MachineOperand::ImmNumber || operand->getOperandType() == MachineOperand::Label) {
            continue;
        }
        if ((!is_float && operand->getValueType() == MachineOperand::Int) || (is_float && operand->getValueType() == MachineOperand::Float)) {
            result.insert(operand);
        }
    }

    return result;
}

std::unordered_set<MachineOperand *> MachineInst::getUses(MachineInst *inst) {
    auto uses1 = getUses(inst, true);
    auto uses2 = getUses(inst, false);
    uses1.insert(uses2.begin(), uses2.end());
    return uses1;
}

// 不考虑非Virtual寄存器的话
std::unordered_set<MachineOperand *> MachineInst::getDefs(MachineInst *inst, bool is_float) {
    std::unordered_set<MachineOperand *> operands;
    switch (inst->machine_inst_type_) {
        case MachineInstType::Move:
            operands.insert(dynamic_cast<MoveInst*>(inst)->getDst());
            break;
        case MachineInstType::Vneg:
            operands.insert(dynamic_cast<VnegInst*>(inst)->getDst());
            break;
        case MachineInstType::Cvt:
            operands.insert(dynamic_cast<CvtInst*>(inst)->getDst());
            break;
        case MachineInstType::Load:
            operands.insert(dynamic_cast<LoadInst*>(inst)->getDst());
            break;
        case MachineInstType::Binary:
            operands.insert(dynamic_cast<BinaryInst*>(inst)->getDst());
            break;
        case MachineInstType::Clz:
            operands.insert(dynamic_cast<ClzInst*>(inst)->getDst());
            break;
        case MachineInstType::Branch:{
            if (dynamic_cast<BranchInst *>(inst)->getBranchType() == BranchInst::Bl) {
                auto mc_module = inst->getParent()->getParent()->getModule();       // 获取Module
                assert(mc_module);
                for (auto i = 0; i < 4; ++i) {
                    MachineReg::Reg mreg = static_cast<MachineReg::Reg>(MachineReg::r0 + i);
                    auto mc_reg = mc_module->getMachineReg(mreg);
                    operands.insert(mc_reg);
                }
                operands.insert(mc_module->getMachineReg(MachineReg::r12));
            }
            break;
        }
        default:
            break;
    }
    std::unordered_set<MachineOperand *> result;
    for (auto operand: operands){
        if (operand->getOperandType() == MachineOperand::ImmNumber || operand->getOperandType() == MachineOperand::Label) {
            continue;
        }
        if (((!is_float && operand->getValueType() == MachineOperand::Int) || (is_float && operand->getValueType() == MachineOperand::Float))) {
            result.insert(operand);
        }
    }

    return result;
}

std::unordered_set<MachineOperand *> MachineInst::getDefs(MachineInst *inst) {
    auto defs1 = getDefs(inst, true);
    auto defs2 = getDefs(inst, false);
    defs1.insert(defs2.begin(), defs2.end());
    return defs1;
}

void MachineInst::replaceDefs(MachineInst *inst, VirtualReg *old_operand, VirtualReg *new_operand) {
    assert(old_operand);
    assert(new_operand);
    auto defs = getDefs(inst);
    for (auto def: defs) {
        if (def == old_operand) {
            auto vreg = dynamic_cast<VirtualReg *>(def);
            assert(vreg);
            vreg->replaceWith(new_operand);
        }
    }
}

void MachineInst::replaceUses(MachineInst *inst, VirtualReg *old_operand, VirtualReg *new_operand) {
    assert(old_operand);
    assert(new_operand);
    auto uses = getUses(inst);
    for (auto use: uses) {
        if (use == old_operand) {
            auto vreg = dynamic_cast<VirtualReg *>(use);
            assert(vreg);
            vreg->replaceWith(new_operand);
        }
    }
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






