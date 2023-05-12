//
// Created by 杨丰硕 on 2023/5/12.
//

#ifndef YFSCC_PASSMANAGER_H
#define YFSCC_PASSMANAGER_H

#include <vector>

class Module;
class PassManager;

class Pass {
public:

    friend class PassManager;

    Pass(Module *module);

    virtual ~Pass() = default;

    virtual void run() = 0;

protected:
    Module *module_;
};

class PassManager {
public:

    PassManager(Module *module): module_(module) {}

    ~PassManager() = default;

    void addPass(Pass *pass) {
        passes_.push_back(pass);
    }

    void run() {
        for (auto pass: passes_) {
            pass->run();
        }
    }

private:
    Module *module_;

    std::vector<Pass *> passes_;
};

#endif //YFSCC_PASSMANAGER_H
