//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_REGSALLOCATOR_H
#define YFSCC_REGSALLOCATOR_H

#include <unordered_set>
#include <unordered_map>
#include <deque>
#include "Machine.h"
#include "MachineOperand.h"

class RegsAllocator {
public:
    using BitSet = std::unordered_set<MachineOperand *>;

    using BitSetVec = std::unordered_map<MachineBasicBlock *, BitSet>;

    using AdjEdge = std::pair<MachineOperand *, MachineOperand *>;

    using OperandSet = std::unordered_set<MachineOperand *>;

    using InstSet = std::unordered_set<MachineInst *>;

    RegsAllocator(MachineModule *module): machine_module_(module) {}

    ~RegsAllocator() = default;

private:
    void analyseLiveness(MachineFunction *function);

    void build();       // 构造冲突图，同时也会填充一些关于move的信息

    void mkWorkList();      // 将不同类型的变量分类到不同的集合之中，有三种集合

    void simplify();

    void addEdge(MachineOperand *a, MachineOperand *b);

    bool isInAdjSet(MachineOperand *a, MachineOperand *b);

    bool isPrecolored(MachineOperand *operand);

    OperandSet adjacent(MachineOperand *operand);

    InstSet nodeMoves(MachineOperand *operand);

    bool moveRelated(MachineOperand *operand);

    void decrementDegree(MachineOperand *operand);

    void enableMoves(const OperandSet &operand_set);

    static bool isEqual(const BitSet &lhs, const BitSet &rhs);

    MachineModule *machine_module_;

    MachineFunction *curr_function_;

    BitSetVec live_in_;

    BitSetVec live_out_;

    BitSetVec use_sets_;

    BitSetVec def_sets_;

    std::vector<MachineOperand *> pre_colored_;

    // some data structure about graph
    std::unordered_map<MachineOperand *, InstSet> move_list_;

    std::unordered_map<MachineOperand *, OperandSet> adj_set_;

    std::unordered_map<MachineOperand *, OperandSet> adj_list_;

    std::unordered_map<MachineOperand *, int32_t> degree_;

    //
    OperandSet initial_;

    int32_t k_;

    OperandSet spill_work_list_;

    OperandSet freeze_work_list_;

    OperandSet simplify_work_list_;

    OperandSet coalesced_nodes_;

    //
    InstSet active_moves_;

    InstSet worklist_moves_;

    std::deque<MachineOperand *> select_stack_;

};

#endif //YFSCC_REGSALLOCATOR_H
