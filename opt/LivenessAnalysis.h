//
// Created by 杨丰硕 on 2023/5/12.
//

#ifndef YFSCC_LIVENESSANALYSIS_H
#define YFSCC_LIVENESSANALYSIS_H

#include "PassManager.h"

class LivenessAnalysis: public Pass {
public:
    LivenessAnalysis(Module *module);

    ~LivenessAnalysis() = default;

    void run() override;

private:

};

#endif //YFSCC_LIVENESSANALYSIS_H
