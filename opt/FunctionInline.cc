//
// Created by 杨丰硕 on 2023/6/14.
//

#include "FunctionInline.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"

bool FunctionInline::canBeInline(Function *function) {
    if (call_graph_analysis_->isLibFunction(function)) {
        return false;
    }
    int32_t inst_size = 0;
    for (auto &bb: function->getBlocks()) {
        inst_size += bb->getInstructionSize();
    }
    if (inst_size > inline_insts_size || call_graph_analysis_->isRecursive(function)) {
        return false;
    }
    return true;
}

void FunctionInline::copyFunction(Function *function, std::list<BasicBlockUptr> &copyed_bbs) {



}

FunctionInline::FunctionInline(Module *module):
        Pass(module),
        call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module_)) {
}

void FunctionInline::inlineOnFunction(Function *function) {

}

void FunctionInline::runOnFunction() {
    call_graph_analysis_->analysis();
    if (canBeInline(curr_func_)) {
        inlineOnFunction(curr_func_);
    }
}