//
// Created by 杨丰硕 on 2023/6/9.
//

#ifndef YFSCC_DEADCODEELIM_H
#define YFSCC_DEADCODEELIM_H

#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"
#include "CallGraphAnalysis.h"
#include "GlobalAnalysis.h"

class Value;
class Instruction;
class IrDumper;
class UserAnalysis;

class DeadCodeElim: public Pass {
public:

    explicit DeadCodeElim(Module *module);

    ~DeadCodeElim() = default;

protected:

    void runOnFunction() override;

private:

    void removeDeadInsts();

    bool hasSideEffect(Instruction *inst);

    std::unique_ptr<UserAnalysis> user_analysis_;

    std::unique_ptr<CallGraphAnalysis> call_graph_analysis_;

    std::unordered_set<Instruction *> useful_insts_;

    std::unordered_set<Instruction *> dead_insts_;

};


#endif //YFSCC_DEADCODEELIM_H
