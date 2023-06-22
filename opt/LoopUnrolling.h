//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_LOOPUNROLLING_H
#define YFSCC_LOOPUNROLLING_H

#include <memory>
#include "PassManager.h"
#include "ComputeLoops.h"
#include "../ir/BasicBlock.h"

class Value;
class BinaryOpInstruction;

class LoopUnrolling: public Pass {
public:

    explicit LoopUnrolling(Module *module): Pass(module), has_compute_loop_(false), curr_condition_var_(nullptr), compute_loops_(nullptr) {}

    ~LoopUnrolling() = default;

protected:

    void runOnFunction() override;

private:

    struct LoopUnrollingInfo {

        explicit LoopUnrollingInfo(const ComputeLoops::LoopInfoPtr &loopinfo): loopinfo_(loopinfo), iterator_inst(nullptr) {}

        ~LoopUnrollingInfo() = default;

        ComputeLoops::LoopInfoPtr loopinfo_;

        Instruction *iterator_inst;

        int32_t limit_;

        int32_t stride_;

        int32_t init_value_;

        int32_t cal_iteratorions_cnt_;

    };

    Instruction *getCopyInstruction(Instruction *inst, BasicBlock *basicblock, const std::string &new_name);

    Value *getCopyValue(Value *value);

    void copyOneBasicBlockForFullUnroll(const std::list<Instruction *> &origin_insts, BasicBlock *basicblock, int32_t unroll_index, const LoopUnrollingInfo &unrolling_info);

    bool isFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo, LoopUnrollingInfo &unrolling_info) const;

    bool isNotFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo) const;

    void unroll(ComputeLoops::LoopInfoPtr &loopinfo);

    bool has_compute_loop_;

    Value *curr_condition_var_;

    static const constexpr int unrolling_cnt = 4;

    std::unique_ptr<ComputeLoops> compute_loops_;

    std::unordered_map<Value *, Value *> copy_insts_map_;


};


#endif //YFSCC_LOOPUNROLLING_H
