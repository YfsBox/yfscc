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

static inline int32_t getHigh(int32_t value) {
    return (value >> 16) & 0xffff;
}

static inline int32_t getLow(int32_t value) {
    return value & 0xffff;
}

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

bool CodeGen::isImmNeedSplitMove(int32_t imm_value) {
    int32_t v = imm_value;
    for (int r = 0; r < 31; r += 2) {
        if ((v & 0xff) == v) {
            return true;
        }
        v = (v >> 2) | (v << 30);       // 循环右移2位
    }
    return false;
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
        // TODO,目前还不确定具体应该push或者pop哪些寄存器
        if (basic_block->getPreDecessorBlocks().empty()) {
            curr_machine_function_->setEnterBasicBlock(curr_machine_basic_block_);
            auto push_inst = new PushInst(curr_machine_basic_block_);
            curr_machine_basic_block_->addFrontInstruction(push_inst);
        }

        if (basic_block->getSuccessorBlocks().empty()) {
            curr_machine_function_->addExitBasicBlock(curr_machine_basic_block_);
            auto pop_inst = new PopInst(curr_machine_basic_block_);
            curr_machine_basic_block_->addInstruction(pop_inst);
        }
    }
}

void CodeGen::visit(GEPInstruction *inst) {

}

void CodeGen::visit(RetInstruction *inst) {
    RetInst *ret_inst = new RetInst(curr_machine_basic_block_);
    if (!inst->isRetVoid()) {
        bool is_float = false;
        MachineOperand *dst_operand = nullptr;
        MoveInst *mov_inst = nullptr;
        auto operand = value2MachineOperand(inst->getRetValue(), &is_float);
        dst_operand = createVirtualReg(is_float ? MachineOperand::Float : MachineOperand::Int);
        mov_inst = new MoveInst(curr_machine_basic_block_, is_float ? MoveInst::F2F : MoveInst::I2I, operand, dst_operand);
        addMachineInst(mov_inst);
    }
    addMachineInst(ret_inst);
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

MachineOperand *CodeGen::value2MachineOperand(Value *value, bool *is_float) {
    auto find_value = value_machinereg_map_.find(value);
    if (find_value != value_machinereg_map_.end()) {
        *is_float = find_value->second->getValueType() == MachineOperand::Float;
        return find_value->second;
    }

    MachineOperand *ret_operand = nullptr;
    auto value_type = value->getValueType();
    if (value_type == ValueType::GlobalVariableValue) {
        auto load_global_mov = loadGlobalVarAddr(dynamic_cast<GlobalVariable *>(value));
        assert(load_global_mov);
        addMachineInst(load_global_mov);
        ret_operand = load_global_mov->getDst();
    } else if (value_type == ValueType::ConstantValue) {
        auto const_value = dynamic_cast<ConstantVar *>(value);
        assert(const_value);
        int32_t const_value_number;
        if (const_value->isFloat()) {
            const_value_number = getFloat2IntForm(const_value->getFValue());
        } else {
            const_value_number = const_value->getIValue();
        }

        auto dst_vreg = createVirtualReg(MachineOperand::ValueType::Int);
        if (isImmNeedSplitMove(const_value_number)) {
            int32_t high_value = getHigh(const_value_number);
            int32_t low_value = getLow(const_value_number);

            auto h_imm = new ImmNumber(high_value);
            auto l_imm = new ImmNumber(low_value);

            auto movh_inst = new MoveInst(curr_machine_basic_block_, MoveInst::H2I, h_imm, dst_vreg);
            auto movl_inst = new MoveInst(curr_machine_basic_block_, MoveInst::L2I, l_imm, dst_vreg);

            addMachineInst(movh_inst);
            addMachineInst(movl_inst);

            ret_operand = dst_vreg;
        } else {
            auto imm = new ImmNumber(const_value_number);
            auto mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, imm, dst_vreg);
            addMachineInst(mov_inst);

            ret_operand = dst_vreg;
        }

        if (const_value->isFloat()) {       // 因为前面的ret_operand是Int类型的,所以还需要涉及到一步到float的转换操作
            auto src = ret_operand;
            auto new_vreg = createVirtualReg(MachineOperand::Int);
            auto mov_i2i_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, src, new_vreg);
            auto vdst = createVirtualReg(MachineOperand::Float);
            auto mov_i2f_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F_I, new_vreg, vdst);

            addMachineInst(mov_i2i_inst);
            addMachineInst(mov_i2f_inst);

            *is_float = true;
        }
    }

    return ret_operand;
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

