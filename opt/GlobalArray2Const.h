//
// Created by 杨丰硕 on 2023/8/17.
//

#ifndef YFSCC_GLOBALARRAY2CONST_H
#define YFSCC_GLOBALARRAY2CONST_H

#include "PassManager.h"

class GlobalArray2Const : public Pass {
public:

    explicit GlobalArray2Const(Module *module);

    ~GlobalArray2Const() = default;

protected:

    void runOnFunction() override;

private:

};


#endif //YFSCC_GLOBALARRAY2CONST_H
