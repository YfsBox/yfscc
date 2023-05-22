//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINE_H
#define YFSCC_MACHINE_H

#include <vector>
#include <list>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "MachineInst.h"
#include "MachineOperand.h"

class MachineInst;
class MachineFunction;
class MachineBasicBlock;
class BasicBlock;
class Module;

class MachineModule {
public:
    using MachineFunctionPtr = std::unique_ptr<MachineFunction>;

    MachineModule(Module *irmodule);

    ~MachineModule() = default;

    const Module *getIRModule() const {
        return ir_module_;
    }

    void addMachineFunction(MachineFunction *function);

    const std::vector<MachineFunctionPtr> &getMachineFunctions() const {
        return machine_functions_;
    }

    void addBasicBlockPair(MachineBasicBlock *mcbb, BasicBlock *bb) {
        machinebb2bb_map_[mcbb] = bb;
        bb2machinebb_map_[bb] = mcbb;
    }

    MachineBasicBlock *getMachineBasicBlock(BasicBlock *bb) {
        return bb2machinebb_map_[bb];
    }

    BasicBlock *getBasicBlock(MachineBasicBlock *mcbb) {
        return machinebb2bb_map_[mcbb];
    }

    MachineReg *getMachineReg(MachineReg::Reg reg) {
        return machine_regs_map_[reg];
    }

    const std::unordered_map<MachineReg::Reg, MachineReg *> &getMachineRegMap() const {
        return machine_regs_map_;
    }

private:
    Module *ir_module_;

    std::vector<MachineFunctionPtr> machine_functions_;

    std::unordered_map<MachineBasicBlock *, BasicBlock *> machinebb2bb_map_;

    std::unordered_map<BasicBlock *, MachineBasicBlock *> bb2machinebb_map_;

    std::unordered_map<MachineReg::Reg, MachineReg *> machine_regs_map_;
};

class MachineBasicBlock {
public:
    using MachineInstPtr = std::unique_ptr<MachineInst>;

    using MachineInstListIt = std::list<MachineInstPtr>::iterator;

    MachineBasicBlock(MachineFunction *function, const std::string &name = "");

    ~MachineBasicBlock() = default;

    void addInstruction(MachineInst *inst);

    void addFrontInstruction(MachineInst *inst);

    void insertInstruction(MachineInstListIt it, MachineInst *inst);

    void insertInstructionBefore(MachineInstListIt it, MachineInst *inst);

    MachineInstListIt getInstBackIt() {
        MachineInstListIt back_it = instructions_.end();
        --back_it;
        return back_it;
    }

    const std::list<MachineInstPtr> &getInstructionList() const {
        return instructions_;
    }

    std::list<MachineInstPtr> &getInstructionListNonConst() {
        return instructions_;
    }

    std::string getLabelName() const {
        return label_name_;
    }

    int32_t getLoopDepth() const {
        return loop_depth_;
    }

    int32_t getBelongsToLoops() const {
        return belongs_to_loops_;
    }

    void setLoopDepth(int32_t loop_depth) {
        loop_depth_ = loop_depth;
    }

    void setBelongsToLoops(int32_t belongs_to_loops) {
        belongs_to_loops_ = belongs_to_loops;
    }

    MachineFunction *getParent() const {
        return parent_;
    }

private:
    int32_t loop_depth_;

    int32_t belongs_to_loops_;

    std::string label_name_;

    MachineFunction *parent_;

    std::list<MachineInstPtr> instructions_;
};

class MachineFunction {
public:
    using MachineBasicBlockPtr = std::unique_ptr<MachineBasicBlock>;

    MachineFunction(MachineModule *module, const std::string &name = "");

    ~MachineFunction() = default;

    MachineModule *getModule() const {
        return module_;
    }

    int getBasicBlockSize() const {
        return basic_blocks_.size();
    }

    int getInstSize() const {
        int count = 0;
        for (auto &bb: basic_blocks_) {
            count += bb->getInstructionList().size();
        }
        return count;
    }

    const std::vector<MachineBasicBlockPtr> &getMachineBasicBlock() const {
        return basic_blocks_;
    }

    void addBasicBlock(MachineBasicBlock *basic_block);


    void setEnterBasicBlock(MachineBasicBlock *enter) {
        enter_basic_block_ = enter;
    }

    void addExitBasicBlock(MachineBasicBlock *exit) {
        exit_basic_block_.emplace_back(exit);
    }

    int32_t getExitBasicBlockSize() const {
        return exit_basic_block_.size();
    }

    MachineBasicBlock *getEnterBasicBlock() const {
        return enter_basic_block_;
    }

    MachineBasicBlock *getExitBasicBlock(int idx) const {
        return exit_basic_block_[idx];
    }

    std::string getFunctionName() const {
        return function_name_;
    }

    void addVirtualReg(MachineOperand *operand) {
        virtual_regs_.insert(operand);
    }

    const std::unordered_set<MachineOperand *> &getVirtualRegs() {
        return virtual_regs_;
    }

    void setStackSize(int32_t stack_size) {
        stack_size_ = stack_size;
    }

    int32_t getStackSize() const {
        return stack_size_;
    }

private:
    int32_t stack_size_;

    std::string function_name_;

    MachineModule *module_;

    MachineBasicBlock *enter_basic_block_;

    std::vector<MachineBasicBlock *> exit_basic_block_;

    std::vector<MachineBasicBlockPtr> basic_blocks_;

    std::unordered_set<MachineOperand *> virtual_regs_;

};

#endif //YFSCC_MACHINE_H
