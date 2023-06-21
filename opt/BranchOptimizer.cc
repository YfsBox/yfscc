//
// Created by 杨丰硕 on 2023/6/21.
//

#include "BranchOptimizer.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"

bool BranchOptimizer::findRedundantAndReplace(BranchInstruction *br_inst, BasicBlock *target) {
    if (target->getInstructionList().size() == 1) {
        auto inst = target->getInstructionList().back().get();
        if (auto target_br_inst = dynamic_cast<BranchInstruction *>(inst); target_br_inst) {
            assert(!target_br_inst->isCondBranch());
            auto br_target_bb = target_br_inst->getLabel();
            assert(dynamic_cast<BasicBlock *>(br_target_bb));
            br_inst->replaceWithValue(target, br_target_bb);
            return true;
        }
    }
    return false;
}

void BranchOptimizer::runOnFunction() {

    redundant_brinsts_.clear();

    bool has_changed = true;
    while (has_changed) {
        has_changed = false;

        for (auto &bb_uptr: curr_func_->getBlocks()) {
            auto bb = bb_uptr.get();
            if (auto br_inst = dynamic_cast<BranchInstruction *>(bb->getInstructionList().back().get()); br_inst) {
                if (br_inst->isCondBranch()) {
                    auto br_true_target = dynamic_cast<BasicBlock *>(br_inst->getTrueLabel());
                    auto br_false_target = dynamic_cast<BasicBlock *>(br_inst->getFalseLabel());

                    assert(br_true_target);
                    assert(br_false_target);

                    has_changed |= findRedundantAndReplace(br_inst, br_true_target);
                    has_changed |= findRedundantAndReplace(br_inst, br_false_target);

                } else {
                    auto br_target = dynamic_cast<BasicBlock *>(br_inst->getLabel());
                    assert(br_inst);

                    has_changed |= findRedundantAndReplace(br_inst, br_target);
                }
            }
        }
    }

    for (auto &bb: curr_func_->getBlocks()) {
        bb->clearSuccessors();
        bb->clearPresuccessors();
    }
    curr_func_->bindBasicBlocks();

}