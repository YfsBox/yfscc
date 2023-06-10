//
// Created by 杨丰硕 on 2023/6/10.
//

#ifndef YFSCC_COMPUTEDOMINATORS_H
#define YFSCC_COMPUTEDOMINATORS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"

class BasicBlock;

class ComputeDominators: public Pass {
public:
    using Matrix = std::vector<std::vector<int8_t>>;

    using DomsSet = std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>>;

    using BasicblockMap = std::unordered_map<BasicBlock *, BasicBlock *>;

    using BasicBlockIntValueMap = std::unordered_map<BasicBlock *, int>;
    
    explicit ComputeDominators(Module *module);

    ~ComputeDominators() = default;

    DomsSet getDomTree() const {
        return dom_tree_;
    }

    DomsSet getBasicBlockDoms() const {
        return basicblock_doms_;
    }

    BasicblockMap getImmDomsMap() const {
        return imm_doms_map_;
    }

protected:

    void runOnFunction() override;

private:

    void clearSets();

    void initForBasicBlockIndexMap();

    void initForMatrix();

    int32_t basicblock_n_;

    Matrix dominators_matrix_;

    DomsSet dom_tree_;

    DomsSet basicblock_doms_;

    BasicblockMap imm_doms_map_;

    BasicBlock *top_basicblock_;

    std::unordered_map<int, BasicBlock *> index2basicblock_map_;

    BasicBlockIntValueMap basicblock2index_map_;

    BasicBlockIntValueMap basicblock_dom_depth_map_;

};


#endif //YFSCC_COMPUTEDOMINATORS_H
