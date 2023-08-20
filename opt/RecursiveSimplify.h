//
// Created by 杨丰硕 on 2023/8/21.
//

#ifndef YFSCC_RECURSIVESIMPLIFY_H
#define YFSCC_RECURSIVESIMPLIFY_H

#include "PassManager.h"

class Instruction;
class Value;
class CallInstruction;

class RecursiveSimplify: public Pass {
public:

    explicit RecursiveSimplify(Module *module);

    ~RecursiveSimplify() = default;

protected:

    void runOnFunction() override;

private:

    bool isRecursiveCurrFunction(CallInstruction *inst) const;

    int getOnlyDiffArgumentIndex(CallInstruction *call_inst1, CallInstruction *call_inst2) const;

};


#endif //YFSCC_RECURSIVESIMPLIFY_H
