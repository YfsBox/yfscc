//
// Created by 杨丰硕 on 2023/6/8.
//

#ifndef YFSCC_COLLECTUSEDGLOBALS_H
#define YFSCC_COLLECTUSEDGLOBALS_H

#include "PassManager.h"

class CollectUsedGlobals: public Pass {
public:
    CollectUsedGlobals(Module *module): Pass(module) {}

    ~CollectUsedGlobals() = default;

    void run() override;
private:
};

#endif //YFSCC_COLLECTUSEDGLOBALS_H
