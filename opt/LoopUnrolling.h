//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_LOOPUNROLLING_H
#define YFSCC_LOOPUNROLLING_H

#include <memory>
#include "PassManager.h"
#include "ComputeLoops.h"

class LoopUnrolling: public Pass {
public:

    explicit LoopUnrolling(Module *module): Pass(module), has_compute_loop_(false), compute_loops_(nullptr) {}

    ~LoopUnrolling() = default;

protected:

    void runOnFunction() override;

private:

    struct LoopUnrollingInfo {

        explicit LoopUnrollingInfo(const ComputeLoops::LoopInfoPtr &loopinfo): loopinfo_(loopinfo) {}

        ~LoopUnrollingInfo() = default;

        ComputeLoops::LoopInfoPtr loopinfo_;

        int32_t limit_;

        int32_t stride_;

        int32_t init_value_;

        int32_t cal_iteratorions_cnt_;
    };

    bool isFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo, LoopUnrollingInfo &unrolling_info) const;

    bool isNotFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo) const;

    void unroll(ComputeLoops::LoopInfoPtr &loopinfo);

    bool has_compute_loop_;

    static const constexpr int unrolling_cnt = 4;

    std::unique_ptr<ComputeLoops> compute_loops_;

};


#endif //YFSCC_LOOPUNROLLING_H
