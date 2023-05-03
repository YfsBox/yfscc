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

VirtualReg *CodeGen::createVirtualReg(MachineOperand::ValueType value_type) {
    virtual_reg_id_++;
    VirtualReg *virtual_reg = new VirtualReg(virtual_reg_id_, value_type);
    virtual_reg_map_[virtual_reg_id_] = virtual_reg;
    return virtual_reg;
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
    auto inst_type = inst->getInstType();
    if (inst_type >= AddType && inst_type <= XorType) {
        visit(dynamic_cast<BinaryOpInstruction *>(inst));
        return;
    }
    if (inst_type == NegType || inst_type == NotType) {
        visit(dynamic_cast<UnaryOpInstruction *>(inst));
        return;
    }

    switch (inst_type) {
        case RetType:
            visit(dynamic_cast<RetInstruction *>(inst));
            return;
        case BrType:
            visit(dynamic_cast<BranchInstruction *>(inst));
            return;
        case AllocaType:
            visit(dynamic_cast<AllocaInstruction *>(inst));
            return;
        case LoadType:
            visit(dynamic_cast<LoadInstruction *>(inst));
            return;
        case StoreType:
            visit(dynamic_cast<StoreInstruction *>(inst));
            return;
        case CallType:
            visit(dynamic_cast<CallInstruction *>(inst));
            return;
        case SetCondType:
            visit(dynamic_cast<SetCondInstruction *>(inst));
            return;
        case PhiType:
            visit(dynamic_cast<PhiInstruction *>(inst));
            return;
        case CastType:
            visit(dynamic_cast<CastInstruction *>(inst));
            return;
        case GEPType:
            visit(dynamic_cast<GEPInstruction *>(inst));
            return;
        case ZextType:
            visit(dynamic_cast<ZextInstruction *>(inst));
            return;
    }

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

MoveInst *CodeGen::loadGlobalVarAddr(GlobalVariable *global) {
    MoveInst *move_inst = nullptr;
    auto addr_reg = createVirtualReg(MachineOperand::Int);
    auto find_label = global_var_map_.find(global->getName());
    if (find_label != global_var_map_.end()) {
        auto label = find_label->second.get();
        move_inst = new MoveInst(curr_machine_basic_block_, MoveInst::MoveType::I2I, label, addr_reg);
    }
    return move_inst;
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

