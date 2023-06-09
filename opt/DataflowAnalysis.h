//
// Created by 杨丰硕 on 2023/5/12.
//

#ifndef YFSCC_LIVENESSANALYSIS_H
#define YFSCC_LIVENESSANALYSIS_H

#include <unordered_set>
#include <unordered_map>
#include "PassManager.h"

class Value;
class Function;
class BasicBlock;

class LivenessAnalysis {
public:
    using BitSet = std::unordered_set<Value *>;

    using BitSetMap = std::unordered_map<BasicBlock *, BitSet>;

    LivenessAnalysis(Function *function): function_(function) {};

    ~LivenessAnalysis() = default;

    void analysis();

    BitSetMap getLiveInSet() const {
        return live_in_;
    }

    BitSetMap getLiveOutSet() const {
        return live_out_;
    }

private:
    static bool isEqual(const BitSet& left, const BitSet &right);

    Function *function_;

    BitSetMap live_in_;

    BitSetMap live_out_;

    BitSetMap use_sets_;

    BitSetMap def_sets_;
};

#endif //YFSCC_LIVENESSANALYSIS_H
