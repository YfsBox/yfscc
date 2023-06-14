//
// Created by 杨丰硕 on 2023/6/14.
//
#include "CallGraphAnalysis.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"

void CallGraphAnalysis::initForLibFunction() {
    for (int i = 0; i < module_->getLibFuncSize(); ++i) {
        auto lib_func = module_->getLibFunction(i);
        lib_function_set_.insert(lib_func);
        side_effect_functions_.insert(lib_func);
    }
}


void CallGraphAnalysis::buildCallGraph() {
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto func = module_->getFunction(i);
        std::unordered_set<Argument *> ptr_args;
        for (int j = 0; j < func->getArgumentSize(); ++j) {
            auto arg = func->getArgument(j);
            if (arg->isPtrArg()) {
                ptr_args.insert(arg);
                printf("the ptr arg is %s in function %s\n", arg->getName().c_str(), func->getName().c_str());
            }
        }

        auto &blocks = func->getBlocks();

        for (auto &bb: blocks) {
            auto &insts_list = bb->getInstructionList();
            for (auto &inst_uptr: insts_list) {
                auto inst = inst_uptr.get();
                if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst) {
                    auto callee = call_inst->getFunction();
                    callee_sets_[func].insert(callee);
                    caller_sets_[callee].insert(func);
                    if (side_effect_functions_.count(callee)) {
                        side_effect_functions_.insert(func);
                    }
                }
                if (auto store_inst = dynamic_cast<StoreInstruction *>(inst); store_inst) {
                    auto store_addr = dynamic_cast<AllocaInstruction *>(store_inst->getPtr());
                    auto value_inst = dynamic_cast<Argument *>(store_inst->getValue());
                    if (ptr_args.count(value_inst) && store_addr) {
                        continue;
                    }
                    side_effect_functions_.insert(func);
                }
            }
        }
    }
}

void CallGraphAnalysis::analysis() {
    initForLibFunction();
    buildCallGraph();

    while (true) {
        auto old_side_effect_size = side_effect_functions_.size();

        for (int i = 0; i < module_->getFuncSize(); ++i) {
            auto func = module_->getFunction(i);
            if (side_effect_functions_.count(func)) {
                for (auto caller: caller_sets_[func]) {
                    side_effect_functions_.insert(caller);
                }
            }
        }
        // printf("the old size is %d, and new size is %d\n", old_side_effect_size, side_effect_functions_.size());
        if (old_side_effect_size == side_effect_functions_.size()) {
            break;
        }
    }
    // 输出有副作用的function

    for (auto func: side_effect_functions_) {
        if (!lib_function_set_.count(func)) {
            printf("function %s has side effect\n", func->getName().c_str());
        }
    }

}