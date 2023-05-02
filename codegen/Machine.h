//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINE_H
#define YFSCC_MACHINE_H

#include <vector>
#include <list>
#include <memory>

class MachineInst;
class Module;

class MachineModule {
public:

private:
    Module *ir_module_;
};

class MachineBasicBlock {
public:
    using MachineInstPtr = std::unique_ptr<MachineInst>;

private:
    std::list<MachineInstPtr> instructions_;
};

class MachineFunction {
public:
    using MachineBasicBlockPtr = std::unique_ptr<MachineBasicBlock>;

private:
    std::vector<MachineBasicBlockPtr> basic_blocks_;
};


#endif //YFSCC_MACHINE_H
