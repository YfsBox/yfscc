//
// Created by 杨丰硕 on 2023/7/1.
//

#include "SimplifyPhiInsts.h"
#include "../ir/Instruction.h"
#include "../ir/BasicBlock.h"
#include "../ir/Value.h"
#include "../ir/User.h"

void SimplifyPhiInsts::collectUserSets() {
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            for (int i = 0; i < inst->getOperandNum(); ++i) {
                auto operand = inst->getOperand(i);
                if (operand->getValueType() ==  InstructionValue || operand->getValueType() == ArgumentValue) {
                    user_insts_map_[operand].insert(inst);
                }
            }
        }
    }
}


void SimplifyPhiInsts::runOnFunction() {
    collectUserSets();
    std::unordered_map<PhiInstruction *, Value *> phi_insts_map;
    std::unordered_set<PhiInstruction *> has_simplify_insts;

    bool has_changed = true;

    while (has_changed) {
        has_changed = false;
        auto old_simplify_size = has_simplify_insts.size();

        for (auto &bb_uptr: curr_func_->getBlocks()) {
            auto bb = bb_uptr.get();
            printf("the block is %s\n", bb->getName().c_str());
            for (auto &inst_uptr: bb->getInstructionList()) {
                auto inst = inst_uptr.get();
                if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst); phi_inst && !has_simplify_insts.count(phi_inst)) {
                    printf("the phi inst %s has size %d\n", phi_inst->getName().c_str(), phi_inst->getSize());
                    if (phi_inst->getSize() == 1) {
                        auto value_bb = phi_inst->getValueBlock(0);
                        bool need_simplify = true;
                        for (auto user_inst: user_insts_map_[phi_inst]) {
                            if (user_inst != value_bb.first) {
                                user_inst->replaceWithValue(phi_inst, value_bb.first);
                                user_inst->replaceWithValue(phi_inst, value_bb.second);
                            } else {
                                need_simplify = false;
                            }
                        }
                        if (need_simplify) {
                            has_simplify_insts.insert(phi_inst);
                        }
                    }
                }
            }
        }

        if (old_simplify_size != has_simplify_insts.size()) {
            has_changed = true;
        }
    }
}
