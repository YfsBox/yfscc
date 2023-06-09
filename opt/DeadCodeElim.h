//
// Created by 杨丰硕 on 2023/6/9.
//

#ifndef YFSCC_DEADCODEELIM_H
#define YFSCC_DEADCODEELIM_H

#include "PassManager.h"

class DeadCodeElim: public Pass {
public:

    explicit DeadCodeElim(Module *module): Pass(module) {}

    ~DeadCodeElim() = default;

protected:

    void runOnFunction() override;

private:
};


#endif //YFSCC_DEADCODEELIM_H
