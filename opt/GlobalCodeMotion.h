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

class Instruction;
class BasicBlock;

class GlobalCodeMotion: public Pass {       // 一般紧接着在Svn的后面处理
public:
    using InstsSet = std::unordered_set<Instruction *>;

    explicit GlobalCodeMotion(Module *module);

    ~GlobalCodeMotion() = default;

protected:

    void runOnFunction() override;

private:

    bool isPinned(const Instruction *inst);

    void scheduleLate(Instruction *inst);

    void scheduleEarly(Instruction *inst);

    void collectUserSets();    // 因为ir中的user接口存在问题，所以在这里一个map收集

    BasicBlock *findLCA(BasicBlock *blocka, BasicBlock *blockb);

    void moveInsts();

    bool has_compute_loop_;

    BasicBlock *root_block_in_currfunc_;

    std::unique_ptr<ComputeLoops> compute_loops_;

    std::unique_ptr<ComputeDominators> compute_doms_;

    std::unordered_set<Instruction *> visited_insts_;

    std::unordered_map<Instruction *, BasicBlock *> inst_block_map_;

    std::unordered_map<Instruction *, InstsSet> user_insts_map_;        // 因为ir中的user接口存在问题，所以在这里一个map收集

    std::unordered_map<Instruction *, BasicBlock *> best_block_map_;

    InstsSet wait_move_insts_set_;

    std::unordered_map<BasicBlock *, std::list<Instruction *>> move_insts_map_;
};


#endif //YFSCC_GLOBALCODEMOTION_H
