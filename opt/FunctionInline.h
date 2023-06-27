//
// Created by 杨丰硕 on 2023/6/14.
//

#ifndef YFSCC_FUNCTIONINLINE_H
#define YFSCC_FUNCTIONINLINE_H

#include <memory>
#include <list>
#include <set>
#include <unordered_map>
#include "CallGraphAnalysis.h"
#include "PassManager.h"
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"

class CallInstruction;

class FunctionInline: public Pass {
public:

    using BasicBlockUptr = std::unique_ptr<BasicBlock>;

    explicit FunctionInline(Module *module);

    ~FunctionInline() = default;

protected:
    void runOnFunction() override;  // 这里的runOnFunction并不是针对某个function的，而是整个module的所有
private:

    static const constexpr int32_t inline_insts_size = 200;

    bool canBeInline(Function *function);

    void inlineOnFunction();

    void collectCaninlineFunctions();

    void collectCallPoint();

    void computeCallCnt();

    void initForFunction();

    void preSetCopyMap(CallInstruction *call_inst);

    void generateBasicBlocks(Function *inlined_function, CallInstruction *call_inst, std::list<BasicBlock *> &bbs_list);

    Value *getCopyValue(Value *value);

    BasicBlock *dfsSetCopyMap(BasicBlock *basic_block);

    void setForUnfinishedCopyValue(Function *inlined_func);

    void replaceWithNextBasicBlock();

    static int inline_point_cnt_;

    bool has_collect_caninline_functions_;

    BasicBlock *curr_caller_basicblock_;

    BasicBlock *curr_inlined_exit_basicblock_;

    std::unordered_map<Function *, int> call_cnt_map_;

    std::unique_ptr<CallGraphAnalysis> call_graph_analysis_;

    std::unordered_set<CallInstruction *> call_insts_for_inline_;

    std::unordered_map<BasicBlock *, Function::BasicBlocksIt> insert_point_nextit_map_;

    std::unordered_map<Value *, Value *> copy_value_map_;

    std::unordered_map<Function *, std::unordered_set<BasicBlock *>> copy_exit_blocks_;

    std::unordered_map<BasicBlock *, Value *> exitblock_retvalue_map_;

    std::unordered_map<BasicBlock *, BasicBlock *> next_block_map_;

    std::unordered_set<BasicBlock *> visited_basicblocks_;

    std::set<Instruction *> inlined_alloca_insts_;

    std::unordered_set<Function *> caninline_functions_;

};


#endif //YFSCC_FUNCTIONINLINE_H
