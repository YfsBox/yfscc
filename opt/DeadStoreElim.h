//
// Created by 杨丰硕 on 2023/8/20.
//

#ifndef YFSCC_DEADSTOREELIM_H
#define YFSCC_DEADSTOREELIM_H

#include "PassManager.h"
#include <unordered_set>

class Instruction;
// 分析main函数栈上数组相关的store是否是dead的
// 放置在split GEP之前
class DeadStoreElim: public Pass {
public:

    explicit DeadStoreElim(Module *module);

    ~DeadStoreElim() = default;

protected:

    void runOnFunction() override;

private:

    std::unordered_set<Instruction *> dead_store_insts_;

};


#endif //YFSCC_DEADSTOREELIM_H
