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

}

void MachineDumper::dump(const MachineFunction *function) {


}

void MachineDumper::dump(const MachineBasicBlock *basicblock) {


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
    fout_ << operand->getReg();
}

void MachineDumper::dump(const VirtualReg *operand) {
    fout_ << operand->getRegId();
}

void MachineDumper::dump(const ImmNumber *operand) {
    if(operand -> isFloat())
        fout_ << "#" << operand->getFValue();
    else fout_ << "#" << operand->getIValue();
}

//??? 如何得到label
void MachineDumper::dump(const BranchInst *inst) {
    //fout_ << "." << inst->
}

void MachineDumper::dump(const BinaryInst *inst) {

}

void MachineDumper::dump(const StoreInst *inst) {

}

void MachineDumper::dump(const MoveInst *inst) {
    fout_ << "\tmov\t";
    dump(inst->getDst());
    fout_ << ", ";
    dump(inst->getSrc());
    fout_ << std::endl;
}

//无法确定load寻址的类型
void MachineDumper::dump(const LoadInst *inst) {

}

void MachineDumper::dump(const CallInst *inst) {

}

void MachineDumper::dump(const PopInst *inst) {
    fout_ << "\tpop\t" << "{";
    int i=0;
    for(;i<inst->getRegsSize()-1;i++){
        dump(inst->getReg(i));
        fout_ <<",";
    }
    dump(inst->getReg(i));
    fout_ << "}" << std::endl;
}

void MachineDumper::dump(const CmpInst *inst) {
    fout_ << "\tmov\t";
    dump(inst->getlhs());
    fout_ << ", ";
    dump(inst->getrhs());
    fout_ << std::endl;
}

void MachineDumper::dump(const ClzInst *inst) {

}

void MachineDumper::dump(const RetInst *inst) {
    
}

void MachineDumper::dump(const PushInst *inst) {
    fout_ << "\tpush\t" << "{";
    int i=0;
    for(;i<inst->getRegsSize()-1;i++){
        dump(inst->getReg(i));
        fout_ <<",";
    }
    dump(inst->getReg(i));
    fout_ << "}" << std::endl;
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
