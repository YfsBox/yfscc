//
// Created by 杨丰硕 on 2023/7/7.
//

#include "BackendPass.h"
#include "Machine.h"
#include "MachineDumper.h"
#include <cassert>

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
    // pre_insts_it++;
    // pre_insts_list.erase(pre_insts_it);
    /*for (pre_insts_it = pre_insts_list.begin(); pre_insts_it != pre_insts_list.end(); ++pre_insts_it) {
        auto pre_inst = pre_insts_it->get();
        if (isBranchInst(pre_inst)) {
            auto br_inst_target = dynamic_cast<BranchInst *>(pre_inst)->getOperand();
            auto br_inst_target_label = dynamic_cast<Label *>(br_inst_target);
            if (name_block_map_[br_inst_target_label->getName()] == mc_block) {
                pre_insts_list.erase(pre_insts_it);
                break;
            }
        }
    }*/
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

bool BlocksMergePass::isBranchInst(MachineInst *mc_inst) {
    auto br_inst = dynamic_cast<BranchInst *>(mc_inst);
    return br_inst && br_inst->getBranchType() != BranchInst::Bl && br_inst->getBranchType() != BranchInst::Bx;
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

