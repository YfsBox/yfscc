//
// Created by 杨丰硕 on 2023/6/27.
//
#include <memory>
#include "GlobalCodeMotion.h"
#include "ComputeLoops.h"
#include "ComputeDominators.h"
#include "GlobalAnalysis.h"
#include "CallGraphAnalysis.h"
#include "../ir/Instruction.h"
#include "../ir/BasicBlock.h"
#include "../ir/IrDumper.h"
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"

GlobalCodeMotion::GlobalCodeMotion(Module *module): Pass(module),
    has_compute_loop_(false),
    root_block_in_currfunc_(nullptr),
    user_analysis_(std::make_unique<UserAnalysis>()),
    callgraph_analysis_(std::make_unique<CallGraphAnalysis>(module_)),
    compute_loops_(std::make_unique<ComputeLoops>(module)),
    compute_doms_(nullptr) {

}

bool GlobalCodeMotion::isPinned(const Value *value) {
    if (auto inst = dynamic_cast<const Instruction *>(value); inst) {
        auto inst_type = inst->getInstType();
        return inst_type == PhiType || inst_type == RetType || inst_type == SetCondType
               || inst_type == BrType || inst_type == LoadType
               || inst_type == StoreType || (inst_type == CallType && callgraph_analysis_->hasSideEffect(dynamic_cast<const CallInstruction *>(inst)->getFunction()))
               || inst_type == MemSetType;
    }
    return value->getValueType() == ArgumentValue;
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
    pinned_values_.clear();
    visited_insts_.clear();
    move_insts_map_.clear();
    wait_move_insts_set_.clear();
    // schedule early
    // printf("begin schedule early\n");
    root_block_in_currfunc_ = curr_func_->getBlocks().front().get();
    user_analysis_->analysis(curr_func_);
    callgraph_analysis_->analysis();
    collectPinnedValues();

    for (auto pinned_inst_value: pinned_values_) {
        if (auto pinned_inst = dynamic_cast<Instruction *>(pinned_inst_value); pinned_inst) {
            visited_insts_.insert(pinned_inst);
            for (int i = 0; i < pinned_inst->getOperandNum(); ++i) {
                auto operand = pinned_inst->getOperand(i);
                if (operand->getValueType() == InstructionValue) {
                    scheduleEarly(dynamic_cast<Instruction *>(operand));
                }
            }
        }
    }
    // schedule lately
    // printf("begin schedule lately\n");
    visited_insts_.clear();
    for (auto pinned_value: pinned_values_) {
        visited_insts_.insert(pinned_value);
        for (auto user: user_analysis_->getUserInsts(pinned_value)) {
            scheduleLate(user);
        }
    }
    moveInsts();
}


void GlobalCodeMotion::scheduleEarly(Instruction *inst) {
    if (visited_insts_.count(inst)) {
        return;
    }

    if (isPinned(inst)) {
        return;
    }

    visited_insts_.insert(inst);
    // printf("visit inst %s in %s in early\n", inst->getName().c_str(), curr_func_->getName().c_str());
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

void GlobalCodeMotion::moveInsts() {
    std::unordered_map<BasicBlock *, std::list<Instruction *>> insert_insts;
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            auto inst = inst_uptr.get();
            if (wait_move_insts_set_.count(inst)) {
                move_insts_map_[inst_block_map_[inst]].push_back(inst);
            }
        }
    }

    /*for (auto &[basicblock, insts_list]: move_insts_map_) {
        printf("the basicblock is %s, some insts will move to here\n", basicblock->getName().c_str());
        for (auto inst: insts_list) {
            printf("the inst %s\n", inst->getName().c_str());
        }
    }*/

    // 首先在原处进行移除
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
            auto inst = inst_it->get();
            if (wait_move_insts_set_.count(inst)) {
                inst->setParent(inst_block_map_[inst]);
                inst_it->release();
                inst_it = insts_list.erase(inst_it);
            } else {
                inst_it++;
            }
        }
    }
    // 确定插入点，一般在倒数位置

    for (auto &[basicblock, insts_list]: move_insts_map_) {
        auto &basicblock_insts_list = basicblock->getInstructionList();
        auto insert_it = basicblock_insts_list.begin();
        std::unordered_set<Instruction *> late_insert_insts;
        for (; insert_it != basicblock_insts_list.end(); ++insert_it) {
            auto curr_inst = insert_it->get();
            if (curr_inst->getInstType() != PhiType) {
                break;
            }
        }

        for (auto &inst: basicblock_insts_list) {
            auto user_set = user_analysis_->getUserInsts(inst.get());
            for (auto insert_inst: insts_list) {
                if (user_set.count(insert_inst)) {
                    late_insert_insts.insert(insert_inst);
                }
            }
        }

        for (auto inst: insts_list) {
            if (!late_insert_insts.count(inst)) {
                basicblock->insertInstruction(insert_it, inst);
            }
            // printf("the inst size is %d\n", basicblock->getInstructionList().size());
        }

        for (; insert_it != basicblock_insts_list.end(); ++insert_it) {
            auto curr_inst = insert_it->get();
            if (curr_inst->getInstType() == BrType) {
                break;
            }
        }


        for (auto inst: insts_list) {
            if (late_insert_insts.count(inst)) {
                basicblock->insertInstruction(insert_it, inst);
            }
            // printf("the inst size is %d\n", basicblock->getInstructionList().size());
        }
        // ir_dumper_->dump(basicblock);
    }

}

void GlobalCodeMotion::collectPinnedValues() {
    auto enter_block = curr_func_->getBlocks().front().get();
    // 将全局变量也设置为global
    for (int i = 0; i < module_->getGlobalSize(); ++i) {
        auto global = module_->getGlobalVariable(i);
        pinned_values_.push_back(global);
        inst_block_map_[global] = enter_block;
    }

    // put the args of current function as pinned
    for (int i = 0; i < curr_func_->getArgumentSize(); ++i) {
        auto arg = curr_func_->getArgument(i);
        pinned_values_.push_back(arg);
        inst_block_map_[arg] = enter_block;
    }
    // collect inst pinned
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            auto inst = inst_uptr.get();
            if (isPinned(inst)) {
                pinned_values_.push_back(inst);
                inst_block_map_[inst] = inst->getParent();
            }
        }
    }
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

    if (isPinned(inst)) {
        return;
    }

    visited_insts_.insert(inst);
    // printf("visit inst %s in %s in late\n", inst->getName().c_str(), curr_func_->getName().c_str());
    BasicBlock *lca = nullptr;
    for (auto user: user_analysis_->getUserInsts(inst)) {
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
        // printf("the user %s block is %s for inst %s\n", user->getName().c_str(), user_block->getName().c_str(), inst->getName().c_str());
        lca = findLCA(lca, user_block);
    }

    assert(lca);

    if (lca) {      // 如果lca能够找
        // 选择一个block，此时block map中的block仍然是在early调度中的结果, lca可以视为范围中dom深度最浅的，而early中计算的则是depth最小的节点，在该范围内迭代。
        BasicBlock *best = lca;
        // printf("select a node between %s and %s for node %s in function %s\n", lca->getName().c_str(), inst_block_map_[inst]->getName().c_str(), inst->getName().c_str(), curr_func_->getName().c_str());
        while (lca != inst_block_map_[inst]) {
            if (compute_loops_->getBasicBlockLoopDepth(lca) < compute_loops_->getBasicBlockLoopDepth(best)) {
                best = lca;
            }
            // printf("lca is %s and instlockmap is %s\n", lca->getName().c_str(), inst_block_map_[inst]->getName().c_str());
            lca = compute_doms_->getImmDomsMap(lca);
        }
        // printf("the best node is %s\n", best->getName().c_str());
        best_block_map_[inst] = best;
        inst_block_map_[inst] = best;
        if (best != inst->getParent()) {
            // printf("the inst %s will be move from %s to %s\n", inst->getName().c_str(), inst->getParent()->getName().c_str(), best->getName().c_str());
            wait_move_insts_set_.insert(inst);
        }
    }
}
