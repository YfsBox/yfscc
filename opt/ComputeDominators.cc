//
// Created by 杨丰硕 on 2023/6/10.
//
#include <queue>
#include "ComputeDominators.h"
#include "../ir/BasicBlock.h"

ComputeDominators::ComputeDominators(Module *module): Pass(module) {}

void ComputeDominators::clearSets() {
    dominators_matrix_.clear();
    dom_tree_.clear();
    basicblock_doms_.clear();
    imm_doms_map_.clear();
    index2basicblock_map_.clear();
    basicblock2index_map_.clear();
    basicblock_dom_depth_map_.clear();
}

void ComputeDominators::initForBasicBlockIndexMap() {
    int index = 0;
    for (auto &bb: curr_func_->getBlocks()) {
        index2basicblock_map_[index] = bb.get();
        basicblock2index_map_[bb.get()] = index;
        index++;
    }
}

void ComputeDominators::initForMatrix() {
    assert(basicblock2index_map_[top_basicblock_] == 0);
    for (int i = 0; i < basicblock_n_; ++i) {
        dominators_matrix_[0][i] = 0;
    }
    dominators_matrix_[0][0] = 1;

    std::vector<int8_t> tmp_bitset(basicblock_n_);
    bool has_changed = true;
    while (has_changed) {
        has_changed = false;
        for (int i = basicblock_n_ - 1; i >= 0; i--) {
            if (i == 0) {
                continue;
            }
            for (int j = 0; j < basicblock_n_; ++j) {
                tmp_bitset[j] = 1;
            }

            auto bbs = index2basicblock_map_[i]->getPreDecessorBlocks();
            for (auto bb : bbs) {
                int index = basicblock2index_map_[bb];
                for (int m = 0; m < basicblock_n_; ++m) {
                    tmp_bitset[m] &= dominators_matrix_[index][m];
                }
            }

            tmp_bitset[i] = 1;
            bool is_same = true;

            for (int j = 0; j < basicblock_n_; ++j) {
                if (tmp_bitset[j] != dominators_matrix_[i][j]) {
                    is_same = false;
                }
                dominators_matrix_[i][j] = tmp_bitset[j];
            }

            if (!is_same) {
                has_changed = true;
            }
        }
    }
}

void ComputeDominators::runOnFunction() {

    clearSets();

    auto &blocks_list = curr_func_->getBlocks();
    basicblock_n_ = blocks_list.size();
    top_basicblock_ = blocks_list.front().get();
    for (int i = 0; i < basicblock_n_; ++i) {
        std::vector<int8_t> line(basicblock_n_, 1);
        dominators_matrix_.push_back(line);
    }

    initForBasicBlockIndexMap();
    initForMatrix();

    for (auto &bb : curr_func_->getBlocks()) {
        auto bb_index = basicblock2index_map_[bb.get()];
        for (int i = 0; i < basicblock_n_; ++i) {
            if (dominators_matrix_[bb_index][i]) {
                basicblock_doms_[bb.get()].insert(index2basicblock_map_[i]);
            }
        }
    }

    for (int i = basicblock_n_ - 1; i >= 0; i--) {
        auto bb1 = index2basicblock_map_[i];
        auto bb1_doms = basicblock_doms_[bb1];

        for (int j = basicblock_n_ - 1; j >= 0; j--) {
            if (i == j) {
                continue;
            }

            auto bb2 = index2basicblock_map_[j];
            auto bb2_doms = basicblock_doms_[bb2];
            if (bb1_doms.size() - 1 == bb2_doms.size()) {
                bool same = true;
                for (auto dom: bb1_doms) {
                    if (dom == bb1) {
                        continue;
                    }
                    if (!bb2_doms.count(bb1)) {
                        same = false;
                        break;
                    }
                }

                if (same) {
                    imm_doms_map_[bb1] = bb2;
                    dom_tree_[bb2].insert(bb1);
                }
            }
        }
    }
    // 计算dom depth的深度
    // std::queue<BasicBlock *> q;
    // q.push();
    auto start_block = index2basicblock_map_[0];
    std::queue<BasicBlock *> bb_q;
    bb_q.push(start_block);
    std::unordered_map<BasicBlock *, bool> visited_map;

    for (int i = 0; i < basicblock_n_; ++i) {
        auto bb = index2basicblock_map_[i];
        visited_map[bb] = false;
    }

    basicblock_dom_depth_map_[start_block] = 0;
    while (!bb_q.empty()) {
        auto bb = bb_q.front();
        bb_q.pop();

        for (auto child_bb: dom_tree_[bb]) {
            if (!visited_map[child_bb]) {
                visited_map[child_bb] = true;
                basicblock_dom_depth_map_[child_bb] = basicblock_dom_depth_map_[bb] + 1;
                bb_q.push(child_bb);
            }
        }
    }

}


