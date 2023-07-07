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



#endif //YFSCC_BACKENDPASS_H
