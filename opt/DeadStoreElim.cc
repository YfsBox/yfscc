//
// Created by 杨丰硕 on 2023/8/20.
//

#include "DeadStoreElim.h"
#include "GlobalAnalysis.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"

DeadStoreElim::DeadStoreElim(Module *module): Pass(module) {}

void DeadStoreElim::runOnFunction() {
    if (curr_func_->getName() != "main") {
        return;
    }

    ArrayAnalysis array_analysis;
    array_analysis.analysis(curr_func_);

    // 作用于const gep类型的store指令
    std::unordered_set<StoreInstruction *> tmp_dead_store_insts;

    // 将load指令划分成对const gep以及non const gep的两类

    using LoadInstSet = std::unordered_set<LoadInstruction *>;
    using GEPInstSet = std::unordered_set<GEPInstruction *>;

    std::unordered_map<Value *, GEPInstSet> constgep_load_map;
    std::unordered_set<Value *> array_has_nonconst_load;

    auto &array_info_map = array_analysis.getArrayInfoMap();
    for (auto &[array_base, array_info]: array_info_map) {
        // 忽略全局数组
        if (array_base->getValueType() == GlobalVariableValue) {
            continue;
        }

        for (auto load_inst: array_info->load_insts_) {
            if (GEPInstruction *gep_inst = dynamic_cast<GEPInstruction *>(load_inst->getPtr()); gep_inst && array_info->const_index_gep_insts_.count(gep_inst)) {
                constgep_load_map[array_base].insert(gep_inst);
            } else {
                array_has_nonconst_load.insert(array_base);
            }
        }
    }

    // (1) 如果当前函数中没有任何含有指针类型的参数的函数调用，就继续判断下去
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            if (auto call_inst = dynamic_cast<CallInstruction *>(inst_uptr.get()); call_inst) {
                for (int i = 0; i < call_inst->getActualSize(); ++i) {
                    auto actual = call_inst->getActual(i);
                    if (actual->isPtr()) {
                        return;
                    }
                }
            }
        }
    }
    // 一个store inst可以被判定为dead的条件：(2) 该数组nonconst gep相关的load为空 (3) 没有对应的const gep关联的load

    for (auto &[array_base, array_info]: array_info_map) {
        if (array_base->getValueType() == GlobalVariableValue && array_has_nonconst_load.count(array_base)) {
            continue;
        }

        for (auto store_inst: array_info->store_insts_) {
            if (GEPInstruction *gep_inst = dynamic_cast<GEPInstruction *>(store_inst->getPtr()); gep_inst && array_info->const_index_gep_insts_.count(gep_inst)) {
                if (!constgep_load_map[array_base].count(gep_inst)) {         // 如果不包含对这个gep的load, 就可以暂时被判断为dead, 还需要最后一步
                    tmp_dead_store_insts.insert(store_inst);
                }
            }
        }
    }

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto insts_it = insts_list.begin(); insts_it != insts_list.end();) {
            if (tmp_dead_store_insts.count(dynamic_cast<StoreInstruction *>(insts_it->get()))) {
                insts_it = insts_list.erase(insts_it);
            } else {
                insts_it++;
            }
        }
    }

}