//
// Created by 杨丰硕 on 2023/6/10.
//
#ifndef YFSCC_INSTCOMBINE_H
#define YFSCC_INSTCOMBINE_H

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "GlobalAnalysis.h"
#include "PassManager.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"

class BinaryOpInstruction;
class IrDumper;
class Value;

class InstCombine: public Pass {
public:

    explicit InstCombine(Module *module): Pass(module), curr_insert_inst_(nullptr) {
        pass_name_ = "InstCombine";
    }

    ~InstCombine() = default;

protected:

    void runOnFunction() override;

private:

    void initWorkList();

    void combineWithConst(BinaryOpInstruction *inst);

    void combineNonConst(BinaryOpInstruction *inst);

    bool canCombineWithConst(BinaryOpInstruction *inst);

    bool canCombineNonConst(BinaryOpInstruction *inst, Value **value = nullptr);

    BinaryOpInstruction *curr_insert_inst_;

    std::unordered_set<BinaryOpInstruction *> work_insts_withconst_set_;

    std::unordered_map<BinaryOpInstruction *, Value *> work_insts_nonconst_map_;

    std::unordered_set<BinaryOpInstruction *> combined_insts_withconst_set_;

    std::unordered_set<BinaryOpInstruction *> combined_insts_nonconst_set_;

    std::unordered_map<BinaryOpInstruction *, BinaryOpInstruction *> inserted_inst_;

    std::unordered_map<BinaryOpInstruction *, BasicBlock::InstructionListIt> inserted_it_;

    std::unordered_map<Value *, int> combine_value_cnt_;
};

#endif //YFSCC_INSTCOMBINE_H
