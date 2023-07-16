//
// Created by 杨丰硕 on 2023/7/14.
//

#ifndef YFSCC_CRAZYWORK_H
#define YFSCC_CRAZYWORK_H

#include "PassManager.h"


class CrazyWork: public Pass {
public:

    explicit CrazyWork(Module *module);

    ~CrazyWork() = default;

protected:

    void moveStore();

    void global2Const();

    void global2Reg();

    void crazyInline();

    void runOnFunction() override;

private:
};


#endif //YFSCC_CRAZYWORK_H
