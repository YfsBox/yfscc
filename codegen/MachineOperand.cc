//
// Created by 杨丰硕 on 2023/5/2.
//
#include "MachineOperand.h"

MachineOperand::MachineOperand(OperandType operand_type):
    operand_type_(operand_type){

}

MachineOperand::~MachineOperand() = default;

ImmNumber::ImmNumber(int32_t value):
    MachineOperand(MachineOperand::Int),
    is_float_(false),
    ivalue_(value){

}

ImmNumber::ImmNumber(float value):
    MachineOperand(MachineOperand::Float),
    is_float_(true),
    fvalue_(value){

}

std::string ImmNumber::dump() {
    return "";
}

ImmNumber::~ImmNumber() = default;

VirtualReg::VirtualReg(int reg_id, OperandType operand):
    MachineOperand(operand),
    reg_id_(reg_id){

}

std::string VirtualReg::dump() {
    return "";
}

VirtualReg::~VirtualReg() = default;

Label::Label(const std::string &name):
    MachineOperand(Undef),
    name_(name){

}

std::string Label::dump() {
    return "";
}

Label::~Label() = default;

MachineReg::MachineReg(Reg reg):
    MachineOperand(Undef),
    reg_(reg){
    if (reg_ >= r0 && reg_ <= r15) {
        MachineOperand::operand_type_ = Int;
    } else if (reg_ >= s0 && reg_ <= s31) {
        MachineOperand::operand_type_ = Float;
    }
}

std::string MachineReg::dump() {
    return "";
}

MachineReg::~MachineReg() = default;





