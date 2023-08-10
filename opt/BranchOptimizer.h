//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_BRANCHOPTIMIZER_H
#define YFSCC_BRANCHOPTIMIZER_H

#include <unordered_set>
#include "PassManager.h"

class BasicBlock;
class BranchInstruction;

class BranchOptimizer: public Pass {
public:

    explicit BranchOptimizer(Module *module): Pass(module) {
        pass_name_ = "BranchOptimizer";
    }

    ~BranchOptimizer() = default;

protected:

    void runOnFunction() override;

private:

    bool findRedundantAndReplace(BranchInstruction *br_inst, BasicBlock *target);

    std::unordered_set<BranchInstruction *> redundant_brinsts_;

    std::unordered_set<BasicBlock *> been_phi_use_set_;

};


#endif //YFSCC_BRANCHOPTIMIZER_H
