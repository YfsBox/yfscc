//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_COMPUTELOOPS_H
#define YFSCC_COMPUTELOOPS_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <memory>
#include <list>
#include <vector>
#include "ComputeDominators.h"
#include "PassManager.h"

class Value;
class BasicBlock;
class BranchInstruction;
class Instruction;
class PhiInstruction;

class ComputeLoops {
public:

    struct LoopInfo {

        explicit LoopInfo(): enter_block_(nullptr), exit_block_(nullptr), parent_info_(nullptr), has_ret_or_break_(false) {}

        ~LoopInfo() = default;

        bool has_ret_or_break_;

        BasicBlock *enter_block_;

        BasicBlock *exit_block_;

        BasicBlock *next_block_;

        std::shared_ptr<LoopInfo> parent_info_;

        std::set<BasicBlock *> loop_body_;

        std::set<BasicBlock *> sub_loops_;

        std::map<PhiInstruction *, Value *> iterator_var_phi_insts_;

        Instruction *getSetCondInst();

        PhiInstruction *getCondVarPhiInst();

        void setHasReturnOrBreak();

        void setNextBasicBlock();

        void setInteratorVarPhiInsts();

        std::set<BasicBlock *> getSubLoops() {
            return sub_loops_;
        }

        bool isInLoop(BasicBlock *basicblock);
    };

    using LoopInfoPtr = std::shared_ptr<LoopInfo>;

    using LoopInfosList = std::vector<LoopInfoPtr>;

    explicit ComputeLoops(Module *module): module_(module), compute_dominators_(nullptr), function_(nullptr) {}

    ~ComputeLoops() = default;

    LoopInfo *getLoopInfo(BasicBlock *block) {
        return loop_info_map_[block].get();
    }

    LoopInfosList &getLoopInfosList(Function *function);

    LoopInfosList &getDeepestLoops(Function *function);

    void run();

private:

    void init();

    void dfsBasicBlocks(BasicBlock *basicblock, int index);

    void computeLoopBody(const LoopInfoPtr &loopinfo);

    void setDeepestLoops(Function *function);

    Module *module_;

    std::unique_ptr<ComputeDominators> compute_dominators_;

    Function *function_;

    std::unordered_set<BasicBlock *> visited_blocks_;

    std::unordered_map<BasicBlock *, LoopInfoPtr> loop_info_map_;

    std::unordered_map<Function *, LoopInfosList> deepest_loops_;

    std::unordered_map<BasicBlock *, int> basicblock_index_;

    std::unordered_map<Function *, LoopInfosList> func_loopinfos_list_;

};


#endif //YFSCC_COMPUTELOOPS_H
