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
    reg_id_(reg_id){

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

MachineReg::~MachineReg() = default;





