//
// Created by 杨丰硕 on 2023/6/10.
//
#include <algorithm>
#include <queue>
#include "ComputeDominators.h"
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"

void ComputeDominators::clearSets() {
    dom_tree_.clear();
    basicblock_doms_.clear();
    imm_doms_map_.clear();
    index2basicblock_map_.clear();
    basicblock2index_map_.clear();
    basicblock_dom_depth_map_.clear();
}

void ComputeDominators::getPostOrderList(BasicBlock *bb) {
    visited_bb_set_.insert(bb);
    for (auto succ_bb: bb->getSuccessorBlocks()) {
        if (!visited_bb_set_.count(succ_bb)) {
            getPostOrderList(succ_bb);
        }
    }
    post_order_index_map_[bb] = post_order_list_.size();
    post_order_list_.push_back(bb);
}


void ComputeDominators::initForBasicBlockIndexMap() {
    int index = 0;
    for (auto &bb: curr_func_->getBlocks()) {
        index2basicblock_map_[index] = bb.get();
        basicblock2index_map_[bb.get()] = index;
        index++;
    }
}


bool ComputeDominators::isEqual(const BasicBlockSet &seta, const BasicBlockSet &setb) {
    if (seta.size() != setb.size()) {
        return false;
    }
    for (auto &bb: seta) {
        if (!setb.count(bb)) {
            return false;
        }
    }
    return true;
}

void ComputeDominators::computeDoms() {
    std::unordered_set<BasicBlock *> init_set;
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        init_set.insert(bb_uptr.get());
    }

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        doms_map_[bb_uptr.get()] = init_set;
    }

    auto enter_block = curr_func_->getBlocks().front().get();
    doms_map_[enter_block].clear();
    doms_map_[enter_block].insert(enter_block);

    bool has_changed = true;
    while (has_changed) {
        has_changed = false;

        for (auto &bb_uptr: curr_func_->getBlocks()) {
            std::unordered_set<BasicBlock *> temp_set;
            temp_set.insert(bb_uptr.get());

            std::unordered_map<BasicBlock *, int> cnt_map;
            int intersect_cnt = bb_uptr->getPreDecessorBlocks().size();
            for (auto &pre: bb_uptr->getPreDecessorBlocks()) {
                for (auto pre_dom: doms_map_[pre]) {
                    cnt_map[pre_dom]++;
                    if (cnt_map[pre_dom] == intersect_cnt) {
                        temp_set.insert(pre_dom);
                    }
                }
            }

            if (!isEqual(temp_set, doms_map_[bb_uptr.get()])) {
                doms_map_[bb_uptr.get()] = temp_set;
                has_changed = true;
            }

        }
    }
}

void ComputeDominators::run() {

    clearSets();

    auto &blocks_list = curr_func_->getBlocks();
    basicblock_n_ = blocks_list.size();
    top_basicblock_ = blocks_list.front().get();
    auto enter_bb = curr_func_->getBlocks().front().get();
    getPostOrderList(enter_bb);

    initForBasicBlockIndexMap();

    computeImmDoms();
    computeDoms();
    computeFrontiers();
    computeSuccessors();
    computeDomTreeDepth();

}

BasicBlock *ComputeDominators::intersect(BasicBlock *bb1, BasicBlock *bb2) {
    auto finger1 = bb1;
    auto finger2 = bb2;
    while (finger1 != finger2) {
        while (post_order_index_map_[finger1] < post_order_index_map_[finger2]) {
            finger1 = imm_doms_map_[finger1];
        }
        while (post_order_index_map_[finger1] > post_order_index_map_[finger2]) {
            finger2 = imm_doms_map_[finger2];
        }
    }
    return finger1;
}

void ComputeDominators::computeImmDoms() {

    for (auto &bb: curr_func_->getBlocks()) {
        imm_doms_map_[bb.get()] = nullptr;
    }
    auto enter_block = curr_func_->getBlocks().front().get();
    // printf("the enter bb is %s, the bb size is %d\n", enter_block->getName().c_str(), curr_func_->getBlocks().size());
    imm_doms_map_[enter_block] = enter_block;

    bool changed = true;
    while (changed) {
        changed = false;
        auto &bb_list = curr_func_->getBlocks();
        for (auto it = post_order_list_.begin(); it != post_order_list_.end(); ++it) {
            auto curr_bb = *it;
            if (curr_bb == enter_block) {
                continue;
            }
            BasicBlock *pred = nullptr;
            for (auto pre: curr_bb->getPreDecessorBlocks()) {
                if (imm_doms_map_[pre]) {
                    pred = pre;
                    break;
                }
            }

            BasicBlock *new_idom = pred;
            for (auto p: curr_bb->getPreDecessorBlocks()) {
                if (p == pred) {
                    continue;
                }
                if (imm_doms_map_[p]) {
                    new_idom = intersect(p, new_idom);
                }
            }

            if (imm_doms_map_[curr_bb] != new_idom) {
                imm_doms_map_[curr_bb] = new_idom;
                changed = true;
            }
        }
    }
}

void ComputeDominators::computeDomTreeDepth() {
    auto enter_block = curr_func_->getBlocks().front().get();
    std::unordered_set<BasicBlock *> visited_blocks;
    std::queue<BasicBlock *> blockq;
    blockq.push(enter_block);
    doms_depth_map_[enter_block] = 0;
    visited_blocks.insert(enter_block);

    while (!blockq.empty()) {
        auto block = blockq.front();
        blockq.pop();
        for (auto succ: basicblock_succbb_map_[block]) {
            if (!visited_blocks.count(succ)) {
                visited_blocks.insert(succ);
                blockq.push(succ);
                doms_depth_map_[succ] = doms_depth_map_[block] + 1;
            }
        }
    }
}

void ComputeDominators::computeFrontiers() {
    for (auto &bb: curr_func_->getBlocks()) {
        if (bb->getPreDecessorBlocks().size() >= 2) {
            for (auto p: bb->getPreDecessorBlocks()) {
                auto runner = p;
                while (runner != imm_doms_map_[bb.get()]) {
                    dom_frontiers_map_[runner].insert(bb.get());
                    runner = imm_doms_map_[runner];
                }
            }
        }
    }
}

void ComputeDominators::computeSuccessors() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto i_dom = imm_doms_map_[bb.get()];
        if (i_dom != bb.get()) {
            basicblock_succbb_map_[i_dom].insert(bb.get());
        }
    }
}


