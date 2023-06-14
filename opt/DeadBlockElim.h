//
// Created by 杨丰硕 on 2023/6/14.
//

#ifndef YFSCC_DEADBLOCKELIM_H
#define YFSCC_DEADBLOCKELIM_H

#include "PassManager.h"

class BasicBlock;

class DeadBlockElim: public Pass{
public:
    explicit DeadBlockElim(Module *module);

    ~DeadBlockElim() = default;

protected:

    void runOnFunction() override;
private:
    BasicBlock *enter_block_;
};


#endif //YFSCC_DEADBLOCKELIM_H
