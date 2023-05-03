//
// Created by 杨丰硕 on 2023/5/2.
//
#include "Machine.h"
#include "../common/Utils.h"

MachineModule::MachineModule(Module *irmodule):
    ir_module_(irmodule){

}

void MachineModule::addMachineFunction(MachineFunction *function) {
    machine_functions_.push_back(GET_UNIQUEPTR(function));
}

MachineBasicBlock::MachineBasicBlock(MachineFunction *function):
    parent_(function){

}

void MachineBasicBlock::addInstruction(MachineInst *inst) {
    instructions_.push_back(GET_UNIQUEPTR(inst));
}

void MachineBasicBlock::addFrontInstruction(MachineInst *inst) {
    instructions_.push_front(GET_UNIQUEPTR(inst));
}

MachineFunction::MachineFunction(MachineModule *module):
    module_(module){

}

void MachineFunction::addBasicBlock(MachineBasicBlock *basic_block) {
    basic_blocks_.push_back(GET_UNIQUEPTR(basic_block));
}



