//
// Created by 杨丰硕 on 2023/6/10.
//
#include <queue>
#include "Mem2Reg.h"
#include "ComputeDominators.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"
#include "../ir/IrDumper.h"

void Mem2Reg::init() {
    alloca_insts_.clear();
    def_basicblocks_map_.clear();
    phi2alloca_map_.clear();
    phi_var_stack_.clear();
    // 收集好alloca指令
    auto &enter_block = curr_func_->getBlocks().front();
    for (auto &inst: enter_block->getInstructionList()) {
        if (auto alloca_inst = dynamic_cast<AllocaInstruction *>(inst.get()); alloca_inst) {
            if (alloca_inst->isPtrPtr() || alloca_inst->isArray()) {
                continue;
            }
            ir_dumper_->dump(alloca_inst);
            alloca_insts_.insert(alloca_inst);
        }
    }
}

void Mem2Reg::genPhiInsts() {
    // 1. 收集每个def语句所处的basicblock
    for (auto &bb: curr_func_->getBlocks()) {
        for (auto &inst: bb->getInstructionList()) {
            if (auto store_inst = dynamic_cast<StoreInstruction *>(inst.get()); store_inst) {
                auto store_addr = store_inst->getPtr();
                if (auto alloca_inst = dynamic_cast<AllocaInstruction *>(store_addr); alloca_inst && alloca_insts_.count(alloca_inst)) {
                    def_basicblocks_map_[alloca_inst].insert(bb.get());
                }
            }
        }
    }
    // 2. 寻找可以插入Phi指令的地方
    std::queue<BasicBlock *> bb_q;
    std::unordered_set<BasicBlock *> visited_set;
    for (auto alloca_inst : alloca_insts_) {
        auto def_bbs = def_basicblocks_map_[alloca_inst];
        visited_set.clear();
        for (auto bb: def_bbs) {
            bb_q.push(bb);
        }

        int32_t phi_index = 0;
        while (!bb_q.empty()) {
            auto qfront_bb = bb_q.front();
            bb_q.pop();
            for (auto dom_frontier: compute_dominators_->getDomFrontiers(qfront_bb)) {
                if (!visited_set.count(dom_frontier)) {
                    visited_set.insert(dom_frontier);
                    std::string phi_var_name = "phi.var." + alloca_inst->getName() + "." + std::to_string(phi_index++);
                    PhiInstruction *phi_inst = new PhiInstruction(dom_frontier, alloca_inst->getBasicType(), {}, {}, phi_var_name);
                    dom_frontier->addFrontInstruction(phi_inst);
                    phi2alloca_map_[phi_inst] = alloca_inst;
                    if (!def_basicblocks_map_[alloca_inst].count(dom_frontier)) {
                        bb_q.push(dom_frontier);
                    }
                }
            }
            printf("\n");
        }
    }


}

void Mem2Reg::rename(BasicBlock *basic_block) {
    std::unordered_set<Instruction *> deleted_insts;
    auto bb = basic_block;
    for (auto &inst: bb->getInstructionList()) {
        if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst.get()); phi_inst) {
            auto alloca_var = phi2alloca_map_[phi_inst];
            phi_var_stack_[alloca_var].push_back(phi_inst);
        }
    }

    for (auto &inst: bb->getInstructionList()) {
        if (inst->getInstType() == LoadType) {
            auto load_inst = dynamic_cast<LoadInstruction *>(inst.get());
            auto load_src = load_inst->getPtr();

            if (auto alloca_var = dynamic_cast<AllocaInstruction *>(load_src); alloca_var && alloca_insts_.count(alloca_var)) {
                auto replace_value = phi_var_stack_[alloca_var].back();
                // printf("load inst %s replace with %s\n", load_inst->getName().c_str(), replace_value->getName().c_str());
                load_inst->replaceAllUseWith(replace_value);
                deleted_insts.insert(load_inst);
            }

        } else if (inst->getInstType() == StoreType) {
            auto store_inst = dynamic_cast<StoreInstruction *>(inst.get());
            auto store_value = store_inst->getValue();
            auto store_ptr = store_inst->getPtr();

            if (auto alloca_var = dynamic_cast<AllocaInstruction *>(store_ptr); alloca_var && alloca_insts_.count(alloca_var)) {
                // printf("push %s to stack of %s\n", phi_inst->getName().c_str(), alloca_var->getName().c_str());
                phi_var_stack_[alloca_var].push_back(store_value);
                printf("deleted add store inst:\n");
                ir_dumper_->dump(store_inst);
                deleted_insts.insert(store_inst);
            }
        }
    }

    printf("--------curr basicblock is %s, and the phi_var_stack is: --------------\n", bb->getName().c_str());
    for (auto &[var, st]: phi_var_stack_) {
        printf("the var %s stack is here\n", var->getName().c_str());
        for (auto st_value: st) {
            printf("%s\t", st_value->getName().c_str());
        }
        printf("\n");
    }

    // 填充phi指令的内容
    for (auto &succ_bb : bb->getSuccessorBlocks()) {
        for (auto &inst : succ_bb->getInstructionList()) {
            if (inst->getInstType() == PhiType) {
                auto phi_inst = dynamic_cast<PhiInstruction *>(inst.get());
                // printf("the phi name is %s\n", phi_inst->getName().c_str());
                auto phi_var = phi2alloca_map_[phi_inst];
                if (alloca_insts_.count(phi_var) && !phi_var_stack_[phi_var].empty()) {
                    phi_inst->addOperand(phi_var_stack_[phi_var].back());
                    phi_inst->addOperand(bb);
                }
            }
        }
    }

    for (auto dom_succ : compute_dominators_->getSuccessors(bb)) {
        rename(dom_succ);
    }

    for (auto &inst : bb->getInstructionList()) {
        if (inst->getInstType() == StoreType) {
            auto store_inst = dynamic_cast<StoreInstruction *>(inst.get());
            auto store_addr = store_inst->getPtr();
            if (auto alloca_var = dynamic_cast<AllocaInstruction *>(store_addr); alloca_var && alloca_insts_.count(alloca_var)) {
                phi_var_stack_[alloca_var].pop_back();
            }
        } else if (inst->getInstType() == PhiType) {
            auto phi_inst = dynamic_cast<PhiInstruction *>(inst.get());
            auto phi_alloca_var = phi2alloca_map_[phi_inst];
            if (alloca_insts_.count(phi_alloca_var)) {
                phi_var_stack_[phi_alloca_var].pop_back();
            }
        }
    }

    auto &inst_list= bb->getInstructionList();
    for (auto it = inst_list.begin(); it != inst_list.end();) {
        auto inst = it->get();
        if (deleted_insts.count(inst)) {
            ir_dumper_->dump(inst);
            it = inst_list.erase(it);
        } else {
            it++;
        }
    }

}

void Mem2Reg::removeAllocaCode() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto it = insts_list.begin(); it != insts_list.end();) {
            auto inst = it->get();
            if (auto alloca_inst = dynamic_cast<AllocaInstruction *>(inst); alloca_inst && alloca_insts_.count(alloca_inst)) {
                it = insts_list.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Mem2Reg::runOnFunction() {
    compute_dominators_.release();
    compute_dominators_ = std::make_unique<ComputeDominators>(curr_func_);
    compute_dominators_->run();

    init();
    genPhiInsts();
    auto enter_block = curr_func_->getBlocks().front().get();
    rename(enter_block);
    removeAllocaCode();
}