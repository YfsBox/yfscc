//
// Created by 杨丰硕 on 2023/8/7.
//

#include "EndRename.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"

void EndRename::init() {
    basic_block_label_no_ = 0;
    value_no_ = curr_func_->getArgumentSize();
}

void EndRename::runOnFunction() {
    init();
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        bb_uptr->reName(".L" + std::to_string(basic_block_label_no_++));
        auto &inst_values = bb_uptr->getInstructionList();
        for (auto &inst_value: inst_values) {
            inst_value->reName(std::to_string(value_no_++));
        }
    }
}