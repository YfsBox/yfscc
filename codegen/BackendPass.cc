//
// Created by 杨丰硕 on 2023/7/7.
//

#include "BackendPass.h"
#include "Machine.h"
#include "MachineDumper.h"
#include <cassert>

BackendPass::BackendPass(MachineModule *module): module_(module), curr_func_(nullptr) {}

void BackendPass::run() {
    for (auto &func: module_->getMachineFunctions()) {
        curr_func_ = func.get();
        runOnFunction();
    }
}

BackendPassManager::BackendPassManager(MachineModule *module): module_(module) {}

ReDundantLoadElim::ReDundantLoadElim(MachineModule *module): BackendPass(module) {}

void ReDundantLoadElim::runOnFunction() {

    remove_insts_.clear();

    auto get_mreg = [&](MachineOperand *operand) -> MachineReg::Reg {
        MachineReg::Reg value_reg;
        if (operand->getOperandType() == MachineOperand::MachineReg) {
            auto store_value_mreg = dynamic_cast<MachineReg *>(operand);
            value_reg = store_value_mreg->getReg();
        } else if (operand->getOperandType() == MachineOperand::VirtualReg) {
            auto store_value_vreg = dynamic_cast<VirtualReg *>(operand);
            value_reg = store_value_vreg->getColoredReg();
        }
        return value_reg;
    };

    for (auto &bb_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto bb = bb_uptr.get();
        auto &insts_list = bb->getInstructionListNonConst();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
            if (auto store_inst = dynamic_cast<StoreInst *>(inst_it->get()); store_inst) {
                auto store_value = store_inst->getValue();
                auto store_value_mreg = get_mreg(store_value);              // value mreg
                auto store_base_mreg = get_mreg(store_inst->getBase());     // base mreg
                auto store_offset = store_inst->getOffset();              // offset value
                int32_t store_offset_imm = -1;
                MachineReg::Reg store_offset_mreg = MachineReg::Reg::undef;
                bool store_is_offset_imm = false;
                if (store_offset) {
                    // 是否是立即数的形式保存
                    if (store_offset->getOperandType() == MachineOperand::ImmNumber) {
                        store_offset_imm = dynamic_cast<ImmNumber *>(store_offset)->getIValue();
                        // printf("the store offset imm is %d\n", store_offset_imm);
                        store_is_offset_imm = true;
                    } else {
                        store_value_mreg = get_mreg(store_offset);
                    }
                }

                inst_it++;
                if (auto load_inst = dynamic_cast<LoadInst *>(inst_it->get()); load_inst) {
                    auto load_dst_mreg = get_mreg(load_inst->getDst());
                    auto load_base_mreg = get_mreg(load_inst->getBase());
                    auto load_offset = load_inst->getOffset();

                    int32_t load_offset_imm = -1;
                    MachineReg::Reg load_offset_mreg = MachineReg::Reg::undef;
                    bool load_is_offset_imm = false;
                    if (load_offset && store_offset) {
                        if (load_offset->getOperandType() == MachineOperand::ImmNumber) {
                            load_offset_imm = dynamic_cast<ImmNumber *>(load_offset)->getIValue();
                            load_is_offset_imm = true;
                        } else {
                            load_offset_mreg = get_mreg(load_offset);
                        }
                    }
                    if (load_dst_mreg != store_value_mreg || store_base_mreg != load_base_mreg) {
                        inst_it--;
                        continue;
                    }

                    if ((!load_offset && !store_offset)
                        ||(load_is_offset_imm && store_is_offset_imm && load_offset_imm == store_offset_imm)
                        || (!load_is_offset_imm && !store_is_offset_imm && load_offset_mreg == store_offset_mreg)) {
                        // printf("erase redandunt load inst\n");
                        inst_it = insts_list.erase(inst_it);
                    }

                }
                inst_it--;
            }
        }
    }

}

