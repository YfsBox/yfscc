//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_IRBUILDER_H
#define YFSCC_IRBUILDER_H

#include "../common/AstVisitor.h"

class Module;
class Function;
class BasicBlock;

struct IrContext {
    int ssa_no_{-1};
    Module *curr_module_;
    Function *curr_function_;
    BasicBlock *curr_bb_;
    void ResetSSA();
};

void IrContext::ResetSSA() {
    ssa_no_ = -1;
}

class IrBuilder: public AstVisitor {
public:
private:
    std::unique_ptr<IrContext> context_;
};


#endif //YFSCC_IRBUILDER_H
