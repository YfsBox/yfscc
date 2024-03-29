//
// Created by 杨丰硕 on 2023/5/12.
//

#ifndef YFSCC_PASSMANAGER_H
#define YFSCC_PASSMANAGER_H

#include <vector>
#include <iostream>

class Module;
class PassManager;
class Function;
class IrDumper;

class Pass {
public:

    friend class PassManager;

    explicit Pass(Module *module): module_(module), curr_func_(nullptr) {}

    virtual ~Pass() = default;

    void run();

    IrDumper *ir_dumper_;

protected:

    virtual void runOnFunction() = 0;

    Module *module_;

    Function *curr_func_;

    std::string pass_name_;
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
