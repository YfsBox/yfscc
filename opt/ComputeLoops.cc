//
// Created by 杨丰硕 on 2023/6/21.
//
#include <cassert>
#include <queue>
#include "ComputeLoops.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"

Instruction *ComputeLoops::LoopInfo::getSetCondInst() {
    auto rit = enter_block_->getInstructionList().rbegin();
    rit++;
    return rit->get();
}

PhiInstruction *ComputeLoops::LoopInfo::getCondVarPhiInst() {
    SetCondInstruction *setcond_inst = dynamic_cast<SetCondInstruction *>(getSetCondInst());
    assert(setcond_inst);

    auto lhs = setcond_inst->getLeft();
    auto rhs = setcond_inst->getRight();

    if (lhs->getValueType() == ConstantValue && dynamic_cast<PhiInstruction *>(rhs)) {
        return dynamic_cast<PhiInstruction *>(rhs);
    } else if (dynamic_cast<PhiInstruction *>(lhs) && rhs->getValueType() == ConstantValue) {
        return dynamic_cast<PhiInstruction *>(lhs);
    }

    return nullptr;
}

void ComputeLoops::dfsBasicBlocks(BasicBlock *basicblock, int index) {
    if (visited_blocks_.count(basicblock)) {
        return;
    }

    visited_blocks_.insert(basicblock);
    basicblock_index_[basicblock] = index;

    for (auto succ: basicblock->getSuccessorBlocks()) {
        dfsBasicBlocks(succ, index + 1);
    }

    for (auto pre_bb: basicblock->getPreDecessorBlocks()) {
        if (visited_blocks_.count(pre_bb) && basicblock_index_[pre_bb] > basicblock_index_[basicblock]) {
            auto doms = compute_dominators_->getDoms(pre_bb);
            if (doms.count(basicblock)) {
                auto loop = std::make_shared<LoopInfo>();
                loop->enter_block_ = basicblock;
                loop->exit_block_ = pre_bb;

                func_loopinfos_list_[function_].push_back(loop);
                computeLoopBody(loop);
            }
        }
    }

}

void ComputeLoops::computeLoopBody(const LoopInfoPtr &loopinfo) {
    std::unordered_set<BasicBlock *> visited_blocks;
    std::queue<BasicBlock *> bb_q;
    bb_q.push(loopinfo->exit_block_);

    while (!bb_q.empty()) {
        auto front_bb = bb_q.front();
        bb_q.pop();

        assert(front_bb);

        if (visited_blocks.count(front_bb) || front_bb == loopinfo->enter_block_) {
            continue;
        }

        visited_blocks.insert(front_bb);
        loopinfo->loop_body_.push_back(front_bb);

        if (loop_info_map_.find(front_bb) != loop_info_map_.end()) {
            loop_info_map_[front_bb]->parent_info_ = loopinfo;
        }
        loop_info_map_[front_bb] = loopinfo;
        visited_blocks.insert(front_bb);

        for (auto pred: front_bb->getPreDecessorBlocks()) {
            bb_q.push(pred);
        }
    }

}

void ComputeLoops::init() {
}

void ComputeLoops::run() {
    init();
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);
        printf("the function is %s\n", function->getName().c_str());
        if (function->getBlocks().size() < 1) {
            continue;
        }

        function_ = function;

        if (compute_dominators_) {
            compute_dominators_.release();
        }
        compute_dominators_ = std::make_unique<ComputeDominators>(function);
        compute_dominators_->run();

        assert(function->getBlocks().size() > 0);
        auto enter_block = function->getBlocks().front().get();
        visited_blocks_.clear();
        dfsBasicBlocks(enter_block, 0);
    }

    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);

        auto &loopinfo_list = func_loopinfos_list_[function];
        for (auto &loopinfo: loopinfo_list) {
            printf("the enter bb is %s, and exit bb is %s\n", loopinfo->enter_block_->getName().c_str(), loopinfo->exit_block_->getName().c_str());

            for (auto loopbody_bb: loopinfo->loop_body_) {
                printf("the body has %s\n", loopbody_bb->getName().c_str());
            }

            printf("\n");
        }

    }
}