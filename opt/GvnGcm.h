//
// Created by 杨丰硕 on 2023/6/10.
//

#ifndef YFSCC_GVNGCM_H
#define YFSCC_GVNGCM_H

#include "PassManager.h"

class GvnGcm : public Pass {
public:
    explicit GvnGcm(Module *module): Pass(module) {}

    ~GvnGcm() = default;
protected:

    void runOnFunction() override;

private:
};


#endif //YFSCC_GVNGCM_H
