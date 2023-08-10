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

DeadCodeElim::DeadCodeElim(Module *module): Pass(module), user_analysis_(std::make_unique<UserAnalysis>()), call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module)) {
    pass_name_ = "DeadCodeElim";
}

void DeadCodeElim::removeDeadInsts() {
    auto &bbs = curr_func_->getBlocks();
    for (auto &bb: bbs) {
        auto &insts = bb->getInstructionList();
        for (auto it = insts.begin(); it != insts.end();) {
            auto inst = it->get();
            if (dead_insts_.count(inst)) {
                inst->removeUseForOperand();
                it = insts.erase(it);
            } else {
                it++;
            }
        }
    }
}

bool DeadCodeElim::hasSideEffect(Instruction *inst) {
    auto inst_type = inst->getInstType();
    if (inst_type == StoreType || inst_type == RetType || inst_type == BrType) {
        return true;
    } else if (inst_type == CallType && call_graph_analysis_->hasSideEffect(dynamic_cast<CallInstruction *>(inst)->getFunction())) {
        return true;
    }
    return false;
}

void DeadCodeElim::runOnFunction() {
    call_graph_analysis_->analysis();
    useful_insts_.clear();
    dead_insts_.clear();

    user_analysis_->analysis(curr_func_);

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            auto inst = inst_uptr.get();
            if (user_analysis_->getUserInsts(inst).empty() && !hasSideEffect(inst)) {
                dead_insts_.insert(inst_uptr.get());
            }
        }
    }

    if (!dead_insts_.empty()) {
        removeDeadInsts();
        runOnFunction();
    }
}