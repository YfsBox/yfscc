//
// Created by 杨丰硕 on 2023/7/1.
//

#ifndef YFSCC_SIMPLIFYPHIINSTS_H
#define YFSCC_SIMPLIFYPHIINSTS_H

#include "PassManager.h"
#include <unordered_map>
#include <unordered_set>

class Value;
class PhiInstruction;
class Instruction;

class SimplifyPhiInsts: public Pass {
public:

    using InstsSet = std::unordered_set<Instruction *>;

    explicit SimplifyPhiInsts(Module *module): Pass(module) {}

    ~SimplifyPhiInsts() = default;

protected:

    void runOnFunction() override;

private:

    void collectUserSets();

    std::unordered_map<Value *, InstsSet> user_insts_map_;

    std::unordered_map<PhiInstruction *, Value *> replace_insts_map_;

};


#endif //YFSCC_SIMPLIFYPHIINSTS_H
