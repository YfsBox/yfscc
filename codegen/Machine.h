//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINE_H
#define YFSCC_MACHINE_H

#include <vector>
#include <list>
#include <memory>

class MachineInst;
class MachineFunction;
class Module;

class MachineModule {
public:
    MachineModule(Module *irmodule);

    ~MachineModule() = default;

    const Module *getIRModule() const {
        return ir_module_;
    }

    std::string dump();

private:
    Module *ir_module_;
};

class MachineBasicBlock {
public:
    using MachineInstPtr = std::unique_ptr<MachineInst>;

    MachineBasicBlock(MachineFunction *function);

    ~MachineBasicBlock() = default;

    void addInstruction(MachineInst *inst);

    std::string dump();
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

    std::string dump();
private:
    MachineModule *module_;
    std::vector<MachineBasicBlockPtr> basic_blocks_;
};

#endif //YFSCC_MACHINE_H
