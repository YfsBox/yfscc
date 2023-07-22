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

    void setPre(bool pre) {
        pre_ = pre;
    }

protected:

    void moveStore();

    void global2Const();

    void global2Reg();

    void crazyInline();

    void crazyElim();

    void crazyBranch();

    void runOnFunction() override;

private:

    bool pre_;
};


#endif //YFSCC_CRAZYWORK_H
