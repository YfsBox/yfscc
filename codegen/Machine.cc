//
// Created by 杨丰硕 on 2023/5/2.
//
#include "Machine.h"

MachineModule::MachineModule(Module *irmodule):
    ir_module_(irmodule){

}

MachineBasicBlock::MachineBasicBlock(MachineFunction *function):
    parent_(function){

}

MachineFunction::MachineFunction(MachineModule *module):
    module_(module){

}



