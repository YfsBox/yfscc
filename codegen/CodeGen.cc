//
// Created by 杨丰硕 on 2023/5/2.
//
#include "CodeGen.h"
#include "MachineOperand.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "../ir/GlobalVariable.h"
#include "../common/Utils.h"

CodeGen::CodeGen(Module *ir_module):
    virtual_reg_id_(-1),
    module_(std::make_unique<MachineModule>(ir_module)),
    curr_machine_basic_block_(nullptr),
    curr_machine_function_(nullptr){

}

void CodeGen::visit(Module *module) {
    auto global_size = module->getGlobalSize();
    for (int i = 0; i < global_size; ++i) {
        visit(module->getGlobalVariable(i));
    }

    auto function_size = module->getFuncSize();
    for (int i = 0; i < function_size; ++i) {
        visit(module->getFunction(i));
        module_->addMachineFunction(curr_machine_function_);
    }
}

void CodeGen::visit(BasicBlock *block) {
    for (auto &inst: block->getInstructionList()) {
        visit(inst.get());
    }
}

void CodeGen::visit(Instruction *inst) {

}

void CodeGen::visit(Constant *constant) {

}

void CodeGen::visit(Function *function) {
    curr_machine_function_ = new MachineFunction(module_.get());
    for (auto &basic_block: function->getBlocks()) {
        visit(basic_block.get());
        curr_machine_function_->addBasicBlock(curr_machine_basic_block_);
    }
}

void CodeGen::visit(GEPInstruction *inst) {

}

void CodeGen::visit(RetInstruction *inst) {

}

void CodeGen::visit(CallInstruction *inst) {

}

void CodeGen::visit(CastInstruction *inst) {

}

void CodeGen::visit(LoadInstruction *inst) {

}

void CodeGen::visit(ZextInstruction *inst) {

}

void CodeGen::visit(GlobalVariable *global) {
    auto label = new Label(global->getName());
    global_var_map_[label->getName()] = GET_UNIQUEPTR(label);

}

void CodeGen::visit(StoreInstruction *inst) {

}

void CodeGen::visit(AllocaInstruction *inst) {

}

void CodeGen::visit(BranchInstruction *inst) {

}

void CodeGen::visit(MemSetInstruction *inst) {

}

void CodeGen::visit(SetCondInstruction *inst) {

}

void CodeGen::visit(UnaryOpInstruction *uinst) {

}

void CodeGen::visit(BinaryOpInstruction *binst) {

}

