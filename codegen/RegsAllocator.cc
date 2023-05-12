//
// Created by 杨丰硕 on 2023/5/2.
//
#include <cassert>
#include "MachineInst.h"
#include "RegsAllocator.h"
#include "../ir/BasicBlock.h"

bool RegsAllocator::isEqual(const BitSet &lhs, const BitSet &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (auto v: lhs) {
        if (rhs.find(v) == rhs.end()) {
            return false;
        }
    }
    return true;
}

void RegsAllocator::analyseLiveness(MachineFunction *function) {
    live_in_.clear();
    live_out_.clear();
    use_sets_.clear();
    def_sets_.clear();

    auto &basic_blocks = function->getMachineBasicBlock();
    size_t basic_blocks_size = basic_blocks.size();

    for (int i = 0; i < basic_blocks_size; ++i) {
        auto &bb = basic_blocks[i];
        auto &insts = bb->getInstructionList();
        BitSet tmp_use_sets;
        BitSet tmp_def_sets;
        for (auto &inst: insts) {
            auto inst_defs = MachineInst::getDefs(inst.get());
            auto inst_uses = MachineInst::getUses(inst.get());

            for (auto use: inst_uses) {
                if (use->getOperandType() == MachineOperand::VirtualReg && tmp_def_sets.find(use) == tmp_def_sets.end()) {
                    tmp_use_sets.insert(use);
                }
            }

            for (auto def: inst_defs) {
                tmp_def_sets.insert(def);
            }
        }
        use_sets_[bb.get()] = std::move(tmp_use_sets);
        def_sets_[bb.get()] = std::move(tmp_def_sets);
    }

    // 进入存活变量的迭代循环中
    bool has_changed = true;

    while (has_changed) {
        has_changed = false;
        for (int i = 0; i < basic_blocks_size; ++i) {
            auto bb = basic_blocks[i].get();
            auto old_in = live_in_[bb];
            auto old_out = live_out_[bb];

            live_in_[bb] = use_sets_[bb];
            for (auto &out: live_out_[bb]) {
                if (def_sets_[bb].find(out) == def_sets_[bb].end()) {
                    live_in_[bb].insert(out);
                }
            }

            live_out_[bb].clear();
            auto ir_bb = machine_module_->getBasicBlock(bb);
            assert(ir_bb);

            for (auto succ: ir_bb->getSuccessorBlocks()) {
                auto mc_succ = machine_module_->getMachineBasicBlock(succ);
                for (auto succ_in: live_in_[mc_succ]) {
                    live_out_[bb].insert(succ_in);
                }
            }

            if (!has_changed) {
                if (!isEqual(live_in_[bb], old_in) || !isEqual(live_out_[bb], old_out)) {
                    has_changed = true;
                }
            }

        }
    }

}

