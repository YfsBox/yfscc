//
// Created by 杨丰硕 on 2023/8/20.
//

#include "DeadStoreElim.h"
#include "GlobalAnalysis.h"
#include "ComputeDominators.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"

DeadStoreElim::DeadStoreElim(Module *module): Pass(module) {}

void DeadStoreElim::replaceOnlyLoad(ArrayAnalysis& analysis) {
    // (1) 如果一个数组中，对于const index gep的load只有一个也是在该const index gep上store
    // (2) 如果这个store是支配这个load的
    // (3) 没有涉及到访存的函数调用(这个条件是调用replaceOnlyLoad的前提)
    auto &array_info_map = analysis.getArrayInfoMap();
    for (auto &[array_base, array_info]: array_info_map) {


    }



}

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
    using StoreInstSet = std::unordered_set<StoreInstruction *>;

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
                if (!constgep_load_map[array_base].count(gep_inst)) {         // 如果不包含对这个gep的load, 就可以暂时被判断为dead
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

    // 需要对于一种特殊的情况进行处理，也就是只有唯一个load的情况，或者说只存在一对多的store(一个store，多个load)、load指令(对于同一个const index gep的)，并且store都是支配load的，这个时候这对load和store也是可以消除的
    // 并且还有其他限制条件，也就是没有对非const gep的store


    // 1. 首先分出对const index gep的store以及对nonconst index gep的指令
    ArrayAnalysis array_analysis1;
    array_analysis1.analysis(curr_func_);

    std::unordered_map<GEPInstruction *, StoreInstSet> store_const_gep_insts;        // 保存一个array中对于const index gep的store指令
    std::unordered_map<GEPInstruction *, LoadInstSet> load_const_gep_insts;
    std::unordered_set<Value *> array_has_nonconstgep_store;        // 用来保存那些存在对nongep const进行store的array

    std::vector<std::pair<StoreInstruction *, LoadInstruction *>> only_load_store_pairs;

    for (auto &[array_base, array_info]: array_analysis1.getArrayInfoMap()) {
        if (array_base->getValueType() == GlobalVariableValue && array_has_nonconst_load.count(array_base)) {
            continue;
        }

        // 仍然需要将load和store划分为对const index gep和noncosnt index gep的两类
        bool has_store_on_nonconstgep = false;
        for (auto store_inst: array_info->store_insts_) {
            if (auto gep_ptr = dynamic_cast<GEPInstruction *>(store_inst->getPtr()); gep_ptr && array_info->const_index_gep_insts_.count(gep_ptr)) {
                store_const_gep_insts[gep_ptr].insert(store_inst);
            } else {
                has_store_on_nonconstgep = true;
            }
        }

        if (has_store_on_nonconstgep) {
            continue;
        }

        for (auto load_inst: array_info->load_insts_) {
            if (auto gep_ptr = dynamic_cast<GEPInstruction *>(load_inst->getPtr()); gep_ptr && array_info->const_index_gep_insts_.count(gep_ptr)) {
                load_const_gep_insts[gep_ptr].insert(load_inst);
            }
        }
    }

    std::unordered_map<StoreInstruction *, LoadInstSet> store_load_pairs;       // 都是作用于同一个const index gep的访存指令，并且store和load是一对多的
    for (auto &[gep, store_set]: store_const_gep_insts) {
        if (store_set.size() == 1) {        // store对load一对一或者一对多
            for (auto load_inst: load_const_gep_insts[gep]) {
                store_load_pairs[*store_set.begin()].insert(load_inst);
            }
        }
    }

    if (!store_load_pairs.empty()) {
        ComputeDominators dominator_compute(curr_func_);
        dominator_compute.run();

        std::unordered_map<LoadInstruction *, StoreInstruction *> load_insts_need_replace;
        std::unordered_set<Instruction *> store_insts_canremove;

        for (auto &[store_inst, load_insts]: store_load_pairs) {
            bool is_dominator_all = true;
            BasicBlock *store_on_block = store_inst->getParent();
            for (auto load_inst: load_insts) {
                if (!dominator_compute.getDoms(load_inst->getParent()).count(store_on_block)) {     // 不支配的情况
                    is_dominator_all = false;
                }
            }

            if (is_dominator_all) {
                for (auto load_inst: load_insts) {
                    load_insts_need_replace[load_inst] = store_inst;
                }
                store_insts_canremove.insert(store_inst);
            }
        }

        // 进行替换操作需要基于user analysis进行
        UserAnalysis user_analysis;
        user_analysis.analysis(curr_func_);


        // 对load指令进行替换
        for (auto &[load_inst, store_inst]: load_insts_need_replace) {
            for (auto user: user_analysis.getUserInsts(load_inst)) {
                user->replaceWithValue(load_inst, store_inst->getValue());
            }
        }
        // 接着对store指令进行移除
        for (auto &bb_uptr: curr_func_->getBlocks()) {
            auto &insts_list = bb_uptr->getInstructionList();
            for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
                if (store_insts_canremove.count(inst_it->get())) {
                    inst_it = insts_list.erase(inst_it);
                } else {
                    inst_it++;
                }
            }
        }
    }

}