//
// Created by 杨丰硕 on 2023/6/9.
//

#ifndef YFSCC_CONSTANTPROPAGATION_H
#define YFSCC_CONSTANTPROPAGATION_H

#include "PassManager.h"

class ConstantPropagation: public Pass {
public:

    explicit ConstantPropagation(Module *module): Pass(module) {}

    ~ConstantPropagation() = default;

protected:

    void runOnFunction() override;

private:
};


#endif //YFSCC_CONSTANTPROPAGATION_H
