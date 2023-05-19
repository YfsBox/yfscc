//
// Created by 杨丰硕 on 2023/5/2.
//
#include <cassert>
#include "Machine.h"
#include "../common/Utils.h"

MachineModule::MachineModule(Module *irmodule):
    ir_module_(irmodule){
    for (int i = 0; i < 16; ++i) {
        MachineReg::Reg mc_reg = static_cast<MachineReg::Reg>(MachineReg::r0 + i);
        machine_regs_map_[mc_reg] = new MachineReg(mc_reg);
    }
    for (int i = 0; i < 32; ++i) {
        MachineReg::Reg mc_reg = static_cast<MachineReg::Reg>(MachineReg::s0 + i);
        machine_regs_map_[mc_reg] = new MachineReg(mc_reg);
    }
}

void MachineModule::addMachineFunction(MachineFunction *function) {
    machine_functions_.emplace_back(GET_UNIQUEPTR(function));
}

MachineBasicBlock::MachineBasicBlock(MachineFunction *function, const std::string &name):
    label_name_(name),
    parent_(function){

}

void MachineBasicBlock::addInstruction(MachineInst *inst) {
    assert(inst);
    instructions_.emplace_back(GET_UNIQUEPTR(inst));
}

void MachineBasicBlock::addFrontInstruction(MachineInst *inst) {
    assert(inst);
    instructions_.push_front(GET_UNIQUEPTR(inst));
}

void MachineBasicBlock::insertInstruction(MachineInstListIt it, MachineInst *inst) {
    ++it;
    assert(inst);
    instructions_.insert(it, GET_UNIQUEPTR(inst));
    --it;
}

void MachineBasicBlock::insertInstructionBefore(MachineInstListIt it, MachineInst *inst) {
    assert(inst);
    instructions_.insert(it, GET_UNIQUEPTR(inst));
}

MachineFunction::MachineFunction(MachineModule *module, const std::string &name):
    function_name_(name),
    module_(module){
}

void MachineFunction::addBasicBlock(MachineBasicBlock *basic_block) {
    basic_blocks_.emplace_back(GET_UNIQUEPTR(basic_block));
}



