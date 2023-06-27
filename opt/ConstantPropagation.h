//
// Created by 杨丰硕 on 2023/6/9.
//

#ifndef YFSCC_CONSTANTPROPAGATION_H
#define YFSCC_CONSTANTPROPAGATION_H

#include <queue>
#include "PassManager.h"

class Instruction;

class ConstantPropagation: public Pass {
public:

    explicit ConstantPropagation(Module *module): Pass(module) {}

    ~ConstantPropagation() = default;

protected:

    void runOnFunction() override;

private:

    void initForWorkList();

    bool checkCanFold(Instruction *inst);

    void foldForCondJump();

    std::queue<Instruction *> work_inst_list_;

};


#endif //YFSCC_CONSTANTPROPAGATION_H
