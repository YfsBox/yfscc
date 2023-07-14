//
// Created by 杨丰硕 on 2023/7/3.
//

#include "MemoryAnalysis.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"
#include "../ir/IrDumper.h"

void MemoryAnalysis::visitBasicBlock(BasicBlock *basicblock) {
    // printf("the curr block is %s, and memversion size is %d\n", basicblock->getName().c_str(), memversion_table_.back().size());
    auto &insts_list = basicblock->getInstructionList();
    auto end_index = memversion_table_.size() - 1;
    for (auto &inst_uptr: insts_list) {
        if (auto store_inst = dynamic_cast<StoreInstruction *>(inst_uptr.get()); store_inst) {  // 更新version
            auto store_addr = store_inst->getPtr();
            removes_insts_.insert(store_inst);
            // printf("mem version table on %s has value %s\n", store_addr->getName().c_str(), store_inst->getValue()->getName().c_str());
            memversion_table_[end_index][store_addr] = store_inst;
        }
        if (auto load_inst = dynamic_cast<LoadInstruction *>(inst_uptr.get()); load_inst) {
            auto load_ptr = load_inst->getPtr();
            auto &curr_table = memversion_table_.back();
            if (curr_table.find(load_ptr) != curr_table.end() && curr_table[load_ptr] != nullptr) {// 已经存在了，也就是需要进行替换的
                // printf("load inst %s replaced by %s\n", load_inst->getName().c_str(), curr_table[load_ptr]->getValue()->getName().c_str());
                if (curr_table[load_ptr]->getInstType() == StoreType) {
                    replace_load_insts_[load_inst] = dynamic_cast<StoreInstruction *>(curr_table[load_ptr])->getValue();
                } else {
                    replace_load_insts_[load_inst] = curr_table[load_ptr];
                }
            } else {
                memversion_table_[end_index][load_inst->getPtr()] = load_inst;
            }
        }
        if (auto call_inst = dynamic_cast<CallInstruction *>(inst_uptr.get()); call_inst) {
             auto &curr_table = memversion_table_.back();
             for (auto &[addr, memvalue]: curr_table) {
                 removes_insts_.erase(memvalue);
                 memversion_table_[end_index][addr] = nullptr;
             }
        }
    }
}

void MemoryAnalysis::removeAndReplace() {
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
            auto inst = inst_it->get();
            if (replace_load_insts_.count(inst)) {
                // printf("load inst %s replaced by %s\n", inst->getName().c_str(), replace_load_insts_[inst]->getName().c_str());
                //inst->replaceAllUseWith(replace_load_insts_[inst]);
                for (auto user: user_analysis_->getUserInsts(inst)) {
                    user->replaceWithValue(inst, replace_load_insts_[inst]);
                }
            }
        }
        // ir_dumper_->dump(bb_uptr.get());
    }

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
            auto inst = inst_it->get();
            if (removes_insts_.count(inst)) {
                // printf("the inst removed:\n");
                // ir_dumper_->dump(inst);
                inst_it = insts_list.erase(inst_it);
            } else {
                ++inst_it;
            }
        }
        // ir_dumper_->dump(bb_uptr.get());
    }

}

void MemoryAnalysis::simplifyStoreSecondaryPtr() {
    // 首先收集Store指令

    std::unordered_map<Value *, Value *> store_ptr_map;

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (auto store_inst = dynamic_cast<StoreInstruction *>(inst); store_inst && store_inst->getValue()->isPtr()) {
                printf("the store %s to %s set in map\n", store_inst->getValue()->getName().c_str(), store_inst->getPtr()->getName().c_str());
                store_ptr_map[store_inst->getPtr()] = store_inst->getValue();
            }
        }
    }
    // 找到和map中的指针关联的load语句，

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (auto load_inst = dynamic_cast<LoadInstruction *>(inst); load_inst && store_ptr_map.find(load_inst->getPtr()) != store_ptr_map.end()) {
                printf("load %s replaced with %s\n", load_inst->getName().c_str(), store_ptr_map[load_inst->getPtr()]->getName().c_str());
                for (auto user_inst : user_analysis_->getUserInsts(load_inst)) {
                    user_inst->replaceWithValue(load_inst, store_ptr_map[load_inst->getPtr()]);
                }
            }
        }
    }

    // 将load的结果进行替换

}

void MemoryAnalysis::visitExtendBasicBlock(BasicBlock *basicblock) {
    visited_bbs_.insert(basicblock);
    allocateTable();
    visitBasicBlock(basicblock);

    bool end_extendbb = true;
    for (auto succ_bb: basicblock->getSuccessorBlocks()) {
        if (succ_bb->getPreDecessorBlocks().size() == 1) {
            visitExtendBasicBlock(succ_bb);
        } else {
            end_extendbb = false;
            if (!visited_bbs_.count(succ_bb)) {
                visited_bbs_.insert(succ_bb);
                work_list_.push_back(succ_bb);
            }
        }
    }

    if (!end_extendbb || basicblock->getSuccessorBlocks().size() == 0) {     // 处于一个拓展基本块的末尾，需要保留最后的store
        auto &curr_table = memversion_table_.back();
        for (auto &[addr, memvalue]: curr_table) {
            removes_insts_.erase(memvalue);
        }
    }

    deallocateTable();
}

void MemoryAnalysis::runOnFunction() {
    replace_load_insts_.clear();
    removes_insts_.clear();
    visited_bbs_.clear();
    work_list_.clear();
    memversion_table_.clear();

    user_analysis_->analysis(curr_func_);

    auto enter_block = curr_func_->getBlocks().begin()->get();
    work_list_.push_back(enter_block);

    while (!work_list_.empty()) {
        auto bb = work_list_.front();
        work_list_.pop_front();
        visitExtendBasicBlock(bb);
    }

    removeAndReplace();

    simplifyStoreSecondaryPtr();
}