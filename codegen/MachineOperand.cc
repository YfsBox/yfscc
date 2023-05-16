//
// Created by 杨丰硕 on 2023/5/2.
//
#include "MachineOperand.h"

MachineOperand::MachineOperand(ValueType value_type, OperandType operand_type):
    value_type_(value_type),
    operand_type_(operand_type){

}

MachineOperand::~MachineOperand() = default;

ImmNumber::ImmNumber(int32_t value):
    MachineOperand(MachineOperand::Int, OperandType::ImmNumber),
    is_float_(false),
    ivalue_(value){

}

ImmNumber::ImmNumber(float value):
    MachineOperand(MachineOperand::Float, OperandType::ImmNumber),
    is_float_(true),
    fvalue_(value){

}

ImmNumber::~ImmNumber() = default;

VirtualReg::VirtualReg(int reg_id, ValueType value_type):
    MachineOperand(value_type, OperandType::VirtualReg),
    reg_id_(reg_id),
    is_colored_(false),
    colored_mcreg_(MachineReg::Reg::undef){

}

void VirtualReg::replaceWith(VirtualReg *vreg) {
    this->reg_id_ = vreg->reg_id_;
    this->is_colored_ = vreg->is_colored_;
    this->colored_mcreg_ = vreg->colored_mcreg_;
}

Label::Label(const std::string &name):
    MachineOperand(Undef, OperandType::Label),
    name_(name){

}

Label::~Label() = default;

MachineReg::MachineReg(Reg reg):
    MachineOperand(Undef, OperandType::MachineReg),
    reg_(reg){
    if (reg_ >= r0 && reg_ <= r15) {
        MachineOperand::value_type_ = Int;
    } else if (reg_ >= s0 && reg_ <= s31) {
        MachineOperand::value_type_ = Float;
    }
}

std::string MachineReg::machineReg2RegName(MachineReg::Reg reg) {
    if (reg >= r0 && reg <= r10) {
        return "r" + std::to_string(reg - r0);
    } else if (reg >= s0 && reg <= s31) {
        return "s" + std::to_string(reg - s0);
    }
    switch (reg) {
        case fp:
            return "fp";
        case ip:
            return "ip";
        case sp:
            return "sp";
        case lr:
            return "lr";
        case pc:
            return "pc";
    }
    return "";
}

std::string MachineReg::machieReg2RegName() const {
    return machineReg2RegName(reg_);
}

MachineReg::~MachineReg() = default;





