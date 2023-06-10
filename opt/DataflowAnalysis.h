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
class IrDumper;

class DataflowAnalysis {
public:
    using BitSet = std::unordered_set<Value *>;

    using BitSetMap = std::unordered_map<BasicBlock *, BitSet>;

    explicit DataflowAnalysis(Function *function): function_(function) {}

    virtual ~DataflowAnalysis() = default;

    virtual void analysis() = 0;

    BitSetMap &getInSet() {
        return live_in_;
    }

    BitSetMap &getOutSet() {
        return live_out_;
    }

    IrDumper *irdumper_;

protected:
    static bool isEqual(const BitSet& left, const BitSet &right);

    Function *function_;

    BitSetMap live_in_;

    BitSetMap live_out_;

};

class LivenessAnalysis: public DataflowAnalysis {
public:

    explicit LivenessAnalysis(Function *function): DataflowAnalysis(function) {};

    ~LivenessAnalysis() = default;

    void analysis() override;

private:

    BitSetMap use_sets_;

    BitSetMap def_sets_;

};

class AvailExprsAnalysis: public DataflowAnalysis {
public:

    explicit AvailExprsAnalysis(Function *function): DataflowAnalysis(function) {}

    ~AvailExprsAnalysis() = default;

    void analysis() override;

private:

    BitSet egen_sets_;

    BitSet ekill_sets_;
};

class ReachingDefsAnalysis: public DataflowAnalysis {
public:

    explicit ReachingDefsAnalysis(Function *function): DataflowAnalysis(function) {}

    ~ReachingDefsAnalysis() = default;

    void analysis() override;

private:

    BitSet gen_sets_;

    BitSet kill_sets_;

};



#endif //YFSCC_LIVENESSANALYSIS_H
