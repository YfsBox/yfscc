//
// Created by 杨丰硕 on 2023/6/21.
//
#include <cassert>
#include <queue>
#include "ComputeLoops.h"
#include "../ir/Value.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"

Instruction *ComputeLoops::LoopInfo::getSetCondInst() {
    auto rit = enter_block_->getInstructionList().rbegin();
    rit++;
    return rit->get();
}

PhiInstruction *ComputeLoops::LoopInfo::getCondVarPhiInst() {
    SetCondInstruction *setcond_inst = dynamic_cast<SetCondInstruction *>(getSetCondInst());
    assert(setcond_inst);

    auto lhs = setcond_inst->getLeft();
    auto rhs = setcond_inst->getRight();

    auto lhs_phi_inst = dynamic_cast<PhiInstruction *>(lhs);
    auto rhs_phi_inst = dynamic_cast<PhiInstruction *>(rhs);

    if (lhs->getValueType() == ConstantValue && rhs_phi_inst) {
        return dynamic_cast<PhiInstruction *>(rhs);
    } else if (lhs_phi_inst && rhs->getValueType() == ConstantValue) {
        return dynamic_cast<PhiInstruction *>(lhs);
    } else {
        if (lhs_phi_inst) {
            auto rhs_inst = dynamic_cast<Instruction *>(rhs);
            if ((rhs_inst && !isInLoop(rhs_inst->getParent())) || !rhs_inst) {
                return lhs_phi_inst;
            }
        } else if (rhs_phi_inst) {
            auto lhs_inst = dynamic_cast<Instruction *>(lhs);
            if ((lhs_inst && !isInLoop(lhs_inst->getParent())) || !lhs_inst) {
                return rhs_phi_inst;
            }
        }
    }

    return nullptr;
}

void ComputeLoops::LoopInfo::setHasReturnOrBreak() {
    for (auto body_bb: loop_body_) {
        auto terminal_inst = body_bb->getInstructionList().back().get();
        if (auto ret_inst = dynamic_cast<RetInstruction *>(terminal_inst); ret_inst) {
            has_ret_or_break_ = false;
            return;
        }

        if (auto branch_inst = dynamic_cast<BranchInstruction *>(terminal_inst); branch_inst) {
            if (branch_inst->isCondBranch()) {
                auto br_label = dynamic_cast<BasicBlock *>(branch_inst->getLabel());
                if (!loop_body_.count(br_label)) {
                    has_ret_or_break_ = false;
                    return;
                }
            } else {
                auto true_label = dynamic_cast<BasicBlock *>(branch_inst->getTrueLabel());
                auto false_label = dynamic_cast<BasicBlock *>(branch_inst->getFalseLabel());

                if ((!loop_body_.count(true_label) && true_label != exit_block_)
                || (!loop_body_.count(false_label) && false_label != exit_block_)) {
                    has_ret_or_break_ = false;
                    return;
                }
            }
        }
    }
    has_ret_or_break_ = true;
}

void ComputeLoops::LoopInfo::setNextBasicBlock() {
    auto branch_inst = dynamic_cast<BranchInstruction *>(enter_block_->getInstructionList().back().get());
    assert(branch_inst);
    auto true_label = dynamic_cast<BasicBlock *>(branch_inst->getTrueLabel());
    auto false_label = dynamic_cast<BasicBlock *>(branch_inst->getFalseLabel());

    if (loop_body_.count(true_label)) {
        next_block_ = false_label;
    } else {
        next_block_ = true_label;
    }
}

void ComputeLoops::LoopInfo::setInteratorVarPhiInsts() {
    auto &enter_block_insts = enter_block_->getInstructionList();
    for (auto &inst: enter_block_insts) {
        if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst.get()); phi_inst) {
            if (phi_inst->getSize() == 2) {
                auto value_bb_0 = phi_inst->getValueBlock(0);
                auto value_bb_1 = phi_inst->getValueBlock(1);

                if (!isInLoop(value_bb_0.second) && value_bb_1.second == exit_block_) {
                    // printf("phi inst %s set as %s\n", phi_inst->getName().c_str(), value_bb_1.first->getName().c_str());
                    init_var_phi_insts_[phi_inst] = value_bb_0.first;
                    iterator_var_phi_insts_[phi_inst] = value_bb_1.first;
                } else {
                    // printf("phi inst %s set as %s\n", phi_inst->getName().c_str(), value_bb_0.first->getName().c_str());
                    init_var_phi_insts_[phi_inst] = value_bb_1.first;
                    iterator_var_phi_insts_[phi_inst] = value_bb_0.first;
                }
            }
        }
    }
}

bool ComputeLoops::LoopInfo::isInLoop(BasicBlock *basicblock) {
    return basicblock == enter_block_ || basicblock == exit_block_ || loop_body_.count(basicblock);
}

void ComputeLoops::dfsBasicBlocks(BasicBlock *basicblock, int index) {
    if (visited_blocks_.count(basicblock)) {
        return;
    }

    visited_blocks_.insert(basicblock);
    basicblock_index_[basicblock] = index;

    for (auto succ: basicblock->getSuccessorBlocks()) {
        dfsBasicBlocks(succ, index + 1);
    }

    for (auto pre_bb: basicblock->getPreDecessorBlocks()) {
        if (visited_blocks_.count(pre_bb) && basicblock_index_[pre_bb] > basicblock_index_[basicblock]) {
            auto doms = compute_dominators_->getDoms(pre_bb);
            if (doms.count(basicblock)) {
                auto loop = std::make_shared<LoopInfo>();
                loop->enter_block_ = basicblock;
                loop->exit_block_ = pre_bb;

                func_loopinfos_list_[function_].push_back(loop);
                computeLoopBody(loop);
            }
        }
    }

}

void ComputeLoops::computeLoopBody(const LoopInfoPtr &loopinfo) {
    std::unordered_set<BasicBlock *> visited_blocks;
    std::queue<BasicBlock *> bb_q;
    bb_q.push(loopinfo->exit_block_);

    while (!bb_q.empty()) {
        auto front_bb = bb_q.front();
        bb_q.pop();

        assert(front_bb);

        if (visited_blocks.count(front_bb) || front_bb == loopinfo->enter_block_) {
            continue;
        }

        visited_blocks.insert(front_bb);
        loopinfo->loop_body_.insert(front_bb);
        loopinfo->loop_body_list_.push_front(front_bb);

        if (loop_info_map_.find(front_bb) != loop_info_map_.end()) {
            loop_info_map_[front_bb]->parent_info_ = loopinfo;
            loopinfo->sub_loops_.insert(front_bb);
        }
        loop_info_map_[front_bb] = loopinfo;
        visited_blocks.insert(front_bb);

        for (auto pred: front_bb->getPreDecessorBlocks()) {
            bb_q.push(pred);
        }
    }

}

void ComputeLoops::setDeepestLoops(Function *function) {
    auto &loopinfos_list = getLoopInfosList(function);
    for (auto &loopinfo: loopinfos_list) {
        if (loopinfo->getSubLoops().empty()) {
            deepest_loops_[function].push_back(loopinfo);
        }
    }
}

ComputeLoops::LoopInfosList &ComputeLoops::getLoopInfosList(Function *function) {
    return func_loopinfos_list_[function];
}

ComputeLoops::LoopInfosList &ComputeLoops::getDeepestLoops(Function *function) {
    return deepest_loops_[function];
}

void ComputeLoops::computeLoopDepths() {
    auto &loopinfos = func_loopinfos_list_[function_];
    int loop_cnt = loopinfos.size();
    for (int i = loop_cnt - 1; i >= 0; i--) {
        auto enter_block = loopinfos[i]->enter_block_;
        bool nested = false;
        for (int j = i + 1; j < loop_cnt; ++j) {
            if (loopinfos[j]->loop_body_.count(enter_block)) {
                nested = true;
                if (loopinfos[j]->enter_block_ == enter_block) {
                    loops_depth_[loopinfos[i].get()] = loops_depth_[loopinfos[j].get()];
                } else {
                    loops_depth_[loopinfos[i].get()] = loops_depth_[loopinfos[j].get()] + 1;
                }
                break;
            }
        }
        if (!nested) {
            loops_depth_[loopinfos[i].get()] = 1;
        }
    }
    // 计算出block的所在的loop的depth
    for (int i = loop_cnt - 1; i >= 0; i--) {
        auto &loop = loopinfos[i];
        for (auto body: loop->loop_body_) {
            if (loops_depth_[loop.get()] > basicblock_loopdepth_map_[body]) {
                basicblock_loopdepth_map_[body] = loops_depth_[loop.get()];
                body->setWhileLoopDepth(basicblock_loopdepth_map_[body]);
            }
        }

        auto loop_enter = loopinfos[i]->enter_block_;
        if (loops_depth_[loop.get()] > basicblock_loopdepth_map_[loop_enter]) {
            basicblock_loopdepth_map_[loop_enter] = loops_depth_[loop.get()];
            loop_enter->setWhileLoopDepth(loops_depth_[loop.get()]);
        }
    }
}

void ComputeLoops::run() {
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);
        // printf("the function is %s\n", function->getName().c_str());
        if (function->isDead()) {
            continue;
        }
        if (function->getBlocks().size() < 1) {
            continue;
        }
        function_ = function;

        if (compute_dominators_) {
            compute_dominators_ = nullptr;
        }
        compute_dominators_ = std::make_unique<ComputeDominators>(function);
        compute_dominators_->run();

        assert(function->getBlocks().size() > 0);
        auto enter_block = function->getBlocks().front().get();
        visited_blocks_.clear();
        dfsBasicBlocks(enter_block, 0);
        computeLoopDepths();
    }

    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);
        setDeepestLoops(function);
        auto &loopinfo_list = func_loopinfos_list_[function];
        for (auto &loopinfo: loopinfo_list) {
            loopinfo->setHasReturnOrBreak();
            loopinfo->setNextBasicBlock();
            loopinfo->setInteratorVarPhiInsts();
        }
    }
}