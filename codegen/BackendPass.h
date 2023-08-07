//
// Created by 杨丰硕 on 2023/7/7.
//

#ifndef YFSCC_BACKENDPASS_H
#define YFSCC_BACKENDPASS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "MachineInst.h"

class MachineModule;
class MachineFunction;
class MachineBasicBlock;
class MachineDumper;

class BackendPass {
public:

    explicit BackendPass(MachineModule *module);

    ~BackendPass() = default;

    void run();

    MachineDumper *dumper_;

protected:

    virtual void runOnFunction() = 0;

    MachineModule *module_;

    MachineFunction *curr_func_;
};


class BackendPassManager {
public:

    explicit BackendPassManager(MachineModule *module);

    ~BackendPassManager() = default;

    void addPass(BackendPass *pass) {
        passes_.push_back(pass);
    }

    void run() {
        for (auto pass: passes_) {
            pass->run();
        }
    }

private:

    MachineModule *module_;

    std::vector<BackendPass *> passes_;
};

class ReDundantLoadElim: public BackendPass {
public:

    explicit ReDundantLoadElim(MachineModule *module);

    ~ReDundantLoadElim() = default;

protected:

    void runOnFunction() override;

private:

    std::unordered_set<MachineInst *> remove_insts_;

};

class BlocksMergePass: public BackendPass {
public:

    using McBlocksSet = std::unordered_set<MachineBasicBlock *>;

    using McInstSet = std::unordered_set<MachineInst *>;

    explicit BlocksMergePass(MachineModule *module);

    ~BlocksMergePass() = default;

protected:

    void runOnFunction() override;

private:

    void init();

    void buildCfgs();

    bool canBeMerged(MachineBasicBlock *mc_block);

    void mergeBlock(MachineBasicBlock *mc_block);

    void mergeBlocks();

    void simplifyBranch();

    void collectBranchInsts();

    void clearEmptyBlocks();

    std::unordered_map<MachineBasicBlock *, McInstSet> bb_br_insts_map_;

    std::unordered_map<std::string, MachineBasicBlock *> name_block_map_;

    std::unordered_map<MachineBasicBlock *, McBlocksSet> successors_map_;

    std::unordered_map<MachineBasicBlock *, McBlocksSet> predecessors_map_;

};

class SimplifyBackendBranch: public BackendPass {
public:

    using BrMachineInsts = std::unordered_set<BranchInst *>;

    explicit SimplifyBackendBranch(MachineModule *module);

    ~SimplifyBackendBranch() = default;

protected:

    void runOnFunction() override;

private:

    void collectBranchInsts();

    void removeInsts();

    std::unordered_set<MachineInst *> removed_insts_;

    std::unordered_map<MachineBasicBlock *, BrMachineInsts> block_branch_inst_map_;
};



#endif //YFSCC_BACKENDPASS_H
