//
// Created by 杨丰硕 on 2023/5/2.
//
#include "CodeGen.h"
#include "MachineOperand.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "../ir/GlobalVariable.h"
#include "../ir/Instruction.h"
#include "../common/Utils.h"

static inline int32_t getHigh(int32_t value) {
    return (value >> 16) & 0xffff;
}

static inline int32_t getLow(int32_t value) {
    return value & 0xffff;
}

static inline MachineOperand::ValueType basicType2ValueType(BasicType basic_type) {
    return basic_type == BasicType::INT_BTYPE ? MachineOperand::Int : MachineOperand::Float;
}
// 6000, 600, 30, 1
// array[10][20][30] -> array[5][1][20],
static void getDimensionBasesSize(const std::vector<int32_t> &dimension_number, std::vector<int32_t> &dimension_bases) {
    auto dimension_size = dimension_number.size();
    dimension_bases.resize(dimension_size + 1);
    int32_t product = 1;
    for (int i = 0; i < dimension_size; ++i) {
        dimension_bases[dimension_size - i] = product;
        product *= dimension_number[dimension_size - i - 1];
    }
    dimension_bases[0] = product;
}

CodeGen::CodeGen(Module *ir_module):
    virtual_reg_id_(-1),
    stack_offset_(0),
    sp_reg_(nullptr),
    fp_reg_(nullptr),
    lr_reg_(nullptr),
    module_(std::make_unique<MachineModule>(ir_module)),
    curr_machine_basic_block_(nullptr),
    curr_machine_function_(nullptr),
    curr_machine_operand_(nullptr){

    sp_reg_ = module_->getMachineReg(MachineReg::sp);
    fp_reg_ = module_->getMachineReg(MachineReg::fp);
    lr_reg_ = module_->getMachineReg(MachineReg::lr);
}

VirtualReg *CodeGen::createVirtualReg(MachineOperand::ValueType value_type, Value *value) {
    virtual_reg_id_++;
    VirtualReg *virtual_reg = new VirtualReg(virtual_reg_id_, value_type);
    curr_machine_function_->addVirtualReg(virtual_reg);
    if (value) {
        value_machinereg_map_[value] = virtual_reg;
    }
    return virtual_reg;
}

VirtualReg *CodeGen::createVirtualReg(MachineFunction *function, MachineOperand::ValueType value_type) {
    auto vreg_no = function->getVirtualRegs().size();
    VirtualReg *virtual_reg = new VirtualReg(vreg_no, value_type);
    function->addVirtualReg(virtual_reg);
    return virtual_reg;
}

MachineOperand *CodeGen::constant2VirtualReg(int32_t const_value, bool can_be_imm, MachineBasicBlock *basicblock, std::vector<MachineInst *> *move_insts) {
    MachineOperand *dst_reg;
    int32_t high_value = getHigh(const_value);
    if (high_value != 0) {
        dst_reg = createVirtualReg(MachineOperand::Int);
        int32_t low_value = getLow(const_value);

        auto h_imm = new ImmNumber(high_value);
        auto l_imm = new ImmNumber(low_value);

        auto movh_inst = new MoveInst(basicblock, MoveInst::H2I, h_imm, dst_reg);
        auto movl_inst = new MoveInst(basicblock, MoveInst::L2I, l_imm, dst_reg);

        if (!move_insts) {
            addMachineInst(movl_inst);
            addMachineInst(movh_inst);
        } else {
            move_insts->push_back(movl_inst);
            move_insts->push_back(movh_inst);
        }
    } else {
        auto imm = new ImmNumber(const_value);
        dst_reg = imm;
        if (!can_be_imm) {
            dst_reg = createVirtualReg(MachineOperand::Int);
            auto mov_inst = new MoveInst(basicblock, MoveInst::I2I, imm, dst_reg);
            if (!move_insts) {
                addMachineInst(mov_inst);
            } else {
                move_insts->push_back(mov_inst);
            }
        }
    }
    return dst_reg;
}

bool CodeGen::isImmNeedSplitMove(int imm_value) {
    int v = imm_value;
    for (int r = 0; r < 31; r += 2) {
        if ((v & 0xff) == v) {
            return true;
        }
        v = (v >> 2) | (v << 30);       // 循环右移2位
    }
    return false;
}

void CodeGen::addPushInst(MachineBasicBlock *basicblock, std::unordered_set<MachineReg::Reg> *regs) {
    auto push_regs_inst = new PushInst(curr_machine_basic_block_, MachineInst::Int);
    auto push_sreg_inst = new PushInst(curr_machine_basic_block_, MachineInst::Float);

    for (int i = 0; i < 8; ++i) {
        auto machine_reg = getMachineReg(false, 4 + i);
        auto machine_sreg = getMachineReg(true, 16 + i);

        if ((regs && regs->count(machine_reg->getReg())) || !regs) {
            push_regs_inst->addReg(machine_reg);
        }
        if ((regs && regs->count(machine_sreg->getReg())) || !regs) {
            push_sreg_inst->addReg(machine_sreg);
        }
    }

    for (int i = 0; i < 8; ++i) {
        auto machine_sreg = getMachineReg(true, 24 + i);
        if ((regs && regs->count(machine_sreg->getReg())) || !regs) {
            push_sreg_inst->addReg(machine_sreg);
        }
    }

    push_regs_inst->addReg(module_->getMachineReg(MachineReg::lr));
    push_sreg_inst->setValueType(MachineInst::Float);

    if (push_regs_inst->getRegsSize()) {
        basicblock->addFrontInstruction(push_regs_inst);
    }
    if (push_sreg_inst->getRegsSize()) {
        basicblock->addFrontInstruction(push_sreg_inst);
    }
}

void CodeGen::addPopInst(MachineBasicBlock *basicblock, std::unordered_set<MachineReg::Reg> *regs) {
    auto pop_regs_inst = new PopInst(curr_machine_basic_block_, MachineInst::Int);
    auto pop_sreg_inst = new PopInst(curr_machine_basic_block_, MachineInst::Float);

    for (int i = 0; i < 8; ++i) {
        auto machine_reg = getMachineReg(false, 4 + i);
        auto machine_sreg = getMachineReg(true, 16 + i);

        if ((regs && regs->count(machine_reg->getReg())) || !regs) {
            pop_regs_inst->addReg(machine_reg);
        }
        if ((regs && regs->count(machine_sreg->getReg())) || !regs) {
            pop_sreg_inst->addReg(machine_sreg);
        }
    }

    for (int i = 0; i < 8; ++i) {
        auto machine_sreg = getMachineReg(true, 24 + i);
        if ((regs && regs->count(machine_sreg->getReg())) || !regs) {
            pop_sreg_inst->addReg(machine_sreg);
        }
    }

    pop_regs_inst->addReg(module_->getMachineReg(MachineReg::lr));
    pop_sreg_inst->setValueType(MachineInst::Float);

    if (pop_regs_inst->getRegsSize()) {
        basicblock->addInstruction(pop_regs_inst);
    }
    if (pop_sreg_inst->getRegsSize()) {
        basicblock->addInstruction(pop_sreg_inst);
    }
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
    curr_machine_basic_block_ = new MachineBasicBlock(curr_machine_function_, curr_machine_function_->getFunctionName() + "." + block->getName());
    curr_machine_basic_block_->setLoopDepth(block->getWhileLoopDepth());
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
        default:
            return;
    }

}

void CodeGen::visit(Constant *constant) {
    bool imm_float = false;
    auto const_reg = value2MachineOperand(constant, true);
    setCurrMachineOperand(const_reg);
}

MachineOperand *CodeGen::getImmOperandInBinary(int32_t value, MachineBasicBlock *bb, std::vector<MachineInst *> *moves, bool is_float, bool *use_ip_base, bool is_simple_ralloc) {
    MachineOperand *result = nullptr;
    if (canImmInBinary(value)) {
        result = new ImmNumber(value);
    } else {
        if (is_simple_ralloc) {
            result= module_->getMachineReg(MachineReg::lr);
        } else {
            result= module_->getMachineReg(MachineReg::ip);
        }

        auto mov1_inst = new MoveInst(bb, MoveInst::L2I, new ImmNumber(getLow(value)), result);
        auto mov2_inst = new MoveInst(bb, MoveInst::H2I, new ImmNumber(getHigh(value)), result);
        if (moves) {
            moves->push_back(mov1_inst);        //
            moves->push_back(mov2_inst);
        } else {
            bb->addInstruction(mov1_inst);
            bb->addInstruction(mov2_inst);
        }

        if (is_float) {
            auto add_base_inst = new BinaryInst(bb, BinaryInst::IAdd, result, fp_reg_, result);
            *use_ip_base = true;
            if (moves) {
                moves->push_back(add_base_inst);
            } else {
                bb->addInstruction(add_base_inst);
            }
        }

    }
    return result;
}

void CodeGen::visit(Function *function) {
    curr_machine_function_ = new MachineFunction(module_.get(), function->getName());
    virtual_reg_id_ = -1;
    stack_offset_ = 0;
    curr_used_globals_.clear();
    phi2vreg_map_.clear();

    int32_t old_stack_offset = stack_offset_;

    int32_t int_arg_reg_cnt = 0, float_arg_reg_cnt = 0;
    std::vector<MachineInst *> args_move_insts;
    std::vector<MachineInst *> args_load_insts;

    std::vector<MachineOperand *> args_onstack_vregs;

    std::vector<MachineInst *> init_global_insts;
    initForGlobals(function, init_global_insts);

    for (int i = 0; i < function->getArgumentSize(); ++i) {
        auto arg = function->getArgument(i);
        MachineOperand *vreg;
        if (arg->getBasicType() == BasicType::FLOAT_BTYPE && !arg->isPtrArg()) {
            vreg = createVirtualReg(MachineOperand::Float, arg);
            // printf("the arg %d in function %s to vreg %d\n", i, function->getName().c_str(), dynamic_cast<VirtualReg *>(vreg)->getRegId());
            if (float_arg_reg_cnt < 16) {
                auto mov_inst = new MoveInst(nullptr, MoveInst::MoveType::F2F, getMachineReg(true, float_arg_reg_cnt), vreg);
                args_move_insts.push_back(mov_inst);
                float_arg_reg_cnt++;
            } else {
                args_onstack_vregs.push_back(vreg);
            }
        } else {
            vreg = createVirtualReg(MachineOperand::Int, arg);
            // printf("the arg %d in function %s to vreg %d\n", i, function->getName().c_str(), dynamic_cast<VirtualReg *>(vreg)->getRegId());
            if (int_arg_reg_cnt < 4) {
                auto mov_inst = new MoveInst(nullptr, MoveInst::MoveType::I2I, getMachineReg(false, int_arg_reg_cnt), vreg);
                args_move_insts.push_back(mov_inst);
                int_arg_reg_cnt++;
            } else {
                args_onstack_vregs.push_back(vreg);
            }
        }
    }

    auto args_onstack_cnt = args_onstack_vregs.size();
    for (int i = args_onstack_cnt - 1; i >= 0; --i) {
        auto vreg = args_onstack_vregs[i];
        LoadInst *load_inst;
        int32_t offset_cnt = args_onstack_cnt - i - 1;
        load_inst = new LoadInst(nullptr, vreg, fp_reg_, new ImmNumber(push_regs_offset_ + 4 * offset_cnt));
        curr_machine_function_->addLoadArgsInst(load_inst);
        args_load_insts.push_back(load_inst);
    }

    for (auto &basic_block: function->getBlocks()) {
        visit(basic_block.get());
        curr_machine_function_->addBasicBlock(curr_machine_basic_block_);
        module_->addBasicBlockPair(curr_machine_basic_block_, basic_block.get());
        // printf("curr basicblock is %s\n", basic_block->getName().c_str());
        if (basic_block->isEnterBasicBlock()) {
            curr_machine_function_->setEnterBasicBlock(curr_machine_basic_block_);
            // printf("set basic block %s as enterblock\n", curr_machine_basic_block_->getLabelName().c_str());
        }

        if (basic_block->getSuccessorBlocks().empty()) {
            curr_machine_function_->addExitBasicBlock(curr_machine_basic_block_);
        }
    }
    // 一个函数在进入和退出时所需要的额外指令
    int32_t mov_stack_offset = stack_offset_ - old_stack_offset;
    MachineBasicBlock *enter_basicblock = curr_machine_function_->getEnterBasicBlock();

    for (auto load_inst: args_load_insts) {
        load_inst->setParent(enter_basicblock);
        enter_basicblock->addFrontInstruction(load_inst);
    }
    for (auto mov_inst: args_move_insts) {
        mov_inst->setParent(enter_basicblock);
        enter_basicblock->addFrontInstruction(mov_inst);
    }
    for (auto rit = init_global_insts.rbegin(); rit != init_global_insts.rend(); ++rit) {
        auto inst = *rit;
        inst->setParent(enter_basicblock);
        enter_basicblock->addFrontInstruction(inst);
    }

    curr_machine_function_->setStackSize(stack_offset_);
    addMoveForPhiInst();
}

void CodeGen::addInstAboutStack(MachineFunction *function, int32_t offset, std::unordered_set<MachineReg::Reg> *regs) {
    std::vector<MachineInst *> moves;
    auto enter_block = function->getEnterBasicBlock();
    enter_block->addFrontInstruction(new BinaryInst(enter_block, BinaryInst::ISub, sp_reg_, sp_reg_, getImmOperandInBinary(offset, enter_block, &moves)));
    assert(moves.size() == 2 || moves.empty());
    if (!moves.empty()) {
        enter_block->addFrontInstruction(moves[1]);
        enter_block->addFrontInstruction(moves[0]);        // 低
    }
    enter_block->addFrontInstruction(new MoveInst(enter_block, sp_reg_, fp_reg_));
    addPushInst(enter_block, regs);
    for (int i = 0; i < function->getExitBasicBlockSize(); ++i) {
        auto exit_basicblock = function->getExitBasicBlock(i);
        exit_basicblock->addInstruction(new BinaryInst(exit_basicblock, BinaryInst::IAdd, sp_reg_, sp_reg_, getImmOperandInBinary(offset, exit_basicblock)));
        exit_basicblock->addInstruction(new MoveInst(exit_basicblock, fp_reg_, sp_reg_));
        addPopInst(exit_basicblock, regs);
        auto bx_inst = new BranchInst(exit_basicblock, lr_reg_, BranchInst::BrNoCond, BranchInst::Bx);
        exit_basicblock->addInstruction(bx_inst);
    }
}

void CodeGen::addRetBranchInExitBlocks(MachineFunction *function) {

}


void CodeGen::visit(GEPInstruction *inst) {
    auto base_ptr = inst->getPtr();
    auto base_ptr_reg = value2MachineOperand(base_ptr, false);
    assert(base_ptr_reg);
    auto dst_base_reg = createVirtualReg(MachineOperand::Int, inst);
    auto mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, base_ptr_reg, dst_base_reg);
    addMachineInst(mov_inst);

    int index_size = inst->getIndexSize();
    const std::vector<int32_t> &array_dimension = inst->getArrayDimension();
    std::vector<int32_t> array_base_offsets;
    getDimensionBasesSize(array_dimension, array_base_offsets);

    for (int i = 0; i < index_size; ++i) {
        auto index = inst->getIndexValue(i);
        int32_t addbase_offset = array_base_offsets[i];
        MachineOperand *tmp_mul_dst = nullptr;
        if (auto const_index = dynamic_cast<ConstantVar *>(index); const_index) {
            int32_t const_index_number = const_index->getIValue();
            if (const_index_number == 0) {
                continue;
            }
            auto base_add_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IAdd, dst_base_reg, dst_base_reg,
                                                getImmOperandInBinary(addbase_offset * const_index_number * 4, curr_machine_basic_block_));
            addMachineInst(base_add_inst);
        } else {
            auto const_index_reg = value2MachineOperand(index, false);
            assert(base_ptr_reg);
            if (tmp_mul_dst == nullptr) {
                tmp_mul_dst = createVirtualReg(MachineOperand::Int);
            }
            auto mov_to_mul_dst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, constant2VirtualReg(addbase_offset * 4, true, curr_machine_basic_block_), tmp_mul_dst);
            addMachineInst(mov_to_mul_dst);

            auto mul_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IMul, tmp_mul_dst, tmp_mul_dst, const_index_reg);
            addMachineInst(mul_inst);

            auto base_add_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IAdd, dst_base_reg, dst_base_reg, tmp_mul_dst);
            addMachineInst(base_add_inst);
        }
    }

}

void CodeGen::visit(RetInstruction *inst) {
    RetInst *ret_inst = new RetInst(curr_machine_basic_block_);
    if (!inst->isRetVoid()) {
        bool is_float = false;
        MachineOperand *dst_operand = nullptr;
        MoveInst *mov_inst = nullptr;
        auto operand = value2MachineOperand(inst->getRetValue(), false, &is_float);
        assert(operand);
        dst_operand = is_float ? getMachineReg(true, 0) : getMachineReg(false, 0);
        mov_inst = new MoveInst(curr_machine_basic_block_, is_float ? MoveInst::F2F : MoveInst::I2I, operand, dst_operand);
        addMachineInst(mov_inst);
        setCurrMachineOperand(dst_operand);
    }
    addMachineInst(ret_inst);
}

void CodeGen::visit(CallInstruction *inst) {
    int32_t int_args_cnt = 0, float_args_cnt = 0;
    Function *function = inst->getFunction();
    for (int i = 0; i < function->getArgumentSize(); ++i) {
        auto arg = function->getArgument(i);
        if (!arg->isPtrArg() && arg->getBasicType() == BasicType::FLOAT_BTYPE) {
            float_args_cnt++;
        } else {
            int_args_cnt++;
        }
    }
    int32_t stack_offset = 0;
    if (int_args_cnt > 4) {
        stack_offset += int_args_cnt - 4;
    }
    if (float_args_cnt > 16) {
        stack_offset += float_args_cnt - 16;
    }
    stack_offset *= 4;

    bool align_offset = false;
    // 考虑内存对齐，对齐到8
    if (stack_offset % 8) {
        // stack_offset += 4;
        align_offset = true;
        addMachineInst(new BinaryInst(curr_machine_basic_block_, BinaryInst::ISub, sp_reg_, sp_reg_, new ImmNumber(4)));
    }
    // MachineOperand *stack_offset_imm = nullptr;
    int32_t saved_stack_offset = stack_offset;
    if (stack_offset != 0) {
        // stack_offset_imm = new ImmNumber(stack_offset);
        auto sub_stack_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::ISub, sp_reg_, sp_reg_,
                                             getImmOperandInBinary(stack_offset, curr_machine_basic_block_));
        addMachineInst(sub_stack_inst);
    }

    int32_t old_stack_offset = stack_offset;
    int32_t curr_float_arg_cnt = 0, curr_int_arg_cnt = 0;
    for (int i = 0; i < function->getArgumentSize(); ++i) {
        auto arg = function->getArgument(i);
        auto actual = inst->getActual(i);
        auto actual_vreg = value2MachineOperand(actual, true);
        assert(actual_vreg);
        if (!arg->isPtrArg() && arg->getBasicType() == BasicType::FLOAT_BTYPE) {
            if (curr_float_arg_cnt < 16) {
                auto dst_vreg = getMachineReg(true, curr_float_arg_cnt);
                auto mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F2F, actual_vreg, dst_vreg);
                addMachineInst(mov_inst);
            } else {
                auto tmp_dst_reg = getMachineReg(true, 16);
                auto tmp_mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F2F, actual_vreg, tmp_dst_reg);
                auto store_inst = new StoreInst(MemIndexType::PostiveIndex, curr_machine_basic_block_, tmp_dst_reg, sp_reg_, new ImmNumber(stack_offset - 4));

                // printf("the arg %d in function %s on offset %d\n", i, inst->getFunction()->getName().c_str(), stack_offset - 4);

                addMachineInst(tmp_mov_inst);
                addMachineInst(store_inst);
                stack_offset -= 4;
            }
            curr_float_arg_cnt++;
        } else {
            if (curr_int_arg_cnt < 4) {
                auto dst_vreg = getMachineReg(false, curr_int_arg_cnt);
                auto mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, actual_vreg, dst_vreg);
                addMachineInst(mov_inst);
            } else {
                auto tmp_dst_reg = getMachineReg(false, 4);
                auto tmp_mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, actual_vreg, tmp_dst_reg);
                auto store_inst = new StoreInst(MemIndexType::PostiveIndex, curr_machine_basic_block_, tmp_dst_reg, sp_reg_, new ImmNumber(stack_offset - 4));

                // printf("the arg %d in function %s on offset %d\n", i, inst->getFunction()->getName().c_str(), stack_offset - 4);
                addMachineInst(tmp_mov_inst);
                addMachineInst(store_inst);
                stack_offset -= 4;
            }
            curr_int_arg_cnt++;
        }
    }

    auto branch_funciton_inst = new BranchInst(curr_machine_basic_block_, new Label(function->getName()), BranchInst::BrNoCond, BranchInst::BranchType::Bl);
    addMachineInst(branch_funciton_inst);

    if (saved_stack_offset) {
        auto add_stack_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IAdd, sp_reg_, sp_reg_,
                                             getImmOperandInBinary(saved_stack_offset, curr_machine_basic_block_));
        addMachineInst(add_stack_inst);     // 恢复stack
    }

    if (align_offset) {
        addMachineInst(new BinaryInst(curr_machine_basic_block_, BinaryInst::IAdd, sp_reg_, sp_reg_, new ImmNumber(4)));
    }

    // 返回值保存在r0之中，需要增加一个move语句，将r0保存到某个寄存器里
    MachineReg *ret_reg;
    MoveInst *mov_inst = nullptr;
    auto ret_type = function->getRetType();
    if (ret_type == BasicType::FLOAT_BTYPE) {
        ret_reg = getMachineReg(true, 0);
        auto dst_reg = createVirtualReg(MachineOperand::Float, inst);
        mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F2F, ret_reg, dst_reg);
    } else if (ret_type == BasicType::INT_BTYPE) {
        ret_reg = getMachineReg(false, 0);
        auto dst_reg = createVirtualReg(MachineOperand::Int, inst);
        mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, ret_reg, dst_reg);
    }
    if (mov_inst) {
        addMachineInst(mov_inst);
    }
}

void CodeGen::visit(CastInstruction *inst) {
    auto src_vreg = value2MachineOperand(inst->getValue(), false);
    MachineInst *vmov_inst, *cvt_inst;
    MachineOperand *vmov_dst_vreg, *cvt_dst_vreg;
    if (inst->isI2F()) {
        vmov_dst_vreg = createVirtualReg(MachineOperand::Float);
        vmov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F_I, src_vreg, vmov_dst_vreg);
        addMachineInst(vmov_inst);

        cvt_dst_vreg = createVirtualReg(MachineOperand::Float, inst);
        cvt_inst = new CvtInst(curr_machine_basic_block_, CvtInst::I2F, cvt_dst_vreg, vmov_dst_vreg);

        addMachineInst(cvt_inst);
    } else {
        if (src_vreg->getValueType() == MachineOperand::Int) {
            auto vmov_src_vreg = createVirtualReg(MachineOperand::Float);
            auto src_vmov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F_I, src_vreg, vmov_src_vreg);
            addMachineInst(src_vmov_inst);
            src_vreg = vmov_src_vreg;
        }
        cvt_dst_vreg = createVirtualReg(MachineOperand::Float);
        cvt_inst = new CvtInst(curr_machine_basic_block_, CvtInst::F2I, cvt_dst_vreg, src_vreg);
        addMachineInst(cvt_inst);

        vmov_dst_vreg = createVirtualReg(MachineOperand::Int, inst);
        vmov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F_I, cvt_dst_vreg, vmov_dst_vreg);
        addMachineInst(vmov_inst);
    }

}

void CodeGen::visit(LoadInstruction *inst) {
    // load返回的要么是一个int、float值，要么是一个指针
    MachineOperand::ValueType value_type;
    if (inst->isFromSecondaryPtr()) {
        value_type = MachineOperand::Int;
    } else {
        value_type = inst->getBasicType() == BasicType::INT_BTYPE ? MachineOperand::Int: MachineOperand::Float;
    }
    auto dst_reg = createVirtualReg(value_type, inst);

    auto value_reg = value2MachineOperand(inst->getPtr(), true);
    assert(value_reg);
    auto load_inst = new LoadInst(curr_machine_basic_block_, dst_reg, value_reg);

    addMachineInst(load_inst);
}

void CodeGen::visit(ZextInstruction *inst) {
    MachineOperand *value_operand = nullptr;
    if (auto set_cond_value = dynamic_cast<SetCondInstruction *>(inst->getValue())) {
        value_operand = getCmpReusltInOperand(set_cond_value);
    } else {
        value_operand = value2MachineOperand(inst->getValue(), true);
    }
    assert(value_operand);
    auto zext_dst = createVirtualReg(MachineOperand::Int, inst);

    auto mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, value_operand, zext_dst);

    addMachineInst(mov_inst);
}

MachineOperand *CodeGen::loadGlobalVarAddr(GlobalVariable *global, std::vector<MachineInst *> *move_insts) {
    MoveInst *move_inst = nullptr;
    auto addr_reg = createVirtualReg(MachineOperand::Int);
    auto find_label = global_var_map_.find(global->getName());
    if (find_label != global_var_map_.end()) {
        auto label = find_label->second.get();
        auto move_inst1 = new MoveInst(curr_machine_basic_block_, MoveInst::MoveType::L2I, label, addr_reg);
        auto move_inst2 = new MoveInst(curr_machine_basic_block_, MoveInst::MoveType::H2I, label, addr_reg);
        if (move_inst) {
            move_insts->push_back(move_inst1);
            move_insts->push_back(move_inst2);
        } else {
            addMachineInst(move_inst1);
            addMachineInst(move_inst2);
        }
    }
    return addr_reg;
}

MachineOperand *CodeGen::value2MachineOperandForPhi(Value *value, MachineBasicBlock *basic_block, std::vector<MachineInst *> *move_insts) {
    if (value_machinereg_map_.count(value)) {
        return value_machinereg_map_[value];
    }
    if (auto phi_inst = dynamic_cast<PhiInstruction *>(value); phi_inst && phi2vreg_map_.count(phi_inst)) {
        return phi2vreg_map_[phi_inst];
    }

    MachineOperand *ret_operand = nullptr;
    if (auto const_value = dynamic_cast<ConstantVar *>(value); const_value) {
        int32_t const_value_number;
        if (const_value->isFloat()) {
            const_value_number = getFloat2IntForm(const_value->getFValue());
        } else {
            const_value_number = const_value->getIValue();
        }

        ret_operand = constant2VirtualReg(const_value_number, false, basic_block, move_insts);

        if (const_value->isFloat()) {       // 因为前面的ret_operand是Int类型的,所以还需要涉及到一步到float的转换操作
            auto src = ret_operand;
            auto new_vreg = createVirtualReg(MachineOperand::Int);
            auto mov_i2i_inst = new MoveInst(basic_block, MoveInst::I2I, src, new_vreg);
            auto vdst = createVirtualReg(MachineOperand::Float);
            auto mov_i2f_inst = new MoveInst(basic_block, MoveInst::F_I, new_vreg, vdst);

            move_insts->push_back(mov_i2i_inst);
            move_insts->push_back(mov_i2f_inst);
            ret_operand = vdst;
        }
    }
    assert(ret_operand);
    return ret_operand;
}



MachineOperand *CodeGen::value2MachineOperand(Value *value, bool can_be_imm, bool *is_float) {
    assert(value);
    auto find_value = value_machinereg_map_.find(value);
    if (find_value != value_machinereg_map_.end()) {
        if (is_float != nullptr) {
            // printf("find the vreg%d type is %d\n", virreg->getRegId(), virreg->getValueType());
            *is_float = find_value->second->getValueType() == MachineOperand::Float;
        }
        return find_value->second;
    }

    if (auto phi_inst_value = dynamic_cast<PhiInstruction *>(value); phi_inst_value && phi2vreg_map_.count(phi_inst_value)) {
        return phi2vreg_map_[phi_inst_value];
    }

    MachineOperand *ret_operand = nullptr;
    auto value_type = value->getValueType();
    if (value_type == ValueType::GlobalVariableValue) {
        auto load_global_mov = loadGlobalVarAddr(dynamic_cast<GlobalVariable *>(value));
        ret_operand = load_global_mov;
        // auto load_global_vreg = curr_used_globals_[dynamic_cast<GlobalVariable *>(value)];
        // ret_operand = load_global_vreg;
    } else if (value_type == ValueType::ConstantValue) {
        auto const_value = dynamic_cast<ConstantVar *>(value);
        assert(const_value);
        int const_value_number;
        if (const_value->isFloat()) {
            const_value_number = getFloat2IntForm(const_value->getFValue());
        } else {
            const_value_number = const_value->getIValue();
        }

        ret_operand = constant2VirtualReg(const_value_number, can_be_imm, curr_machine_basic_block_);

        if (const_value->isFloat()) {       // 因为前面的ret_operand是Int类型的,所以还需要涉及到一步到float的转换操作
            auto src = ret_operand;
            auto new_vreg = createVirtualReg(MachineOperand::Int);
            auto mov_i2i_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, src, new_vreg);
            auto vdst = createVirtualReg(MachineOperand::Float);
            auto mov_i2f_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F_I, new_vreg, vdst);

            addMachineInst(mov_i2i_inst);
            addMachineInst(mov_i2f_inst);
            ret_operand = vdst;
            if (is_float) {
                *is_float = true;
            }
        }
    }
    if (ret_operand == nullptr) {
        printf("the value type is %d, the inst is %s\n", value_type, value->getName().c_str());
    }
    return ret_operand;
}

void CodeGen::addMoveForPhiInst() {
    for (auto &[phi_inst, vreg]: phi2vreg_map_) {
        int phi_size = phi_inst->getSize();
        for (int i = 0; i < phi_size; ++i) {
            auto vbb_pair = phi_inst->getValueBlock(i);
            auto value = vbb_pair.first;
            auto bb = vbb_pair.second;
            auto mc_bb = module_->getMachineBasicBlock(bb);

            curr_machine_basic_block_ = mc_bb;
            std::unordered_map<MachineInst *, std::vector<MachineInst *>> insert_before;
            std::unordered_map<MachineInst *, MachineBasicBlock::MachineInstListIt> insert_it;

            auto &inst_list = mc_bb->getInstructionListNonConst();
            for (auto it = inst_list.begin(); it != inst_list.end(); ++it) {
                auto mc_inst = it->get();
                if (auto br_inst = dynamic_cast<BranchInst *>(mc_inst); br_inst && br_inst->getBranchType() != BranchInst::Bl) {
                    VirtualReg *dst;
                    MoveInst *mov1, *mov2;
                    std::vector<MachineInst *> insert_insts;
                    MachineOperand *value_src = value2MachineOperandForPhi(value, mc_bb, &insert_insts);
                    // assert(value_src);
                    if (phi_inst->getBasicType() == INT_BTYPE) {
                        dst = createVirtualReg(VirtualReg::Int);
                        mov1 = new MoveInst(mc_bb, MoveInst::I2I, value_src, dst);
                        mov2 = new MoveInst(mc_bb, MoveInst::I2I, dst, vreg);
                    } else {
                        dst = createVirtualReg(VirtualReg::Float);
                        mov1 = new MoveInst(mc_bb, MoveInst::F2F, value_src, dst);
                        mov2 = new MoveInst(mc_bb, MoveInst::F2F, dst, vreg);
                    }
                    insert_insts.push_back(mov1);
                    insert_insts.push_back(mov2);
                    insert_before[mc_inst] = insert_insts;
                    insert_it.insert({mc_inst, it});
                }
            }

            for (auto [inserted, insts]: insert_before) {
                auto find_inserted_it = insert_it.find(inserted);
                assert(find_inserted_it != insert_it.end());
                for (auto inst: insts) {
                    auto mov_inst = dynamic_cast<MoveInst *>(inst);
                    mc_bb->insertInstructionBefore(find_inserted_it->second, inst);
                }
            }
        }
    }
}

void CodeGen::initForGlobals(Function *func, std::vector<MachineInst *> &move_insts) {
    auto curr_func_used_globals = func->getUsedGlobals();
    for (auto global: curr_func_used_globals) {
        auto vreg = loadGlobalVarAddr(global, &move_insts);
        curr_used_globals_.insert({global, vreg});
    }
}

MachineOperand *CodeGen::getCmpReusltInOperand(SetCondInstruction *set_cond_inst) {
    MachineOperand *value;
    auto find_setcond_it = set_cond_it_map_.find(set_cond_inst);
    assert(find_setcond_it != set_cond_it_map_.end());
    value = createVirtualReg(MachineOperand::ValueType::Int);
    auto cmp_type = set_cond_inst->getCmpType();
    auto mov0_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, new ImmNumber(0), value);
    auto mov1_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, new ImmNumber(1), value);
    switch (cmp_type) {
        case SetCondInstruction::SetGT:
            mov1_inst->setMoveCond(MoveInst::MoveGt);
            break;
        case SetCondInstruction::SetLT:
            mov1_inst->setMoveCond(MoveInst::MoveLt);
            break;
        case SetCondInstruction::SetGE:
            mov1_inst->setMoveCond(MoveInst::MoveGe);
            break;
        case SetCondInstruction::SetLE:
            mov1_inst->setMoveCond(MoveInst::MoveLe);
            break;
        case SetCondInstruction::SetEQ:
            mov1_inst->setMoveCond(MoveInst::MoveEq);
            break;
        case SetCondInstruction::SetNE:
            mov1_inst->setMoveCond(MoveInst::MoveNe);
            break;
    }
    curr_machine_basic_block_->insertInstruction(find_setcond_it->second, mov1_inst);
    curr_machine_basic_block_->insertInstruction(find_setcond_it->second, mov0_inst);

    return value;
}

MachineReg *CodeGen::getMachineReg(bool isfloat, int reg_no) {
    MachineReg::Reg reg;
    if (isfloat) {
        reg = static_cast<MachineReg::Reg>(MachineReg::s0 + reg_no);
    } else {
        reg = static_cast<MachineReg::Reg>(MachineReg::r0 + reg_no);
    }
    return module_->getMachineReg(reg);
}

void CodeGen::visit(GlobalVariable *global) {
    auto label = new Label(global->getName());
    global_var_map_[label->getName()] = GET_UNIQUEPTR(label);
}

void CodeGen::visit(StoreInstruction *inst) {
    bool is_float = false;
    auto store_addr_reg = value2MachineOperand(inst->getPtr(), false, &is_float);
    assert(store_addr_reg);
    auto value_operand = value2MachineOperand(inst->getValue(), false, &is_float);
    assert(value_operand);
    auto store_inst = new StoreInst(curr_machine_basic_block_, value_operand, store_addr_reg);
    store_inst->setValueType(!is_float ? MoveInst::ValueType::Int: MoveInst::ValueType::Float);
    addMachineInst(store_inst);
}

void CodeGen::visit(AllocaInstruction *inst) {      // 首先需要在栈上分配,然后将地址返回到一个dst寄存器上
    int stack_move_offset = 1;
    auto dst_reg = createVirtualReg(MachineOperand::Int, inst);
    if (inst->isArray() && !inst->isPtrPtr()) {
        auto dimension_numbers = inst->getArrayDimensionSize();
        for (auto dimension_number: dimension_numbers) {
            stack_move_offset *= dimension_number;
        }
    }
    stack_move_offset *= 4;
    auto offset = getImmOperandInBinary(moveStackOffset(stack_move_offset), curr_machine_basic_block_);
    auto sub_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::ISub, dst_reg, fp_reg_, offset);
    value_machinereg_map_[inst] = dst_reg;
    setCurrMachineOperand(dst_reg);
    addMachineInst(sub_inst);
}

void CodeGen::visit(BranchInstruction *inst) {
    if (inst->isCondBranch()) {
        Label *branch1, *branch2;
        branch1 = new Label(curr_machine_function_->getFunctionName() + "." + inst->getTrueLabel()->getName());
        branch2 = new Label(curr_machine_function_->getFunctionName() + "." + inst->getFalseLabel()->getName());

        BranchInst *branch_inst1, *branch_inst2;
        branch_inst1 = new BranchInst(curr_machine_basic_block_, branch1);
        branch_inst2 = new BranchInst(curr_machine_basic_block_, branch2);

        if (auto cmp_cond = dynamic_cast<SetCondInstruction *>(inst->getCond())) {
            assert(cmp_cond);
            auto cmp_type = cmp_cond->getCmpType();
            BranchInst::BranchCond branch_cond;
            switch (cmp_type) {
                case SetCondInstruction::SetNE:
                    branch_cond = BranchInst::BrNe;
                    break;
                case SetCondInstruction::SetEQ:
                    branch_cond = BranchInst::BrEq;
                    break;
                case SetCondInstruction::SetLE:
                    branch_cond = BranchInst::BrLe;
                    break;
                case SetCondInstruction::SetGE:
                    branch_cond = BranchInst::BrGe;
                    break;
                case SetCondInstruction::SetLT:
                    branch_cond = BranchInst::BrLt;
                    break;
                case SetCondInstruction::SetGT:
                    branch_cond = BranchInst::BrGt;
                    break;
            }
            branch_inst1->setBrCond(branch_cond);
        } else {
            auto cond_value_operand = value2MachineOperand(inst->getCond(), false);
            auto cmp_inst = new CmpInst(curr_machine_basic_block_, cond_value_operand, new ImmNumber(1));
            addMachineInst(cmp_inst);
            branch_inst1->setBrCond(BranchInst::BrEq);
        }
        addMachineInst(branch_inst1);
        addMachineInst(branch_inst2);
    } else {
        Label *branch = new Label(curr_machine_function_->getFunctionName() + "." + inst->getLabel()->getName());
        auto branch_inst = new BranchInst(curr_machine_basic_block_, branch);
        addMachineInst(branch_inst);
    }
}

void CodeGen::visit(MemSetInstruction *inst) {

}

void CodeGen::visit(SetCondInstruction *inst) {     // 一般紧接着就是跳转语句
    MachineInst::ValueType value_type = inst->isFloatCmp() ? MachineInst::Float: MachineInst::Int;
    auto cmp_inst = new CmpInst(curr_machine_basic_block_);
    cmp_inst->setValueType(value_type);

    auto lhs_value = inst->getLeft();
    auto rhs_value = inst->getRight();

    auto lhs = value2MachineOperand(lhs_value, false);
    assert(lhs);
    auto rhs = value2MachineOperand(rhs_value, false);
    assert(rhs);

    cmp_inst->setLhs(lhs);
    cmp_inst->setRhs(rhs);

    addMachineInst(cmp_inst);
    set_cond_it_map_[inst] = curr_machine_basic_block_->getInstBackIt();
}

void CodeGen::visit(UnaryOpInstruction *uinst) {        // 一元操作
    auto uinst_op = uinst->getInstType();
    if (uinst_op == InstructionType::NotType) {

        bool is_float = false;

        MachineOperand *value = nullptr;
        if (auto set_cond_inst = dynamic_cast<SetCondInstruction *>(uinst->getValue())) {
            value = getCmpReusltInOperand(set_cond_inst);
        } else {
                value = value2MachineOperand(uinst->getValue(), true,&is_float);
        }

        auto dst_reg = createVirtualReg(MachineOperand::Int);
        auto clz_inst = new ClzInst(curr_machine_basic_block_, dst_reg, value);
        auto lsr_dst_reg = createVirtualReg(MachineOperand::Int, uinst);
        auto lsr_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::ILsr, lsr_dst_reg, dst_reg,
                                       new ImmNumber(5));

        addMachineInst(clz_inst);
        addMachineInst(lsr_inst);

    } else if (uinst_op == InstructionType::NegType) {
        if (uinst->getBasicType() == BasicType::INT_BTYPE) {
            auto rsb_dst = createVirtualReg(basicType2ValueType(uinst->getBasicType()), uinst);
            auto rsb_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IRsb, rsb_dst,
                                           value2MachineOperand(uinst->getValue(), false), new ImmNumber(0));
            addMachineInst(rsb_inst);
            setCurrMachineOperand(rsb_dst);
        } else {
            auto dst_vreg = createVirtualReg(MachineOperand::Float, uinst);
            auto value_src = value2MachineOperand(uinst->getValue(), false);
            auto vneg_inst = new VnegInst(curr_machine_basic_block_, dst_vreg, value_src);
            addMachineInst(vneg_inst);
            setCurrMachineOperand(dst_vreg);
        }
    }
}

void CodeGen::visit(BinaryOpInstruction *binst) {       // 二元操作
    auto binary_op = binst->getInstType();
    BasicType basic_type = binst->getBasicType();
    MachineOperand::ValueType value_type = binst->getBasicType() == BasicType::INT_BTYPE ? MachineOperand::Int : MachineOperand::Float;
    BinaryInst::BinaryOp binary_inst_op;
    bool can_be_swap = false;
    bool rhs_can_be_imm = false;

    Value *left_value = binst->getLeft();
    Value *right_value = binst->getRight();

    bool is_mod = false;
    switch (binary_op) {
        case InstructionType::AddType:
            can_be_swap = true;
            rhs_can_be_imm = true;
            binary_inst_op = basic_type == BasicType::INT_BTYPE ? BinaryInst::IAdd: BinaryInst::FAdd;
            break;
        case InstructionType::SubType:
            rhs_can_be_imm = true;
            binary_inst_op = basic_type == BasicType::INT_BTYPE ? BinaryInst::ISub: BinaryInst::FSub;
            break;
        case InstructionType::MulType:
            can_be_swap = true;
            binary_inst_op = basic_type == BasicType::INT_BTYPE ? BinaryInst::IMul: BinaryInst::FMul;
            break;
        case InstructionType::DivType:
            binary_inst_op = basic_type == BasicType::INT_BTYPE ? BinaryInst::IDiv: BinaryInst::FDiv;
            break;
        case InstructionType::ModType: {
            auto div_dst = createVirtualReg(MachineOperand::Int);
            auto mul_dst = createVirtualReg(MachineOperand::Int);
            auto sub_dst = createVirtualReg(MachineOperand::Int, binst);

            MachineOperand *lhs = value2MachineOperand(left_value, false);
            assert(lhs);
            MachineOperand *rhs = value2MachineOperand(right_value, false);
            assert(rhs);

            auto div_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IDiv, div_dst, lhs, rhs);
            addMachineInst(div_inst);

            auto mul_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::IMul, mul_dst, div_dst, rhs);
            addMachineInst(mul_inst);

            auto sub_inst = new BinaryInst(curr_machine_basic_block_, BinaryInst::ISub, sub_dst, lhs, mul_dst);
            addMachineInst(sub_inst);
            return;
        }
        default:
            break;
    }

    if (dynamic_cast<ConstantVar *>(left_value) && !dynamic_cast<ConstantVar *>(right_value) && can_be_swap) {
        std::swap(left_value, right_value);
    }

    MachineOperand *lhs = value2MachineOperand(left_value, false);
    assert(lhs);
    MachineOperand *rhs = value2MachineOperand(right_value, false);
    assert(rhs);

    auto binary_dst = createVirtualReg(value_type, binst);
    auto binary_inst = new BinaryInst(curr_machine_basic_block_, binary_inst_op, binary_dst, lhs, rhs);

    addMachineInst(binary_inst);
}

void CodeGen::visit(PhiInstruction *inst) {
    VirtualReg *phi_vreg, *dst_vreg;
    MoveInst *mov_inst;
    if (inst->getBasicType() == INT_BTYPE) {
        phi_vreg = createVirtualReg(VirtualReg::Int);
        dst_vreg = createVirtualReg(VirtualReg::Int, inst);
        mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::I2I, phi_vreg, dst_vreg);
    } else {
        phi_vreg = createVirtualReg(VirtualReg::Float);
        dst_vreg = createVirtualReg(VirtualReg::Float, inst);
        mov_inst = new MoveInst(curr_machine_basic_block_, MoveInst::F2F, phi_vreg, dst_vreg);
    }
    phi2vreg_map_[inst] = phi_vreg;
    addMachineInst(mov_inst);
}

