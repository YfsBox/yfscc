//
// Created by 杨丰硕 on 2023/8/17.
//

#ifndef YFSCC_MOVESTOREPASS_H
#define YFSCC_MOVESTOREPASS_H

#include "PassManager.h"
#include <list>
#include <set>

class Value;
class GEPInstruction;
// 是对于GCM的补充，对一系列极为特殊的store指令进行移动
class MoveStorePass: public Pass {
public:

    explicit MoveStorePass(Module *module);

    ~MoveStorePass() = default;

protected:

    void runOnFunction() override;

private:


};


#endif //YFSCC_MOVESTOREPASS_H
