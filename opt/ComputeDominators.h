//
// Created by 杨丰硕 on 2023/6/10.
//

#ifndef YFSCC_COMPUTEDOMINATORS_H
#define YFSCC_COMPUTEDOMINATORS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>

class BasicBlock;
class Function;

class ComputeDominators {
public:
    using Matrix = std::vector<std::vector<int8_t>>;

    using BasicBlockSet = std::unordered_set<BasicBlock *>;

    using DomsSet = std::unordered_map<BasicBlock *, BasicBlockSet>;

    using BasicblockMap = std::unordered_map<BasicBlock *, BasicBlock *>;

    using BasicBlockIntValueMap = std::unordered_map<BasicBlock *, int>;
    
    explicit ComputeDominators(Function *function): curr_func_(function), basicblock_n_(0), top_basicblock_(nullptr) {}

    ~ComputeDominators() = default;

    DomsSet getDomTree() const {
        return dom_tree_;
    }

    BasicBlockSet getBasicBlockDoms(BasicBlock *bb) {
        return basicblock_doms_[bb];
    }

    BasicBlockSet getDomFrontiers(BasicBlock *bb) {
        return dom_frontiers_map_[bb];
    }

    BasicBlock *getImmDomsMap(BasicBlock *bb) {
        return imm_doms_map_[bb];
    }

    BasicBlockSet getSuccessors(BasicBlock *bb) {
        return basicblock_succbb_map_[bb];
    }

    BasicBlockSet getDoms(BasicBlock *basicblock) {
        return doms_map_[basicblock];
    }

    void run();

private:

    static bool isEqual(const BasicBlockSet &seta, const BasicBlockSet &setb);

    void getPostOrderList(BasicBlock *bb);

    void clearSets();

    void initForBasicBlockIndexMap();

    void initForMatrix();

    BasicBlock *intersect(BasicBlock *bb1, BasicBlock *bb2);

    void computeImmDoms();

    void computeFrontiers();

    void computeSuccessors();

    void computeDoms();

    int32_t basicblock_n_;

    Matrix dominators_matrix_;

    DomsSet dom_tree_;

    DomsSet basicblock_doms_;

    BasicblockMap imm_doms_map_;

    DomsSet dom_frontiers_map_;

    DomsSet doms_map_;

    DomsSet basicblock_succbb_map_;

    BasicBlock *top_basicblock_;

    Function *curr_func_;

    std::unordered_map<int, BasicBlock *> index2basicblock_map_;

    std::list<BasicBlock *> post_order_list_;

    std::unordered_set<BasicBlock *> visited_bb_set_;

    std::unordered_map<BasicBlock *, int> post_order_index_map_;

    std::unordered_map<BasicBlock *, int> doms_depth_map_;

    BasicBlockIntValueMap basicblock2index_map_;

    BasicBlockIntValueMap basicblock_dom_depth_map_;

};


#endif //YFSCC_COMPUTEDOMINATORS_H
