//
// Created by 杨丰硕 on 2023/6/10.
//
#ifndef YFSCC_INSTCOMBINE_H
#define YFSCC_INSTCOMBINE_H

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"

class Instruction;
class BinaryOpInstruction;
class IrDumper;

class InstCombine: public Pass {
public:

    explicit InstCombine(Module *module): Pass(module) {}

    ~InstCombine() = default;

protected:

    void runOnFunction() override;

private:

    void initWorkList();

    void combine(BinaryOpInstruction *inst);

    bool canCombine(BinaryOpInstruction *inst);

    std::unordered_set<BinaryOpInstruction *> work_insts_set_;

    std::unordered_set<BinaryOpInstruction *> combined_insts_set_;
};

#endif //YFSCC_INSTCOMBINE_H
