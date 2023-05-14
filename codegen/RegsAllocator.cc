//
// Created by 杨丰硕 on 2023/5/2.
//
#include <cassert>
#include <algorithm>
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

            live_in_[bb] = use_sets_[bb];       // live_in = use union (out - def)
            for (auto &out: live_out_[bb]) {
                if (def_sets_[bb].find(out) == def_sets_[bb].end()) {
                    live_in_[bb].insert(out);
                }       // 如果这个out的这一项是def中没有的，那么就将其加上
            }

            live_out_[bb].clear();
            auto ir_bb = machine_module_->getBasicBlock(bb);
            assert(ir_bb);

            for (auto succ: ir_bb->getSuccessorBlocks()) {      // 将每一个后继的IN进行union
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

bool RegsAllocator::isInAdjSet(MachineOperand *a, MachineOperand *b) {
    auto finda_it = adj_set_.find(a);
    auto findb_it = adj_set_.find(b);
    if (finda_it != adj_set_.end() && findb_it != adj_set_.end()) {
        return finda_it->second.count(b) && findb_it->second.count(a);
    }
    return false;
}

bool RegsAllocator::isPrecolored(MachineOperand *operand) {
    return false;
}

void RegsAllocator::addEdge(MachineOperand *a, MachineOperand *b) {
    if (!isInAdjSet(a, b) && a != b) {
        adj_set_[a].insert(b);
        adj_set_[b].insert(a);
        if (!isPrecolored(a)) {
            adj_list_[a].insert(b);
            degree_[a] = degree_[a] + 1;
        }
        if (!isPrecolored(b)) {
            adj_list_[b].insert(a);
            degree_[b] = degree_[b] + 1;
        }
    }
}

void RegsAllocator::build() {
    for (auto &mc_basicblock: curr_function_->getMachineBasicBlock()) {
        auto live = live_out_[mc_basicblock.get()];
        auto mc_inst_list = mc_basicblock->getInstructionList();

        for (auto it = mc_inst_list.rbegin(); it != mc_inst_list.rend(); ++it) {
            MachineInst *mc_inst = it->get();
            auto defs = MachineInst::getDefs(mc_inst);
            auto uses = MachineInst::getUses(mc_inst);
            if (mc_inst->getMachineInstType() == MachineInst::Move) {
                for (auto use : uses) {     // live :=  live \ use(I)
                    if (live.find(use) != live.end()) {
                        live.erase(use);
                    }
                    move_list_[use].insert(mc_inst);
                }

                for (auto def: defs) {
                    move_list_[def].insert(mc_inst);
                }
                worklist_moves_.insert(mc_inst);
            }
            live.insert(defs.begin(), defs.end());
            // add edges
            for (auto def: defs) {
                for (auto l: live) {
                    addEdge(l, def);
                }
            }
            // live := use(I) U (live \ def(I))
            std::unordered_set<MachineOperand *> tmp_set;
            std::set_difference(live.begin(), live.end(), defs.begin(), defs.end(), tmp_set.end());
            live.insert(uses.begin(), uses.end());
        }
    }
}


