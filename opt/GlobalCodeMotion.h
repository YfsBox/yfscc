//
// Created by 杨丰硕 on 2023/6/27.
//

#ifndef YFSCC_GLOBALCODEMOTION_H
#define YFSCC_GLOBALCODEMOTION_H

#include "ComputeDominators.h"
#include "ComputeLoops.h"
#include "PassManager.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>

class Instruction;
class BasicBlock;
class UserAnalysis;

class GlobalCodeMotion: public Pass {       // 一般紧接着在Svn的后面处理
public:
    using InstsSet = std::unordered_set<Instruction *>;

    explicit GlobalCodeMotion(Module *module);

    ~GlobalCodeMotion() = default;

protected:

    void runOnFunction() override;

private:

    bool isPinned(const Value *value);

    void scheduleLate(Instruction *value);

    void scheduleEarly(Instruction *value);

    BasicBlock *findLCA(BasicBlock *blocka, BasicBlock *blockb);

    void moveInsts();

    void collectPinnedValues();

    bool has_compute_loop_;

    BasicBlock *root_block_in_currfunc_;

    std::unique_ptr<UserAnalysis> user_analysis_;

    std::unique_ptr<ComputeLoops> compute_loops_;

    std::unique_ptr<ComputeDominators> compute_doms_;

    std::unordered_set<Value *> visited_insts_;

    std::unordered_map<Value *, BasicBlock *> inst_block_map_;

    std::unordered_map<Value *, BasicBlock *> best_block_map_;

    InstsSet wait_move_insts_set_;

    std::list<Value *> pinned_values_;

    std::unordered_map<BasicBlock *, std::list<Instruction *>> move_insts_map_;
};


#endif //YFSCC_GLOBALCODEMOTION_H
