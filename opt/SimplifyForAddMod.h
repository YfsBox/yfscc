//
// Created by 杨丰硕 on 2023/8/21.
//

#ifndef YFSCC_SIMPLIFYFORADDMOD_H
#define YFSCC_SIMPLIFYFORADDMOD_H

#include "PassManager.h"
#include "ComputeLoops.h"
#include "GlobalAnalysis.h"
#include <list>

// 针对的是形如:
// while(i < n) { sum += 4950; sum %= 65535; i = i + 1; }
// 直接可以将循环替换为O(1)的表达式

class SimplifyForAddMod: public Pass {
public:

    explicit SimplifyForAddMod(Module *module);

    ~SimplifyForAddMod() = default;

protected:

    void runOnFunction() override;

private:

    std::list<Instruction *> simpleLoopAnalysis(const std::shared_ptr<ComputeLoops::LoopInfo> &loopinfo);

    std::unique_ptr<ComputeLoops> compute_loops_;

    std::unique_ptr<UserAnalysis> user_analysis_;
};


#endif //YFSCC_SIMPLIFYFORADDMOD_H
