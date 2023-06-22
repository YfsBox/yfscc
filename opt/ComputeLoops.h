//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_COMPUTELOOPS_H
#define YFSCC_COMPUTELOOPS_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <memory>
#include <list>
#include <vector>
#include "ComputeDominators.h"
#include "PassManager.h"

class BasicBlock;
class BranchInstruction;

class ComputeLoops {
public:

    struct LoopInfo {

        explicit LoopInfo(): enter_block_(nullptr), exit_block_(nullptr), parent_info_(nullptr) {}

        ~LoopInfo() = default;

        BasicBlock *enter_block_;

        BasicBlock *exit_block_;

        std::shared_ptr<LoopInfo> parent_info_;

        std::list<BasicBlock *> loop_body_;

        Instruction *getSetCondInst();

        PhiInstruction *getCondVarPhiInst();

    };

    using LoopInfoPtr = std::shared_ptr<LoopInfo>;

    using LoopInfosList = std::vector<LoopInfoPtr>;

    explicit ComputeLoops(Module *module): module_(module), compute_dominators_(nullptr), function_(nullptr) {}

    ~ComputeLoops() = default;

    LoopInfo *getLoopInfo(BasicBlock *block) {
        return loop_info_map_[block].get();
    }

    void run();

private:

    void init();

    void dfsBasicBlocks(BasicBlock *basicblock, int index);

    void computeLoopBody(const LoopInfoPtr &loopinfo);

    Module *module_;

    std::unique_ptr<ComputeDominators> compute_dominators_;

    Function *function_;

    std::unordered_set<BasicBlock *> visited_blocks_;

    std::unordered_map<BasicBlock *, LoopInfoPtr> loop_info_map_;

    std::unordered_map<BasicBlock *, int> basicblock_index_;

    std::unordered_map<Function *, LoopInfosList> func_loopinfos_list_;

};


#endif //YFSCC_COMPUTELOOPS_H
