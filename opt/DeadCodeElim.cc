//
// Created by 杨丰硕 on 2023/6/9.
//
#include <algorithm>
#include <cassert>
#include <queue>
#include <vector>
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "../ir/Instruction.h"
#include "../ir/IrDumper.h"
#include "DeadCodeElim.h"
#include "DataflowAnalysis.h"

void DeadCodeElim::removeDeadInsts() {
    auto &bbs = curr_func_->getBlocks();
    for (auto &bb: bbs) {
        auto &insts = bb->getInstructionList();
        for (auto it = insts.begin(); it != insts.end();) {
            auto inst = it->get();
            if (dead_insts_.count(inst)) {
                it = insts.erase(it);
            } else {
                it++;
            }
        }
    }
}

bool DeadCodeElim::hasSideEffect(Instruction *inst) {
    auto inst_type = inst->getInstType();
    if (inst_type == StoreType || inst_type == RetType || inst_type == BrType || inst_type == CallType) {
        return true;
    }
    return false;
}

void DeadCodeElim::markAsUsefulInst(Instruction *inst) {
    if (useful_insts_.count(inst)) {
        return;
    }
    useful_insts_.insert(inst);
    auto uses = inst->getUses();
    for (auto use: uses) {
        auto use_inst = dynamic_cast<Instruction *>(use);
        if (use_inst) {
            markAsUsefulInst(use_inst);
        }
    }
}

void DeadCodeElim::runOnFunction() {
    useful_insts_.clear();
    dead_insts_.clear();

    LivenessAnalysis liveness_analysis(curr_func_);
    liveness_analysis.analysis();
    auto live_out = liveness_analysis.getOutSet();
    auto live_in = liveness_analysis.getInSet();

    for (auto &bb: curr_func_->getBlocks()) {
        auto live = live_out[bb.get()];
        auto &insts_list = bb->getInstructionList();

        for (auto rit = insts_list.rbegin(); rit != insts_list.rend(); ++rit) {
            Instruction *inst = rit->get();

            auto defs = inst->getDefs();
            auto uses = inst->getUses();

            if (!hasSideEffect(inst)) {
                assert(defs.size() == 1);
                if (!live.count(*defs.begin())) {
                    dead_insts_.insert(inst);
                }
            } else if (inst->getInstType() == StoreType) {
                auto store_inst = dynamic_cast<StoreInstruction *>(inst);
                auto addr_inst = dynamic_cast<AllocaInstruction *>(store_inst->getPtr());
                if (addr_inst && !live.count(addr_inst)) {
                    dead_insts_.insert(inst);
                }
            }

            for (auto def: defs) {
                live.erase(def);
            }
            for (auto use: uses) {
                live.insert(use);
            }
        }
    }

    for (auto dead_inst: dead_insts_) {
        irdumper_->dump(dead_inst);
    }

    if (!dead_insts_.empty()) {
        removeDeadInsts();
        runOnFunction();
    }
    // removeDeadInsts();
}