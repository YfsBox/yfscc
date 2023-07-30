//
// Created by 杨丰硕 on 2023/6/9.
//
#include "../ir/Function.h"
#include "../ir/Module.h"
#include "PassManager.h"

void Pass::run() {
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto curr_func = module_->getFunction(i);
        if (curr_func->isDead()) {
            continue;
        }
        curr_func_ = curr_func;
        runOnFunction();
    }
}

