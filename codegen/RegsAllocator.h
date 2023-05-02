//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_REGSALLOCATOR_H
#define YFSCC_REGSALLOCATOR_H

#include "Machine.h"

MachineModule::MachineModule(Module *irmodule):
    ir_module_(irmodule){

}

std::string MachineModule::dump() {
    return "";
}

MachineBasicBlock::MachineBasicBlock(MachineFunction *function):
    parent_(function){

}

std::string MachineBasicBlock::dump() {
    return "";
}

MachineFunction::MachineFunction(MachineModule *module):
    module_(module){

}

std::string MachineFunction::dump() {
    return "";
}



#endif //YFSCC_REGSALLOCATOR_H
