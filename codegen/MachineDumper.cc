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
    fout_ << "vreg" << operand->getRegId();
}

void MachineDumper::dump(const ImmNumber *operand) {
    if(operand->isFloat()) {
        fout_ << "#" << operand->getFValue();
    } else {
        fout_ << "#" << operand->getIValue();
    }
}

void MachineDumper::dump(const BranchInst *inst) {
    auto Cond_type = inst->getBranchCond();
    switch (Cond_type) {
        case BranchInst::BrNoCond:
            fout_ << "\tb\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrEq:
            fout_ << "\tbeq\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrNe:
            fout_ << "\tbne\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrLe:
            fout_ << "\tble\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrGe:
            fout_ << "\tbge\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrLt:
            fout_ << "\tblt\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
        case BranchInst::BrGt:
            fout_ << "\tbgt\t.";
            dump(inst->getBranchLabel());
            fout_ << "\n";
            return ;
    }
}

void MachineDumper::dump(const BinaryInst *inst) {
    //对状态寄存器的判断 所以没有写后缀S
    //如果对状态寄存器有改变的话 指令之后要加s 加和减的相关指令都有可能改变
    auto Op_type = inst->getBinaryOp();
    switch (Op_type) {
        case BinaryInst::IAdd:
            fout_ << "\tadd\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::ISub:
            fout_ << "\tsub\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::IMul:
            fout_ << "\tmul\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::IDiv:
            fout_ << "\tdiv\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::FAdd:
            fout_ << "\tadd\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::FSub:
            fout_ << "\tsub\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::FMul:
            fout_ << "\tmul\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::Mod:
            fout_ << "\tmul\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        //除了 mov 指令 其他的也可以将移位作为第二操作数
        case BinaryInst::ILsl:
            fout_ << "\tmov ";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", lsl ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::ILsr:
            fout_ << "\tmov ";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", lsr ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::IAsl:
            fout_ << "\tmov ";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", Asl ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::IAsr:
            fout_ << "\tmov ";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", Asr ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
        case BinaryInst::IRsb:
            fout_ << "\tsub\t";
            dump(inst->getDst());
            fout_ << ", ";
            dump(inst->getLeft());
            fout_ << ", ";
            dump(inst->getRight());
            fout_ << "\n";
            return;
    }
}

void MachineDumper::dump(const StoreInst *inst) {
    //多寄存器寻址时STM 但是没有多寄存器就只写了STR
    //前索引寻址：需要index_type
    //后索引寻址
    fout_ << "\tSTR\t";
    dump(inst->getValue());
    fout_ << ", [";
    dump(inst->getBase());
    fout_ << "], ";
    dump(inst->getOffset());
    fout_ << std::endl;
}

void MachineDumper::dump(const MoveInst *inst) {
    fout_ << "\t";
    auto move_type = inst->getMoveType();
    if (move_type == MoveInst::F2F) {
        fout_ << "v";
    }
    fout_ << "mov";
    if (move_type == MoveInst::L2I) {
        fout_ << "w";
    } else if (move_type == MoveInst::H2I) {
        fout_ << "t";
    }
     fout_ << "\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << std::endl;
}

//无法确定load寻址的类型
void MachineDumper::dump(const LoadInst *inst) {
    //多寄存器寻址时LDM 但是没有多寄存器就只写了LDR
    //前索引寻址：需要index_type
    //后索引寻址
    fout_ << "\tLDR\t";
    dump(inst->getDst());
    fout_ << ", [";
    dump(inst->getBase());
    fout_ << "], ";
    dump(inst->getOffset());
    fout_ << std::endl;
}

void MachineDumper::dump(const CallInst *inst) {
    fout_ << "\tbl\t";
    //输出函数的名字
    dump(inst->getCallLabel());
    fout_ << "\n";
}

void MachineDumper::dump(const PopInst *inst) {
    fout_ << "\tpop\t" << "{";
    auto reg_size = inst->getRegsSize();
    for(int i = 0; i < reg_size; i++){
        dump(inst->getReg(i));
        if (i != reg_size - 1) {
            fout_ <<", ";
        }
    }
    fout_ << "}" << std::endl;
}

void MachineDumper::dump(const CmpInst *inst) {
    fout_ << "\tcmp\t";
    dump(inst->getLhs());
    fout_ << ", ";
    dump(inst->getRhs());
    fout_ << std::endl;
}

void MachineDumper::dump(const ClzInst *inst) {
    fout_ << "\tclz\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << "\n";
}

void MachineDumper::dump(const RetInst *inst) {
    fout_ << "\tret\t" << "\n";
}

void MachineDumper::dump(const PushInst *inst) {
    fout_ << "\tpush\t" << "{";
    auto reg_size = inst->getRegsSize();
    for(int i = 0; i< reg_size; i++) {
        dump(inst->getReg(i));
        if (i != reg_size - 1) {
            fout_ << ", ";
        }
    }
    fout_ << "}" << "\n";
}

void MachineDumper::dumpGlobals() {
    fout_ << ".data\n.align2\n";
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
