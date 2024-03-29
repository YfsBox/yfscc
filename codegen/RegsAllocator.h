//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_REGSALLOCATOR_H
#define YFSCC_REGSALLOCATOR_H

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>
#include "Machine.h"
#include "MachineOperand.h"

class CodeGen;
class MachineDumper;

class RegsAllocator {
public:
    using OperandSet = std::unordered_set<MachineOperand *>;

    using InstSet = std::unordered_set<MachineInst *>;

    RegsAllocator(MachineModule *mc_module, CodeGen *codegen): machine_module_(mc_module), code_gen_(codegen) {}

    ~RegsAllocator() = default;

    static void regsAllocate(MachineModule *mc_module, CodeGen *codegen);

    static constexpr const int32_t VREGS_THRSHOLD_VALUE = 6000;

    static constexpr const int32_t INST_THRSHOLD_VALUE = 6000;

    virtual void allocate(MachineFunction *func) = 0;

    virtual void allocate() = 0;

protected:

    virtual void runOnMachineFunction(MachineFunction *function) = 0;

    bool allocate_float_;

    MachineModule *machine_module_;

    CodeGen *code_gen_;

    MachineFunction *curr_function_;

};

class SimpleRegsAllocator: public RegsAllocator {
public:

    SimpleRegsAllocator(MachineModule *module, CodeGen *codegen);

    ~SimpleRegsAllocator() = default;

    void allocate(MachineFunction *func) override;

    void allocate() override;

protected:

    void runOnMachineFunction(MachineFunction *function) override;

private:
    int32_t spilled_stack_offset_;

    std::unordered_map<MachineOperand *, int32_t> spilled_vregs_offset_;
};

class ColoringRegsAllocator: public RegsAllocator {
public:
    using BitSet = std::unordered_set<MachineOperand *>;

    using BitSetVec = std::unordered_map<MachineBasicBlock *, BitSet>;

    using AdjEdge = std::pair<MachineOperand *, MachineOperand *>;

    ColoringRegsAllocator(MachineModule *module, CodeGen *codegen);

    ~ColoringRegsAllocator() = default;

    void allocate(MachineFunction *func) override;

    void allocate() override;

    // for test
    MachineDumper *dumper_;

protected:

    void runOnMachineFunction(MachineFunction *function) override;

private:

    struct Stack {
        std::deque<MachineOperand *> stack_;
        std::unordered_set<MachineOperand *> set_;

        void push(MachineOperand *operand) {
            stack_.push_back(operand);
            set_.insert(operand);
        }

        MachineOperand *pop() {
            auto back = stack_.back();
            stack_.pop_back();
            set_.erase(back);
            return back;
        }

        bool isInStack(MachineOperand *operand) {
            return set_.count(operand) > 0;
        }

        void clear() {
            stack_.clear();
            set_.clear();
        }

        bool empty() {
            return stack_.empty();
        }
    };

    bool needAllocateForFloat();

    void analyseLiveness(MachineFunction *function);

    void init();

    void build();       // 构造冲突图，同时也会填充一些关于move的信息

    void mkWorkList();      // 将不同类型的变量分类到不同的集合之中，有三种集合

    void simplify();

    void coalesce();

    void freeze();

    void finishAllocate();

    MachineOperand *getAlias(MachineOperand *operand);

    bool isNeedAlloca(MachineOperand *operand) {
        return operand->getOperandType() == MachineOperand::MachineReg || operand->getOperandType() == MachineOperand::VirtualReg;
    }

    void addEdge(MachineOperand *a, MachineOperand *b);

    bool isInAdjSet(MachineOperand *a, MachineOperand *b);

    bool isPrecolored(MachineOperand *operand);

    bool conservative(const OperandSet &nodes);

    OperandSet adjacent(MachineOperand *operand);

    InstSet nodeMoves(MachineOperand *operand);

    bool moveRelated(MachineOperand *operand);

    void decrementDegree(MachineOperand *operand);

    void enableMoves(const OperandSet &operand_set);

    void addWorkList(MachineOperand *operand);

    void combine(MachineOperand *u, MachineOperand *v);

    bool ok(MachineOperand *t, MachineOperand *r);

    void freezeMoves(MachineOperand *operand);

    void deleteRedundantMoveInsts();

    void selectSpill();

    void assignColors();

    void rewriteProgram();

    static bool isEqual(const BitSet &lhs, const BitSet &rhs);

    std::set<MachineReg::Reg> float_regs_set_;

    std::set<MachineReg::Reg> int_regs_set_;

    // liveness analysis result

    BitSetVec live_in_;

    BitSetVec live_out_;

    BitSetVec use_sets_;

    BitSetVec def_sets_;

    std::unordered_map<MachineOperand *, int32_t> while_loop_depth_map;

    std::unordered_map<MachineOperand *, int32_t> def_use_count_map_;

    // node worklist，node sets，and node stack
    std::vector<MachineOperand *> pre_colored_;     // 预着色的寄存器集合

    OperandSet initial_;        // 所有待分配的变量的集合

    OperandSet simplify_work_list_;     // 可以直接进行简化的集合

    OperandSet freeze_work_list_;       // 应当被冻结的集合

    OperandSet spill_work_list_;        // 应该被溢出的集合，所有度数大于等于k的都会被加入到其中

    OperandSet coalesced_nodes_;        // 已经被合并的寄存器节点，合并本身也会伴随这节点的移除，所谓因合并而移除的节点都会放入到该集合中

    OperandSet colored_nodes_;

    OperandSet spilled_nodes_;

    OperandSet already_spilled_;

    Stack select_stack_;     // 所有从graph中被移除的点都会加入到这个stack中

    // Others
    std::unordered_map<MachineOperand *, OperandSet> adj_set_;      // 描述冲突图的数据结构

    std::unordered_map<MachineOperand *, OperandSet> adj_list_;

    std::unordered_map<MachineOperand *, int32_t> degree_;          // 用来统计点的度数

    std::unordered_map<MachineOperand *, InstSet> move_list_;       // 某个虚拟寄存器所处的move指令集合

    int32_t k_;             // 可以分配的寄存器数量

    int32_t spilled_stack_size_;

    // moves set
    InstSet active_moves_;

    InstSet worklist_moves_;

    InstSet coalesced_moves_;

    InstSet constrained_moves_;

    InstSet frozen_moves_;

    std::unordered_map<MachineOperand *, MachineOperand *> alias_;

    std::unordered_map<MachineOperand *, MachineReg::Reg> color_;

};

#endif //YFSCC_REGSALLOCATOR_H
