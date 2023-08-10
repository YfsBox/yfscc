//
// Created by 杨丰硕 on 2023/6/8.
//

#ifndef YFSCC_COLLECTUSEDGLOBALS_H
#define YFSCC_COLLECTUSEDGLOBALS_H

#include "PassManager.h"

class CollectUsedGlobals: public Pass {
public:
    explicit CollectUsedGlobals(Module *module): Pass(module) {
        pass_name_ = "CollectUsedGlobals";
    }

    ~CollectUsedGlobals() = default;

protected:

    void runOnFunction() override;

private:
};

#endif //YFSCC_COLLECTUSEDGLOBALS_H
