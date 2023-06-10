//
// Created by 杨丰硕 on 2023/5/12.
//
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "DataflowAnalysis.h"

bool DataflowAnalysis::isEqual(const BitSet &left, const BitSet &right) {
    if (left.size() != right.size()) {
        return false;
    }
    for (auto value: left) {
        if (!right.count(value)) {
            return false;
        }
    }
    return true;
}

void LivenessAnalysis::analysis() {
    auto &basic_blocks = function_->getBlocks();
    for (auto &bb: basic_blocks) {
        auto &insts = bb->getInstructionList();
        BitSet tmp_use_sets;
        BitSet tmp_def_sets;
        for (auto &inst: insts) {
            auto inst_defs = inst->getDefs();
            auto inst_uses = inst->getUses();

            for (auto def: inst_defs) {
                tmp_def_sets.insert(def);
            }
            for (auto use: inst_uses) {
                tmp_use_sets.insert(use);
            }
        }

        use_sets_[bb.get()] = std::move(tmp_use_sets);
        def_sets_[bb.get()] = std::move(tmp_def_sets);
    }

    bool has_changed = true;

    while (has_changed) {
        has_changed = false;

        for (auto &bb : basic_blocks) {
            auto old_in = live_in_[bb.get()];
            auto old_out = live_out_[bb.get()];

            live_in_[bb.get()] = use_sets_[bb.get()];
            for (auto &out: live_out_[bb.get()]) {
                if (!def_sets_[bb.get()].count(out)) {
                    live_in_[bb.get()].insert(out);
                }
            }

            live_out_[bb.get()].clear();
            for (auto succ : bb->getSuccessorBlocks()) {
                for (auto succ_in: live_in_[succ]) {
                    live_out_[bb.get()].insert(succ_in);
                }
            }

            if (!has_changed) {
                if (!isEqual(live_in_[bb.get()], old_in) || !isEqual(live_out_[bb.get()], old_out)) {
                    has_changed = true;
                }
            }
        }
    }
}

void AvailExprsAnalysis::analysis() {

}

void ReachingDefsAnalysis::analysis() {

}