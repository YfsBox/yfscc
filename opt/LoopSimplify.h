//
// Created by 杨丰硕 on 2023/8/18.
//

#ifndef YFSCC_LOOPSIMPLIFY_H
#define YFSCC_LOOPSIMPLIFY_H

#include "PassManager.h"
#include "ComputeLoops.h"
#include <memory>

class ComputeLoops;
class UserAnalysis;

// 这一部分包括循环内变量的强度削减, 一般放在循环展开之前使用
class LoopSimplify: public Pass {
public:

    explicit LoopSimplify(Module *module);

    ~LoopSimplify() = default;

protected:

    void runOnFunction() override;

private:

    std::list<Instruction *> isSimpleLoop(const std::shared_ptr<ComputeLoops::LoopInfo> &loopinfo);

    std::unique_ptr<ComputeLoops> compute_loops_;

    std::unique_ptr<UserAnalysis> user_analysis_;

};


#endif //YFSCC_LOOPSIMPLIFY_H
