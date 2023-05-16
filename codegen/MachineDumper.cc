//
// Created by 杨丰硕 on 2023/5/3.
//
#include <cassert>
#include <map>
#include "MachineDumper.h"
#include "Machine.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "../ir/Constant.h"
#include "../ir/GlobalVariable.h"
#include "../ir/Module.h"
#include "../common/Utils.h"

// 库函数的调用
// 头文件 
// BSS
// alloc


MachineDumper::MachineDumper(MachineModule *module, const std::string &file_name):
    module_(module),
    curr_function_(nullptr),
    global_set_(module_->getIRModule()->getGlobalSet()),
    out_file_name_(file_name),
    fout_(file_name){
    if (!fout_.is_open()) {
        assert("file open error");
    }
}

MachineDumper::~MachineDumper() {
    fout_.close();
}

void MachineDumper::dump(const MachineModule *module) {
    fout_ << ".arch armv8-a\n.text\n.align 2\n.syntax unified\n.arm\n.global main\n\n";
    const auto &functions = module->getMachineFunctions();
    for (auto &function: functions) {
        dump(function.get());
    }
    fout_ << "\n";
    dumpGlobals();
}

void MachineDumper::dump(const MachineFunction *function) {
    fout_ << function->getFunctionName() << ":\n";
    curr_function_ = const_cast<MachineFunction *>(function);
    const auto &basicblocks = function->getMachineBasicBlock();
    for (auto &basicblock: basicblocks) {
        dump(basicblock.get());
    }
}

void MachineDumper::dump(const MachineBasicBlock *basicblock) {
    fout_ << basicblock->getLabelName() << ":\n";
    const auto &instructions = basicblock->getInstructionList();
    for (auto &instruction: instructions) {
        dump(instruction.get());
    }
}

void MachineDumper::dump(const MachineInst *inst) {
    auto inst_type = inst->getMachineInstType();
    if (inst->getParent() == nullptr) {
        fout_ << "not parent";
    }
    switch (inst_type) {
        case MachineInst::Move:
            dump(dynamic_cast<const MoveInst *>(inst));
            return; 
        case MachineInst::Clz:
            dump(dynamic_cast<const ClzInst *>(inst));
            return;
        case MachineInst::Binary:
            dump(dynamic_cast<const BinaryInst *>(inst));
            return;
        case MachineInst::Cmp:
            dump(dynamic_cast<const CmpInst *>(inst));
            return;
        case MachineInst::Call:
            dump(dynamic_cast<const CallInst *>(inst));
            return;
        case MachineInst::Branch:
            dump(dynamic_cast<const BranchInst *>(inst));
            return;
        case MachineInst::Ret:
            dump(dynamic_cast<const RetInst *>(inst));
            return;
        case MachineInst::Push:
            dump(dynamic_cast<const PushInst *>(inst));
            return;
        case MachineInst::Pop:
            dump(dynamic_cast<const PopInst *>(inst));
            return;
        case MachineInst::Store:
            dump(dynamic_cast<const StoreInst *>(inst));
            return;
        case MachineInst::Load:
            dump(dynamic_cast<const LoadInst *>(inst));
            return;
        case MachineInst::Cvt:
            dump(dynamic_cast<const CvtInst *>(inst));
            return;
        case MachineInst::Vneg:
            dump(dynamic_cast<const VnegInst *>(inst));
            return;
    }
}

void MachineDumper::dump(const MachineOperand *operand) {
    assert(operand);
    auto operand_type = operand->getOperandType();
    switch (operand_type) {
        case MachineOperand::MachineReg:
            dump(dynamic_cast<const MachineReg *>(operand));
            return;
        case MachineOperand::VirtualReg:
            dump(dynamic_cast<const VirtualReg *>(operand));
            return;
        case MachineOperand::ImmNumber:
            dump(dynamic_cast<const ImmNumber *>(operand));
            return;
        case MachineOperand::Label:
            dump(dynamic_cast<const Label *>(operand));
            return;
    }
}

void MachineDumper::dump(const Label *operand) {
    fout_ << operand->getName();
}

void MachineDumper::dump(const MachineReg *operand) {
    fout_ << operand->machieReg2RegName();
}

void MachineDumper::dump(const VirtualReg *operand) {
    if (operand->isColored()) {
        fout_ << MachineReg::machineReg2RegName(operand->getColoredReg());
        return;
    }
    if (operand->getValueType() == MachineOperand::Float) {
        fout_ << "vfreg" << operand->getRegId();
    } else {
        fout_ << "vreg" << operand->getRegId();
    }
}

void MachineDumper::dump(const ImmNumber *operand) {
    if(operand->isFloat()) {
        fout_ << "#" << operand->getFValue();
    } else {
        fout_ << "#" << operand->getIValue();
    }
}

//??? 如何得到label
void MachineDumper::dump(const BranchInst *inst) {
    fout_ << "\t";
    auto branch_type = inst->getBranchType();
    switch (branch_type) {
        case BranchInst::BranchType::B:
            fout_ << "b";
            break;
        case BranchInst::BranchType::Bx:
            fout_ << "bx";
            break;
        case BranchInst::BranchType::Bl:
            fout_ << "bl";
            break;
    }
    auto branch_cond = inst->getBranchCond();
    switch (branch_cond) {
        case BranchInst::BrGt:
            fout_ << "gt";
            break;
        case BranchInst::BrLt:
            fout_ << "lt";
            break;
        case BranchInst::BrGe:
            fout_ << "ge";
            break;
        case BranchInst::BrLe:
            fout_ << "le";
            break;
        case BranchInst::BrEq:
            fout_ << "eq";
            break;
        case BranchInst::BrNe:
            fout_ << "ne";
            break;
    }
    fout_ << "\t";
    dump(inst->getOperand());
    fout_ << "\n";
}

void MachineDumper::dump(const BinaryInst *inst) {
    BinaryInst::BinaryOp binary_type = inst->getBinaryOp();
    fout_ << "\t";
    switch (binary_type) {
        case BinaryInst::BinaryOp::IAdd:
            fout_ << "add";
            break;
        case BinaryInst::BinaryOp::ISub:
            fout_ << "sub";
            break;
        case BinaryInst::BinaryOp::IMul:
            fout_ << "mul";
            break;
        case BinaryInst::BinaryOp::IDiv:
            fout_ << "sdiv";
            break;
        case BinaryInst::BinaryOp::FAdd:
            fout_ << "vadd.f32";
            break;
        case BinaryInst::BinaryOp::FSub:
            fout_ << "vsub.f32";
            break;
        case BinaryInst::BinaryOp::FMul:
            fout_ << "vmul.f32";
            break;
        case BinaryInst::BinaryOp::FDiv:
            fout_ << "vdiv.f32";
            break;
        case BinaryInst::BinaryOp::ILsr:
            fout_ << "lsr";
            break;
        case BinaryInst::BinaryOp::IRsb:
            fout_ << "rsb";
            break;
        case BinaryInst::BinaryOp::IAsl:
            fout_ << "asl";
            break;
        case BinaryInst::BinaryOp::ILsl:
            fout_ << "lsl";
            break;
        case BinaryInst::BinaryOp::IAsr:
            fout_ << "asr";
            break;
    }
    fout_ << "\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getLeft());
    fout_ << ", ";
    dump(inst->getRight());
    fout_ << "\n";
}

void MachineDumper::dump(const StoreInst *inst) {
    fout_ << "\t";
    if (inst->getValueType() == MachineInst::Float) {
        fout_ << "v";
    }
    fout_ << "str\t";
    dump(inst->getValue());
    fout_ << ", [";
    dump(inst->getBase());
    if (inst->getOffset()) {
        fout_ << ", ";
        dump(inst->getOffset());
    }
    fout_ << "]\n";
}

void MachineDumper::dump(const MoveInst *inst) {
    fout_ << "\t";
    auto move_type = inst->getMoveType();

    switch (move_type) {
        case MoveInst::MoveType::I2I:
            fout_ << "mov";
            break;
        case MoveInst::MoveType::F2F:
            fout_ << "vmov";
            break;
        case MoveInst::MoveType::F_I:
            fout_ << "vmov";
            break;
        case MoveInst::MoveType::L2I:
            fout_ << "movw";
            break;
        case MoveInst::MoveType::H2I:
            fout_ << "movt";
            break;
    }

    auto move_cond = inst->getMoveCond();
    switch (move_cond) {
        case MoveInst::MoveNe:
            fout_ << "ne";
            break;
        case MoveInst::MoveEq:
            fout_ << "eq";
            break;
        case MoveInst::MoveLe:
            fout_ << "le";
            break;
        case MoveInst::MoveGe:
            fout_ << "ge";
            break;
        case MoveInst::MoveLt:
            fout_ << "lt";
            break;
        case MoveInst::MoveGt:
            fout_ << "gt";
            break;
    }
    if (move_type == MoveInst::F2F) {
        fout_ << ".f32";
    }

    fout_ << "\t";
    dump(inst->getDst());
    fout_ << ", ";
    if (inst->getSrc()->getOperandType() == MachineOperand::Label) {
        if (move_type == MoveInst::L2I) {
            fout_ << "#:lower16:";
        } else if (move_type == MoveInst::H2I) {
            fout_ << "#:upper16:";
        }
    }
    dump(inst->getSrc());
    fout_ << "\n";
}

//无法确定load寻址的类型
void MachineDumper::dump(const LoadInst *inst) {
    fout_ << "\t";
    if (inst->getValueType() == MachineInst::Float) {
        fout_ << "v";
    }
    fout_ << "ldr\t";
    dump(inst->getDst());
    fout_ << ", [";
    dump(inst->getBase());
    if (inst->getOffset()) {
        fout_ << ", ";
        dump(inst->getOffset());
    }
    fout_ << "]\n";
}

void MachineDumper::dump(const CallInst *inst) {

}

void MachineDumper::dump(const PopInst *inst) {
    fout_ << "\t";
    if (inst->getValueType() == MachineInst::Float) {
        fout_ << "v";
    }
    fout_ << "pop\t" << "{";
    auto reg_size = inst->getRegsSize();
    for(int i = 0; i < reg_size; i++){
        dump(inst->getReg(i));
        if (i != reg_size - 1) {
            fout_ <<",";
        }
    }
    fout_ << "}" << std::endl;
}

void MachineDumper::dump(const CmpInst *inst) {
    fout_ << "\t";
    if (inst->getValueType() == MachineInst::Int) {
        fout_ << "cmp";
    } else {
        fout_ << "vcmp.f32";
    }
    fout_ << "\t";
    dump(inst->getLhs());
    fout_ << ", ";
    dump(inst->getRhs());
    fout_ << "\n";
    if (inst->getValueType() == MachineInst::Float) {
        fout_ << "\tvmrs\tAPSR_nzcv, FPSCR\n";
    }

}

void MachineDumper::dump(const ClzInst *inst) {
    fout_ << "\tclz\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << "\n";
}

void MachineDumper::dump(const RetInst *inst) {
    
}

void MachineDumper::dump(const PushInst *inst) {
    fout_ << "\t";
    if (inst->getValueType() == MachineInst::Float) {
        fout_ << "v";
    }
    fout_ << "push\t" << "{";
    auto reg_size = inst->getRegsSize();
    for(int i = 0; i< reg_size; i++) {
        dump(inst->getReg(i));
        if (i != reg_size - 1) {
            fout_ << ",";
        }
    }
    fout_ << "}" << "\n";
}

void MachineDumper::dump(const CvtInst *inst) {
    if (inst->getCvtType() == CvtInst::I2F) {
        fout_ << "\tvcvt.f32.s32\t";
    } else {
        fout_ << "\tvcvt.s32.f32\t";
    }
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << "\n";
}

void MachineDumper::dump(const VnegInst *inst) {
    fout_ << "\tvneg.f32\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << "\n";
}

void MachineDumper::dumpGlobals() {
    fout_ << ".data\n.align 2\n";
    for (auto global: global_set_) {
        fout_ << global->getName() << ":\n";
        if (dynamic_cast<ConstantVar *>(global->getConstInit())) {
            auto const_var = dynamic_cast<ConstantVar *>(global->getConstInit());
            fout_ << "  .word\t";
            if (global->getBasicType() == BasicType::INT_BTYPE) {
                fout_ << const_var->getIValue();
            } else {
                float tmp_value = const_var->getFValue();
                fout_ << *(int *)(&tmp_value);
            }
            fout_ << "\n";
        } else {
            auto const_array = dynamic_cast<ConstantArray *>(global->getConstInit());
            auto &init_value_map = const_array->getInitValueMap();
            int last_index = -1;
            size_t array_len = const_array->getArrayLen();
            for (auto &[index, value]: init_value_map) {
                fout_ << "  ";
                if (index != last_index + 1) {
                    fout_ << ".zero\t" << (index - last_index - 1) * 4 << "\n";
                    fout_ << "  ";
                }
                fout_ << ".word\t";
                if (global->getBasicType() == BasicType::INT_BTYPE) {
                    fout_ << value->getIValue();
                } else {
                    float tmp_value = value->getFValue();
                    fout_ << getFloat2IntForm(tmp_value);
                }
                fout_ << "\n";
                last_index = index;
            }

            if (array_len != last_index + 1) {
                fout_ << "  .zero\t" << (array_len - last_index - 1) * 4 << "\n";
            }
        }
        fout_ << "\n";
    }
}
