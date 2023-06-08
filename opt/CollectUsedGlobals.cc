//
// Created by 杨丰硕 on 2023/6/8.
//
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"
#include "../ir/BasicBlock.h"
#include "CollectUsedGlobals.h"

void CollectUsedGlobals::run() {
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto curr_func = module_->getFunction(i);
        for (auto &curr_bb: curr_func->getBlocks()) {
            for (auto &curr_inst : curr_bb->getInstructionList()) {
                for (int k = 0; k < curr_inst->getOperandNum(); ++k) {
                    auto operand = curr_inst->getOperand(k);
                    if (auto global = dynamic_cast<GlobalVariable *>(operand); global) {
                        curr_func->addUsedGlobal(global);
                    }
                }
            }
        }
    }
    // for test
}
