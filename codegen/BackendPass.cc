//
// Created by 杨丰硕 on 2023/7/7.
//

#include "BackendPass.h"
#include "Machine.h"
#include "MachineDumper.h"
#include <cassert>

static bool isBranchInst(MachineInst *mc_inst) {
    auto br_inst = dynamic_cast<BranchInst *>(mc_inst);
    return br_inst && br_inst->getBranchType() != BranchInst::Bl && br_inst->getBranchType() != BranchInst::Bx;
}

static BranchInst::BranchCond getReCond(BranchInst::BranchCond cond) {
    switch (cond) {
        case BranchInst::BrEq:
            return BranchInst::BrNe;
        case BranchInst::BrNe:
            return BranchInst::BrEq;
        case BranchInst::BrGe:
            return BranchInst::BrLt;
        case BranchInst::BrLt:
            return BranchInst::BrGe;
        case BranchInst::BrGt:
            return BranchInst::BrLe;
        case BranchInst::BrLe:
            return BranchInst::BrGt;
        default:
            return BranchInst::BrNoCond;
    }
}

static MachineReg::Reg getReg(MachineOperand *operand) {
    if (auto vreg = dynamic_cast<VirtualReg *>(operand); vreg) {
        return vreg->getColoredReg();
    } else if (auto mreg = dynamic_cast<MachineReg *>(operand); mreg) {
        return mreg->getReg();
    }
    return MachineReg::undef;
}

BackendPass::BackendPass(MachineModule *module): module_(module), curr_func_(nullptr) {}

void BackendPass::run() {
    for (auto &func: module_->getMachineFunctions()) {
        curr_func_ = func.get();
        runOnFunction();
    }
}

BackendPassManager::BackendPassManager(MachineModule *module): module_(module) {}

ReDundantLoadElim::ReDundantLoadElim(MachineModule *module): BackendPass(module) {}

void ReDundantLoadElim::runOnFunction() {

    remove_insts_.clear();

    auto get_mreg = [&](MachineOperand *operand) -> MachineReg::Reg {
        MachineReg::Reg value_reg;
        if (operand->getOperandType() == MachineOperand::MachineReg) {
            auto store_value_mreg = dynamic_cast<MachineReg *>(operand);
            value_reg = store_value_mreg->getReg();
        } else if (operand->getOperandType() == MachineOperand::VirtualReg) {
            auto store_value_vreg = dynamic_cast<VirtualReg *>(operand);
            value_reg = store_value_vreg->getColoredReg();
        }
        return value_reg;
    };

    for (auto &bb_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto bb = bb_uptr.get();
        auto &insts_list = bb->getInstructionListNonConst();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
            if (auto store_inst = dynamic_cast<StoreInst *>(inst_it->get()); store_inst) {
                auto store_value = store_inst->getValue();
                auto store_value_mreg = get_mreg(store_value);              // value mreg
                auto store_base_mreg = get_mreg(store_inst->getBase());     // base mreg
                auto store_offset = store_inst->getOffset();              // offset value
                int32_t store_offset_imm = -1;
                MachineReg::Reg store_offset_mreg = MachineReg::Reg::undef;
                bool store_is_offset_imm = false;
                if (store_offset) {
                    // 是否是立即数的形式保存
                    if (store_offset->getOperandType() == MachineOperand::ImmNumber) {
                        store_offset_imm = dynamic_cast<ImmNumber *>(store_offset)->getIValue();
                        // printf("the store offset imm is %d\n", store_offset_imm);
                        store_is_offset_imm = true;
                    } else {
                        store_value_mreg = get_mreg(store_offset);
                    }
                }

                inst_it++;
                if (auto load_inst = dynamic_cast<LoadInst *>(inst_it->get()); load_inst) {
                    auto load_dst_mreg = get_mreg(load_inst->getDst());
                    auto load_base_mreg = get_mreg(load_inst->getBase());
                    auto load_offset = load_inst->getOffset();

                    int32_t load_offset_imm = -1;
                    MachineReg::Reg load_offset_mreg = MachineReg::Reg::undef;
                    bool load_is_offset_imm = false;
                    if (load_offset && store_offset) {
                        if (load_offset->getOperandType() == MachineOperand::ImmNumber) {
                            load_offset_imm = dynamic_cast<ImmNumber *>(load_offset)->getIValue();
                            load_is_offset_imm = true;
                        } else {
                            load_offset_mreg = get_mreg(load_offset);
                        }
                    }
                    if (load_dst_mreg != store_value_mreg || store_base_mreg != load_base_mreg) {
                        inst_it--;
                        continue;
                    }

                    if ((!load_offset && !store_offset)
                        ||(load_is_offset_imm && store_is_offset_imm && load_offset_imm == store_offset_imm)
                        || (!load_is_offset_imm && !store_is_offset_imm && load_offset_mreg == store_offset_mreg)) {
                        // printf("erase redandunt load inst\n");
                        inst_it = insts_list.erase(inst_it);
                    }

                }
                inst_it--;
            }
        }
    }

}

BlocksMergePass::BlocksMergePass(MachineModule *module): BackendPass(module) {}

void BlocksMergePass::init() {
    for (auto &mc_block_uptr: curr_func_->getMachineBasicBlock()) {
        auto mc_block = mc_block_uptr.get();
        name_block_map_[mc_block->getLabelName()] = mc_block;
    }
}

bool BlocksMergePass::canBeMerged(MachineBasicBlock *mc_block) {
    if (predecessors_map_[mc_block].size() == 1) {
        auto predecessor = *predecessors_map_[mc_block].begin();
        return successors_map_[predecessor].size() == 1;
    }
    return false;
}

void BlocksMergePass::mergeBlock(MachineBasicBlock *mc_block) {
    auto predecessor = *predecessors_map_[mc_block].begin();
    //printf("merge block %s to predecessor %s\n", mc_block->getLabelName().c_str(), predecessor->getLabelName().c_str());
    // 定位到待插入的位置
    auto &pre_insts_list = predecessor->getInstructionListNonConst();
    auto &inst_list = mc_block->getInstructionListNonConst();
    auto pre_insts_it = pre_insts_list.begin();
    while (pre_insts_it != pre_insts_list.end()) {
        auto inst = pre_insts_it->get();
        if (isBranchInst(inst)) {
            --pre_insts_it;
            break;
        }
        ++pre_insts_it;
    }
    // 插入mc block中的指令
    std::list<MachineInst *> insert_insts;
    for (auto insts_it = inst_list.begin(); insts_it != inst_list.end();) {
        auto inst = insts_it->get();
        insts_it->release();
        insert_insts.push_front(inst);
        insts_it = inst_list.erase(insts_it);
    }
    for (auto insert_inst: insert_insts) {
        predecessor->insertInstruction(pre_insts_it, insert_inst);
    }
    // 移除predecessor中的br
    pre_insts_list.pop_back();
}

void BlocksMergePass::mergeBlocks() {
    bool has_changed = true;
    while (has_changed) {
        buildCfgs();

        McBlocksSet can_merge_blocks;
        for (auto &mc_block_uptr: curr_func_->getMachineBasicBlock()) {
            auto mc_block = mc_block_uptr.get();
            if (canBeMerged(mc_block)) {
                auto pre_block = *predecessors_map_[mc_block].begin();
                if (!can_merge_blocks.count(pre_block)) {
                    can_merge_blocks.insert(mc_block);
                }
            }
        }

        for (auto &mc_block: can_merge_blocks) {
            mergeBlock(mc_block);
        }

        has_changed = !can_merge_blocks.empty();
    }
}

void BlocksMergePass::collectBranchInsts() {
    for (auto &mc_block_uptr: curr_func_->getMachineBasicBlock()) {
        auto &mc_insts_list = mc_block_uptr->getInstructionListNonConst();
        for (auto &mc_inst_uptr: mc_insts_list) {
            auto mc_inst = mc_inst_uptr.get();
            if (isBranchInst(mc_inst)) {
                auto br_inst = dynamic_cast<BranchInst *>(mc_inst);
                auto br_label = dynamic_cast<Label *>(br_inst->getOperand());
                bb_br_insts_map_[name_block_map_[br_label->getName()]].insert(mc_inst);
            }
        }
    }
}

void BlocksMergePass::clearEmptyBlocks() {
    buildCfgs();
    auto enter_block = curr_func_->getEnterBasicBlock();
    for (auto &mc_block_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto mc_block = mc_block_uptr.get();
        if (predecessors_map_[mc_block].empty() && mc_block != enter_block) {
            auto &mc_insts_list = mc_block->getInstructionListNonConst();
            mc_insts_list.clear();  // 清空
        }
    }
}

void BlocksMergePass::simplifyBranch() {            // 处理
    collectBranchInsts();
    bool has_changed = true;
    while (has_changed) {
        has_changed = false;

        std::vector<MachineBasicBlock *> can_simplify_blocks;
        for (auto &mc_block_uptr: curr_func_->getMachineBasicBlock()) {
            auto &insts_list = mc_block_uptr->getInstructionListNonConst();
            for (auto &inst_uptr: insts_list) {
                auto inst = inst_uptr.get();
                if (isBranchInst(inst)) {
                    auto br_inst = dynamic_cast<BranchInst *>(inst);
                    auto br_target_label = dynamic_cast<Label *>(br_inst->getOperand());
                    auto br_target_bb = name_block_map_[br_target_label->getName()];
                    if (br_target_bb->getInstructionListNonConst().size() == 1) {
                        auto target_inst = br_target_bb->getInstructionListNonConst().begin()->get();
                        if (isBranchInst(target_inst)) {
                            auto target_br_inst = dynamic_cast<BranchInst *>(target_inst);
                            auto target_br_label = dynamic_cast<Label *>(target_br_inst->getOperand());
                            // br_inst->replaceUses(br_inst->getOperand(), target_br_label);
                            br_inst->setOperand(target_br_label);
                            has_changed = true;
                        }
                    }
                }
            }
        }
    }
}

void BlocksMergePass::buildCfgs() {
    successors_map_.clear();
    predecessors_map_.clear();
    for (auto &mc_block_uptr: curr_func_->getMachineBasicBlock()) {
        auto mc_block = mc_block_uptr.get();
        auto &mc_insts_list = mc_block->getInstructionList();

        for (auto &mc_inst_uptr: mc_insts_list) {
            auto mc_inst = mc_inst_uptr.get();

            if (isBranchInst(mc_inst)) {
                auto br_inst = dynamic_cast<BranchInst *>(mc_inst);
                auto target_block_label = dynamic_cast<Label *>(br_inst->getOperand());
                if (target_block_label) {
                    auto target_block = name_block_map_[target_block_label->getName()];
                    successors_map_[mc_block].insert(target_block);
                    predecessors_map_[target_block].insert(mc_block);
                }
            }
        }
    }
}

void BlocksMergePass::runOnFunction() {
    init();
    simplifyBranch();
    clearEmptyBlocks();
    mergeBlocks();
}

SimplifyBackendBranch::SimplifyBackendBranch(MachineModule *module): BackendPass(module) {}

void SimplifyBackendBranch::runOnFunction() {
    removed_insts_.clear();
    block_branch_inst_map_.clear();
    collectBranchInsts();
    MachineBasicBlock *last_block = nullptr;
    for (auto &bb_uptr: curr_func_->getMachineBasicBlock()) {
        if (last_block && block_branch_inst_map_[last_block].size() == 2) {
            auto br_set_it = block_branch_inst_map_[last_block].begin();
            auto *br_with_cond = *br_set_it;
            br_set_it++;
            auto *br_without_cond = *br_set_it;
            if (br_with_cond->getBranchCond() == BranchInst::BrNoCond) {
                std::swap(br_with_cond, br_without_cond);
            }
            auto cond_target_label = dynamic_cast<Label *>(br_with_cond->getOperand());
            if (cond_target_label->getName() == bb_uptr->getLabelName()) {
                auto br_inst_cond = br_with_cond->getBranchCond();
                removed_insts_.insert(br_with_cond);
                br_without_cond->setBrCond(getReCond(br_inst_cond));
            }
        }
        last_block = bb_uptr.get();
    }

    removeInsts();

    last_block = nullptr;
    for (auto &bb_uptr: curr_func_->getMachineBasicBlock()) {
        auto curr_bb = bb_uptr.get();
        if (curr_bb->isEmpty()) {
            continue;
        }
        if (last_block) {
            auto end_inst = last_block->getInstructionListNonConst().back().get();
            if (auto end_br_inst = dynamic_cast<BranchInst *>(end_inst); end_inst && block_branch_inst_map_[last_block].count(end_br_inst) && end_br_inst->getBranchCond() == BranchInst::BrNoCond) {
                auto target_label = dynamic_cast<Label *>(end_br_inst->getOperand());
                if (target_label->getName() == curr_bb->getLabelName()) {
                    removed_insts_.insert(end_inst);
                    // printf("remove end br inst with target %s in block %s end\n", target_label->getName().c_str(), end_inst->getParent()->getLabelName().c_str());
                }
            }
        }
        last_block = curr_bb;
    }

    removeInsts();
}

void SimplifyBackendBranch::collectBranchInsts() {
    for (auto &bb_uptr: curr_func_->getMachineBasicBlock()) {
        auto &bb_insts = bb_uptr->getInstructionListNonConst();
        for (auto &inst_uptr: bb_insts) {
            auto inst = inst_uptr.get();
            if (isBranchInst(inst)) {
                block_branch_inst_map_[bb_uptr.get()].insert(dynamic_cast<BranchInst *>(inst));
            }
        }
    }
}

void SimplifyBackendBranch::removeInsts() {
    for (auto &bb_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto &bb_insts = bb_uptr->getInstructionListNonConst();
        for (auto inst_it = bb_insts.begin(); inst_it != bb_insts.end();) {
            if (removed_insts_.count(inst_it->get())) {
                inst_it = bb_insts.erase(inst_it);
            } else {
                inst_it++;
            }
        }
    }
}

MergeInsts::MergeInsts(MachineModule *module): BackendPass(module) {}


void MergeInsts::runOnFunction() {
    MachineInst *last_inst = nullptr;
    MachineInst *last_last_inst = nullptr;
    std::unordered_set<MachineInst *> need_remove_insts;
    for (auto &bb_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto curr_bb = bb_uptr.get();
        last_inst = nullptr;
        last_last_inst = nullptr;
        for (auto &inst_uptr: curr_bb->getInstructionListNonConst()) {
            if (last_inst && last_last_inst) {
                auto load_inst = dynamic_cast<LoadInst *>(inst_uptr.get());
                auto mov_inst = dynamic_cast<MoveInst *>(last_last_inst);
                auto add_inst = dynamic_cast<BinaryInst *>(last_inst);
                if (mov_inst && add_inst && add_inst->getBinaryOp() == BinaryInst::IAdd) {
                    auto mov_dst = mov_inst->getDst();
                    auto mov_src = mov_inst->getSrc();

                    auto mov_dst_reg = getReg(mov_dst);
                    auto add_dst_reg = getReg(add_inst->getDst());
                    auto add_left_reg = getReg(add_inst->getLeft());

                    if (mov_dst_reg == add_dst_reg && add_dst_reg == add_left_reg) {
                        if (load_inst) {
                            auto load_dst_reg = getReg(load_inst->getDst());
                            auto load_base_reg = getReg(load_inst->getBase());

                            if (mov_dst_reg == load_dst_reg && load_base_reg == load_dst_reg) {
                                // 重新设置ldr并且将
                                need_remove_insts.insert(mov_inst);
                                need_remove_insts.insert(add_inst);

                                load_inst->setBase(mov_src);
                                load_inst->setOffset(add_inst->getRight());
                                if (add_inst->isLsl()) {
                                    load_inst->setLsl(add_inst->getRhsLsr());
                                }
                            }
                        } else {
                            printf("remove mov for add inst\n");
                            need_remove_insts.insert(mov_inst);
                            add_inst->setLeft(mov_src);
                        }
                    }
                }
            }
            last_last_inst = last_inst;
            last_inst = inst_uptr.get();
        }
    }

    for (auto &bb_uptr: curr_func_->getMachineBasicBlockNoConst()) {
        auto &bb_insts = bb_uptr->getInstructionListNonConst();
        for (auto inst_it = bb_insts.begin(); inst_it != bb_insts.end();) {
            if (need_remove_insts.count(inst_it->get())) {
                inst_it = bb_insts.erase(inst_it);
            } else {
                inst_it++;
            }
        }
    }

}
