//
// Created by 杨丰硕 on 2023/6/27.
//
#include <memory>
#include "GlobalCodeMotion.h"
#include "ComputeLoops.h"
#include "ComputeDominators.h"
#include "../ir/Instruction.h"
#include "../ir/BasicBlock.h"

GlobalCodeMotion::GlobalCodeMotion(Module *module): Pass(module),
    has_compute_loop_(false),
    root_block_in_currfunc_(nullptr),
    compute_loops_(std::make_unique<ComputeLoops>(module)),
    compute_doms_(nullptr) {

}

bool GlobalCodeMotion::isPinned(const Instruction *inst) {
    auto inst_type = inst->getInstType();
    return inst_type == PhiType || inst_type == RetType
    || inst_type == BrType || inst_type == LoadType
    || inst_type == StoreType || inst_type == CallType
    || inst_type == MemSetType;
}

void GlobalCodeMotion::runOnFunction() {
    compute_doms_.release();
    compute_doms_ = std::make_unique<ComputeDominators>(curr_func_);
    // printf("run compute doms\n");
    compute_doms_->run();
    // printf("run compute doms ok\n");

    if (!has_compute_loop_) {
        compute_loops_.release();
        // printf("run on compute loops\n");
        compute_loops_ = std::make_unique<ComputeLoops>(module_);
        compute_loops_->run();
        // printf("run on compute loops ok\n");
        has_compute_loop_ = true;
    }

    visited_insts_.clear();
    user_insts_map_.clear();
    // schedule early
    // printf("begin schedule early\n");
    root_block_in_currfunc_ = curr_func_->getBlocks().front().get();

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (isPinned(inst)) {
                visited_insts_.insert(inst);
                inst_block_map_[inst] = inst->getParent();
                for (int i = 0; i < inst->getOperandNum(); ++i) {
                    if (inst->getOperand(i)->getValueType() == InstructionValue) {
                        auto input_inst = dynamic_cast<Instruction *>(inst->getOperand(i));
                        scheduleEarly(input_inst);
                    }
                }
            }
        }
    }
    // schedule lately
    // printf("begin schedule lately\n");
    visited_insts_.clear();
    collectUserSets();
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (isPinned(inst)) {
                visited_insts_.insert(inst);
                for (auto user: user_insts_map_[inst]) {
                    scheduleLate(user);
                }
            }
        }
    }
    // printf("begin remove codes\n");
}


void GlobalCodeMotion::collectUserSets() {
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            for (int i = 0; i < inst->getOperandNum(); ++i) {
                auto operand = inst->getOperand(i);
                if (operand->getValueType() ==  InstructionValue) {
                    user_insts_map_[dynamic_cast<Instruction *>(operand)].insert(inst);
                }
            }
        }
    }
}

void GlobalCodeMotion::scheduleEarly(Instruction *inst) {
    if (visited_insts_.count(inst)) {
        return;
    }
    visited_insts_.insert(inst);
    inst_block_map_[inst] = root_block_in_currfunc_;

    assert(inst_block_map_[inst]);
    for (int i = 0; i < inst->getOperandNum(); ++i) {
        auto operand = inst->getOperand(i);
        if (auto input = dynamic_cast<Instruction *>(operand); input) {
            scheduleEarly(input);
            auto input_block = inst_block_map_[input];
            auto inst_block = inst_block_map_[inst];
            auto inst_block_domdepth = compute_doms_->getDomDepth(inst_block);
            auto input_block_domdepth = compute_doms_->getDomDepth(input_block);
            /*printf("the inst block %s dom depth is %d, and the input block %s dom depth is %d\n",
                   inst_block->getName().c_str(), inst_block_domdepth, input_block->getName().c_str(), input_block_domdepth);*/
            if (inst_block_domdepth < input_block_domdepth) {
                /*printf("the inst block %s dom depth is %d, and the input block %s dom depth is %d\n",
                       inst_block->getName().c_str(), inst_block_domdepth, input_block->getName().c_str(), input_block_domdepth);*/
                inst_block_map_[inst] = inst_block_map_[input];
            }
        }
    }

    // printf("the early block of %s is %s\n", inst->getName().c_str(), inst_block_map_[inst]->getName().c_str());
}

BasicBlock *GlobalCodeMotion::findLCA(BasicBlock *blocka, BasicBlock *blockb) {
    BasicBlock *tmp_a = blocka, *tmp_b = blockb;
    if (!tmp_a) {
        return tmp_b;
    }

    while (compute_doms_->getDomDepth(tmp_a) > compute_doms_->getDomDepth(tmp_b)) {
        // printf("tmp_a is %s in findLCA\n", tmp_a->getName().c_str());
        tmp_a = compute_doms_->getImmDomsMap(tmp_a);
    }
    while (compute_doms_->getDomDepth(tmp_b) > compute_doms_->getDomDepth(tmp_a)) {
        // printf("tmp_b is %s in findLCA\n", tmp_b->getName().c_str());
        tmp_b = compute_doms_->getImmDomsMap(tmp_b);
    }

    while (tmp_a != tmp_b) {
        // printf("tmp_a is %s in findLCA, tmp_b is %s in findLCA\n", tmp_a->getName().c_str(), tmp_b->getName().c_str());
        tmp_a = compute_doms_->getImmDomsMap(tmp_a);
        tmp_b = compute_doms_->getImmDomsMap(tmp_b);
    }

    return tmp_a;
}


void GlobalCodeMotion::scheduleLate(Instruction *inst) {
    if (visited_insts_.count(inst)) {
        return;
    }

    visited_insts_.insert(inst);
    BasicBlock *lca = nullptr;
    for (auto user: user_insts_map_[inst]) {
        scheduleLate(user);
        BasicBlock *user_block = inst_block_map_[user];
        // if user is a phi inst
        if (auto phi_user_inst = dynamic_cast<PhiInstruction *>(user); phi_user_inst) {
            // pick j so that j th input of user is inst
            for (int i = 0; i < phi_user_inst->getSize(); ++i) {
                auto phi_value_bb = phi_user_inst->getValueBlock(i);
                // printf("the value is %s, block is %s in phi %s\n", phi_value_bb.first->getName().c_str(), phi_value_bb.second->getName().c_str(), phi_user_inst->getName().c_str());
                if (phi_value_bb.first == inst) {
                    user_block = phi_value_bb.second;
                }
            }
        }
        // printf("the user %s block is %s\n", user->getName().c_str(), user_block->getName().c_str());
        lca = findLCA(lca, user_block);
    }
    if (lca) {      // 如果lca能够找
        /*printf("the lca is %s end for inst %s ane the inst block is %s in function %s...\n", lca->getName().c_str(), inst->getName().c_str(),
               inst_block_map_[inst]->getName().c_str(), curr_func_->getName().c_str());*/
        if (isPinned(inst)) {  // 有几种坚决不进行移动的类型
            // printf("return because %s is phi inst\n", inst->getName().c_str());
            return;
        }
        // 选择一个block，此时block map中的block仍然是在early调度中的结果, lca可以视为范围中dom深度最浅的，而early中计算的则是depth最小的节点，在该范围内迭代。
        BasicBlock *best = lca;
        // printf("select a node between %s and %s,", lca->getName().c_str(), inst_block_map_[inst]->getName().c_str());
        while (lca != inst_block_map_[inst]) {
            if (compute_loops_->getBasicBlockLoopDepth(lca) < compute_loops_->getBasicBlockLoopDepth(best)) {
                best = lca;
            }
            // printf("lca is %s and instlockmap is %s\n", lca->getName().c_str(), inst_block_map_[inst]->getName().c_str());
            lca = compute_doms_->getImmDomsMap(lca);
        }
        // printf("the best node is %s\n", best->getName().c_str());
        // best就是确定要移动到的点
        best_block_map_[inst] = best;
        if (best != inst->getParent()) {
            printf("the inst %s will be move from %s to %s\n", inst->getName().c_str(), inst->getParent()->getName().c_str(), best->getName().c_str());
        }
    }
}
