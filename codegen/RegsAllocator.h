//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_REGSALLOCATOR_H
#define YFSCC_REGSALLOCATOR_H

#include <unordered_set>
#include <unordered_map>
#include "Machine.h"
#include "MachineOperand.h"

class RegsAllocator {
public:
    using BitSet = std::unordered_set<MachineOperand *>;

    using BitSetVec = std::unordered_map<MachineBasicBlock *, BitSet>;

    using AdjEdge = std::pair<MachineOperand *, MachineOperand *>;

    RegsAllocator(MachineModule *module): machine_module_(module) {}

    ~RegsAllocator() = default;

private:
    void analyseLiveness(MachineFunction *function);

    void build();

    void addEdge(MachineOperand *a, MachineOperand *b);

    bool isInAdjSet(MachineOperand *a, MachineOperand *b);

    bool isPrecolored(MachineOperand *operand);

    static bool isEqual(const BitSet &lhs, const BitSet &rhs);

    MachineModule *machine_module_;

    MachineFunction *curr_function_;

    BitSetVec live_in_;

    BitSetVec live_out_;

    BitSetVec use_sets_;

    BitSetVec def_sets_;

    std::vector<MachineOperand *> pre_colored_;

    // some data structure about graph
    std::unordered_map<MachineOperand *, std::unordered_set<MachineInst *>> move_list_;

    std::unordered_set<MachineInst *> worklist_moves_;

    std::unordered_map<MachineOperand *, std::unordered_set<MachineOperand *>> adj_set_;

    std::unordered_map<MachineOperand *, std::unordered_set<MachineOperand *>> adj_list_;

    std::unordered_map<MachineOperand *, int32_t> degree_;


};

#endif //YFSCC_REGSALLOCATOR_H
