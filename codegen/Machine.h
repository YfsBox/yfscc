//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINE_H
#define YFSCC_MACHINE_H

#include <vector>
#include <list>
#include <memory>
#include "MachineInst.h"

class MachineInst;
class MachineFunction;
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

private:
    Module *ir_module_;

    std::vector<MachineFunctionPtr> machine_functions_;
};

class MachineBasicBlock {
public:
    using MachineInstPtr = std::unique_ptr<MachineInst>;

    MachineBasicBlock(MachineFunction *function);

    ~MachineBasicBlock() = default;

    void addInstruction(MachineInst *inst);

    void addFrontInstruction(MachineInst *inst);

private:
    MachineFunction *parent_;
    std::list<MachineInstPtr> instructions_;
};

class MachineFunction {
public:
    using MachineBasicBlockPtr = std::unique_ptr<MachineBasicBlock>;

    MachineFunction(MachineModule *module);

    ~MachineFunction() = default;

    MachineModule *getModule() const {
        return module_;
    }

    int getBasicBlockSize() const {
        return basic_blocks_.size();
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

private:
    MachineModule *module_;

    MachineBasicBlock *enter_basic_block_;

    std::vector<MachineBasicBlock *> exit_basic_block_;

    std::vector<MachineBasicBlockPtr> basic_blocks_;
};

#endif //YFSCC_MACHINE_H
