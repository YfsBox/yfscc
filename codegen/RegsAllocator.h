//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_REGSALLOCATOR_H
#define YFSCC_REGSALLOCATOR_H

#include <unordered_set>
#include "Machine.h"
#include "MachineOperand.h"

struct Interval {
    int32_t start_;
    int32_t end_;
    MachineOperand *operand_;
};

class RegsAllocator {
public:
    using BitSet = std::unordered_set<MachineOperand *>;

    using BitSetVec = std::unordered_map<MachineBasicBlock *, BitSet>;

    RegsAllocator(MachineModule *module): machine_module_(module) {}

    ~RegsAllocator() = default;

private:
    void analyseLiveness(MachineFunction *function);

    static bool isEqual(const BitSet &lhs, const BitSet &rhs);

    MachineModule *machine_module_;

    BitSetVec live_in_;

    BitSetVec live_out_;

    BitSetVec use_sets_;

    BitSetVec def_sets_;

};

#endif //YFSCC_REGSALLOCATOR_H
