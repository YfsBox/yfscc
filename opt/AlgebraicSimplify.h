//
// Created by 杨丰硕 on 2023/6/21.
//

#ifndef YFSCC_ALGEBRAICSIMPLIFY_H
#define YFSCC_ALGEBRAICSIMPLIFY_H

#include <unordered_set>
#include <unordered_map>
#include "PassManager.h"

class Value;
class Instruction;

class AlgebraicSimplify: public Pass {
public:

    explicit AlgebraicSimplify(Module *module): Pass(module) {}

    ~AlgebraicSimplify() = default;

protected:

    void runOnFunction() override;

private:

    void removeInsts();

    void replaceWithSimpleInst();

    static bool isPowerOfTwo(int32_t num);

    std::unordered_map<Instruction *, Value *> wait_delete_insts_;

};


#endif //YFSCC_ALGEBRAICSIMPLIFY_H
