//
// Created by 杨丰硕 on 2023/7/30.
//

#ifndef YFSCC_DEADFUNCTIONELIM_H
#define YFSCC_DEADFUNCTIONELIM_H

#include "PassManager.h"
#include "CallGraphAnalysis.h"
#include <memory>

class Function;

class DeadFunctionElim: public Pass {
public:

    explicit DeadFunctionElim(Module *module);

    ~DeadFunctionElim() = default;

protected:

    void runOnFunction() override;

private:

    void init();

    bool has_init_;

    std::unique_ptr<CallGraphAnalysis> call_graph_analysis_;

    std::unordered_set<Function *> not_dead_functions_;

};


#endif //YFSCC_DEADFUNCTIONELIM_H
