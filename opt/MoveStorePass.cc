//
// Created by 杨丰硕 on 2023/8/17.
//

#include "MoveStorePass.h"
#include "GlobalAnalysis.h"
#include "../ir/Constant.h"
#include "../ir/BasicBlock.h"
#include <set>

MoveStorePass::MoveStorePass(Module *module): Pass(module) {}

void MoveStorePass::runOnFunction() {
    // 需要基于支配信息和数组信息进行移动
    ArrayAnalysis array_analysis;
    array_analysis.analysis(curr_func_);
    std::unordered_set<StoreInstruction *> store_insts;
    auto &array_info_map = array_analysis.getArrayInfoMap();
    // 首先收集可以确定用来进行移动的store指令: 1. store的值确定，2. 对应的地址也确定，也就是一个const index gep
    for (auto &[array_base, array_info]: array_info_map) {
        for (auto store_inst: array_info->store_insts_) {
            if (auto const_value = dynamic_cast<ConstantVar *>(store_inst->getValue()); const_value) {
                auto ptr_gep_inst = dynamic_cast<GEPInstruction *>(store_inst->getPtr());
                if (ptr_gep_inst && array_info->const_index_gep_insts_.count(ptr_gep_inst)) {
                    store_insts.insert(store_inst);
                }
            }
        }
    }
    // 如果这些store和memset都是对于同一个数组的操作，并且完全覆盖了这个数组的范围
    for (auto &[array_base, array_info]: array_analysis.getArrayInfoMap()) {
        // printf("has one array\n");
        std::list<Instruction *> move_insts;      // 可以移动的指令
        std::set<Instruction *> move_insts_set;
        auto array_base_alloc_inst = dynamic_cast<AllocaInstruction *>(array_base);

        if (!array_base_alloc_inst || !array_base_alloc_inst->isArray()) {
            continue;
        }
        if (array_info->memset_call_insts_.size() > 1) {
            continue;
        }

        for (auto store_inst: array_info->store_insts_) {
            if (store_inst->getValue()->getValueType() == ConstantValue && array_info->const_index_gep_insts_.count(dynamic_cast<GEPInstruction *>(store_inst->getPtr()))) {
                move_insts.push_back(store_inst);
                move_insts_set.insert(store_inst);
            }
        }
        if (move_insts.size() != 2 * array_info->const_index_gep_insts_.size() - 1) {
            printf("the gep size is %d and the move insts size is %d\n", move_insts.size(), array_info->const_index_gep_insts_.size());
            continue;
        }

        // 之后可以从原处消除掉
        for (auto &bb_uptr: curr_func_->getBlocks()) {
            auto &insts_list = bb_uptr->getInstructionList();
            for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
                if (move_insts_set.count(inst_it->get())) {
                    inst_it->release();
                    inst_it = insts_list.erase(inst_it);
                } else {
                    ++inst_it;
                }
            }
        }
        // 插入到enter block中
        auto enter_block = curr_func_->getBlocks().front().get();
        auto end_inst_it = enter_block->getInstructionList().end();
        end_inst_it--;

        for (auto inst: move_insts) {
            // printf("move inst %s to block %s\n", inst->getName().c_str(), enter_block->getName().c_str());
            inst->setParent(enter_block);
            enter_block->insertInstruction(end_inst_it, inst);
        }
    }
}