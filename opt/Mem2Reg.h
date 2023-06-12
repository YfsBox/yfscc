//
// Created by 杨丰硕 on 2023/6/10.
//

#ifndef YFSCC_MEM2REG_H
#define YFSCC_MEM2REG_H

#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "PassManager.h"
#include "ComputeDominators.h"

class Value;
class BasicBlock;
class PhiInstruction;
class AllocaInstruction;
class Instruction;
class IrDumper;

class Mem2Reg: public Pass {
public:

    using BasicBlockSet = std::unordered_set<BasicBlock *>;

    using PhiInstSet = std::unordered_set<PhiInstruction *>;

    explicit Mem2Reg(Module *module): Pass(module), compute_dominators_(nullptr) {}

    ~Mem2Reg() = default;

protected:

    void runOnFunction() override;

private:

    void init();

    void genPhiInsts();

    void rename(BasicBlock *basic_block);

    void removeAllocaCode();

    std::unique_ptr<ComputeDominators> compute_dominators_;

    std::unordered_set<AllocaInstruction *> alloca_insts_;

    std::unordered_map<AllocaInstruction *, BasicBlockSet> def_basicblocks_map_;

    std::unordered_map<BasicBlock *, PhiInstSet> phi_insts_map_;

    std::unordered_map<PhiInstruction *, AllocaInstruction *> phi2alloca_map_;

    std::unordered_map<AllocaInstruction *, std::vector<Value *>> phi_var_stack_;

    // std::unordered_set<Instruction *> deleted_insts_;

};


#endif //YFSCC_MEM2REG_H
