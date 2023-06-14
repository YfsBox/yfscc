//
// Created by 杨丰硕 on 2023/6/9.
//

#ifndef YFSCC_DEADCODEELIM_H
#define YFSCC_DEADCODEELIM_H

#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"

class Value;
class Instruction;
class IrDumper;

class DeadCodeElim: public Pass {
public:

    explicit DeadCodeElim(Module *module): Pass(module) {}

    ~DeadCodeElim() = default;

protected:

    void runOnFunction() override;

private:

    void removeDeadInsts();

    bool hasSideEffect(Instruction *inst);

    std::unordered_set<Instruction *> useful_insts_;

    std::unordered_set<Instruction *> dead_insts_;

};


#endif //YFSCC_DEADCODEELIM_H
