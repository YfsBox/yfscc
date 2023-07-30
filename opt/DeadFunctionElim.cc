//
// Created by 杨丰硕 on 2023/7/30.
//

#include "DeadFunctionElim.h"
#include "../ir/Function.h"
#include "../ir/Module.h"
#include <queue>

DeadFunctionElim::DeadFunctionElim(Module *module):
    Pass(module),
    has_init_(false),
    call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module)) {

}

void DeadFunctionElim::init() {
    if (has_init_) {
        return;
    }

    call_graph_analysis_->analysis();
    std::unordered_set<Function *> visited;
    std::queue<Function *> function_queue;

    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);
        if (function->getName() == "main") {
            visited.insert(function);
            not_dead_functions_.insert(function);
            function_queue.push(function);
            break;
        }
    }

    while (!function_queue.empty()) {
        auto function = function_queue.front();
        function_queue.pop();

        for (auto callee: call_graph_analysis_->getCalleeSet(function)) {
            if (!visited.count(callee)) {
                visited.insert(callee);
                not_dead_functions_.insert(callee);
                function_queue.push(callee);
            }
        }
    }

    has_init_ = true;
}

void DeadFunctionElim::runOnFunction() {
    init();
    if (not_dead_functions_.count(curr_func_)) {
        curr_func_->setDead(false);
    } else {
        curr_func_->setDead(true);
    }
}

