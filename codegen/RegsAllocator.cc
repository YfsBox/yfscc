//
// Created by 杨丰硕 on 2023/5/2.
//
#include <cassert>
#include <cmath>
#include <set>
#include <algorithm>
#include "CodeGen.h"
#include "Machine.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "RegsAllocator.h"
#include "MachineDumper.h"
#include "../ir/BasicBlock.h"

void RegsAllocator::regsAllocate(MachineModule *mc_module, CodeGen *codegen) {

    for (auto &func: mc_module->getMachineFunctions()) {
        // printf("the vrge count is %d, and inst number is %d\n", func->getVirtualRegs().size(), func->getInstSize());
        if (func->getVirtualRegs().size() * func->getInstSize() <= VREGS_THRSHOLD_VALUE * INST_THRSHOLD_VALUE) {
            ColoringRegsAllocator coloring(mc_module, codegen);
            coloring.allocate(func.get());
        } else {
            SimpleRegsAllocator simple(mc_module, codegen);
            simple.allocate(func.get());
        }
    }
}

SimpleRegsAllocator::SimpleRegsAllocator(MachineModule *module, CodeGen *codegen):
    RegsAllocator(module, codegen),
    spilled_stack_offset_(0){

}

void SimpleRegsAllocator::allocate(MachineFunction *func) {
    curr_function_ = func;
    runOnMachineFunction(func);
    if ((curr_function_->getStackSize() + spilled_stack_offset_) % 8 == 0) {
        spilled_stack_offset_ += 4;
    }
    code_gen_->addInstAboutStack(curr_function_, curr_function_->getStackSize() + spilled_stack_offset_);
}

void SimpleRegsAllocator::allocate() {

}

void SimpleRegsAllocator::runOnMachineFunction(MachineFunction *function) {

    for (auto vir_reg: function->getVirtualRegs()) {            // 需要根据当前是否处理float来分开处理
        spilled_stack_offset_ += 4;
        spilled_vregs_offset_[vir_reg] = spilled_stack_offset_;
    }

    for (auto &bb: curr_function_->getMachineBasicBlock()) {
        auto &inst_list = bb->getInstructionListNonConst();

        std::unordered_map<MachineInst *, std::vector<MachineInst *>> insert_before;
        std::unordered_map<MachineInst *, std::vector<MachineInst *>> insert_after;
        std::unordered_map<MachineInst *, MachineBasicBlock::MachineInstListIt> insert_it;

        for (auto it = inst_list.begin(); it != inst_list.end(); ++it) {
            auto inst = it->get();
            auto defs = MachineInst::getDefs(inst);
            auto uses = MachineInst::getUses(inst);

            for (auto def: defs) {
                if (def->getOperandType() != MachineOperand::VirtualReg) {
                    continue;
                }
                auto def_vreg = dynamic_cast<VirtualReg *>(def);
                MachineOperand::ValueType value_type = def_vreg->getValueType();
                allocate_float_ = value_type == MachineOperand::Float;
                assert(value_type == MachineOperand::Int || value_type == MachineOperand::Float);
                assert(spilled_vregs_offset_.count(def_vreg));

                auto spilled_offset = spilled_vregs_offset_[def_vreg];
                bool use_ip_base = false;
                std::vector<MachineInst *> moves_offset_insts;
                auto offset_reg = code_gen_->getImmOperandInBinary(- curr_function_->getStackSize() - spilled_offset, bb.get(), &moves_offset_insts, allocate_float_, &use_ip_base,true);
                auto store_vreg = allocate_float_ ? code_gen_->getMachineReg(true, 16) : code_gen_->getMachineReg(false, 4);
                // printf("the new vreg is %d, insert before vreg%d inst\n", store_vreg->getRegId(), dynamic_cast<VirtualReg *>(spill_node)->getRegId());
                assert(offset_reg);
                MachineInst *store_inst;
                if (!use_ip_base) {
                    store_inst = new StoreInst(MemIndexType::PostiveIndex, bb.get(), store_vreg, code_gen_->fp_reg_,
                                               offset_reg);
                } else {
                    store_inst = new StoreInst(bb.get(), store_vreg, offset_reg);
                }

                inst->replaceDefs(def, store_vreg);
                moves_offset_insts.push_back(store_inst);
                insert_after.insert({inst, moves_offset_insts});
                insert_it.insert({inst, it});
            }

            int32_t use_ireg_offset_no = 0;
            int32_t use_freg_offset_no = 0;
            for (auto use: uses) {
                if (use->getOperandType() != MachineOperand::VirtualReg) {
                    continue;
                }

                auto use_vreg = dynamic_cast<VirtualReg *>(use);
                MachineOperand::ValueType value_type = use_vreg->getValueType();
                allocate_float_ = value_type == MachineOperand::Float;
                assert(value_type == MachineOperand::Int || value_type == MachineOperand::Float);
                assert(spilled_vregs_offset_.count(use_vreg));

                auto spilled_offset = spilled_vregs_offset_[use_vreg];
                bool use_ip_base = false;
                std::vector<MachineInst *> moves_offset_insts;
                auto offset_reg = code_gen_->getImmOperandInBinary(- curr_function_->getStackSize() - spilled_offset, bb.get(), &moves_offset_insts, allocate_float_, &use_ip_base,true);
                auto load_vreg = allocate_float_ ? code_gen_->getMachineReg(true, 17 + use_freg_offset_no) : code_gen_->getMachineReg(false, 5 + use_ireg_offset_no);

                if (allocate_float_) {
                    use_freg_offset_no++;
                } else {
                    use_ireg_offset_no++;
                }

                MachineInst *load_inst;
                if (!use_ip_base) {
                    load_inst = new LoadInst(bb.get(), load_vreg, code_gen_->fp_reg_, offset_reg);
                } else {
                    load_inst = new LoadInst(bb.get(), load_vreg, offset_reg);
                }
                inst->replaceUses(use, load_vreg);
                moves_offset_insts.push_back(load_inst);
                insert_before[inst].insert(insert_before[inst].end(), moves_offset_insts.begin(), moves_offset_insts.end());
                insert_it.insert({inst, it});
            }
                // assert(both_inserted < 2);
        }


        // printf("the insert before size is %d, and the after inserted size is %d\n", insert_before.size(), insert_after.size());

        for (auto [inserted, insts]: insert_before) {
            auto find_inserted_it = insert_it.find(inserted);
            assert(find_inserted_it != insert_it.end());
            for (auto inst: insts) {
                bb->insertInstructionBefore(find_inserted_it->second, inst);
            }
        }

        for (auto [inserted, insts]: insert_after) {
            auto find_inserted_it = insert_it.find(inserted);
            assert(find_inserted_it != insert_it.end());
            for (auto rit = insts.rbegin(); rit != insts.rend(); ++rit) {
                bb->insertInstruction(find_inserted_it->second, *rit);
            }
        }
    }

}


ColoringRegsAllocator::ColoringRegsAllocator(MachineModule *module, CodeGen *codegen):
    RegsAllocator(module, codegen){
    int_regs_set_ = {
            MachineReg::r0, MachineReg::r1, MachineReg::r2, MachineReg::r3, MachineReg::r4,
            MachineReg::r5, MachineReg::r6, MachineReg::r7, MachineReg::r8, MachineReg::r9,
            MachineReg::r10, /*MachineReg::r12,*/ MachineReg::r14,
    };      // 没有r11, r13、r15，也就是没有fp、sp、pc
    float_regs_set_ = {
            MachineReg::s0, MachineReg::s1, MachineReg::s2, MachineReg::s3, MachineReg::s4,
            MachineReg::s5, MachineReg::s6, MachineReg::s7, MachineReg::s8, MachineReg::s9,
            MachineReg::s10, MachineReg::s11, MachineReg::s12, MachineReg::s13, MachineReg::s14,
            MachineReg::s15, MachineReg::s16, MachineReg::s17, MachineReg::s18, MachineReg::s19,
            MachineReg::s20, MachineReg::s21, MachineReg::s22, MachineReg::s23, MachineReg::s24,
            MachineReg::s25, MachineReg::s26, MachineReg::s27, MachineReg::s28, MachineReg::s29,
            MachineReg::s30, MachineReg::s31,
    };  // 没有特殊用途的寄存器，所以全部用上
    spilled_stack_size_ = 0;
}

bool ColoringRegsAllocator::isEqual(const BitSet &lhs, const BitSet &rhs) {
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

bool ColoringRegsAllocator::needAllocateForFloat() {
    for (auto vir_reg: curr_function_->getVirtualRegs()) {            // 需要根据当前是否处理float来分开处理
        if (vir_reg->getValueType() == MachineOperand::Int) {
            return true;
        }
    }
    return false;
}

void ColoringRegsAllocator::analyseLiveness(MachineFunction *function) {            // 数据流分析中，会对float和int类型的变量进行区分
    live_in_.clear();           // 其中在获取def、use时，也就需要区分不同的类型
    live_out_.clear();
    use_sets_.clear();
    def_sets_.clear();

    auto &basic_blocks = function->getMachineBasicBlock();
    size_t basic_blocks_size = basic_blocks.size();
    // printf("begin analyse liveness the vregs num is %d, and basicblock number is %d\n", curr_function_->getVirtualRegs().size(), basic_blocks_size);

    for (int i = 0; i < basic_blocks_size; ++i) {
        auto &bb = basic_blocks[i];
        auto &insts = bb->getInstructionList();
        BitSet tmp_use_sets;
        BitSet tmp_def_sets;
        for (auto &inst: insts) {
            auto inst_defs = MachineInst::getDefs(inst.get(), allocate_float_);
            auto inst_uses = MachineInst::getUses(inst.get(), allocate_float_);

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
            // printf("the old in size is %d, and the old out size is %d\n", live_in_[bb].size(), live_out_[bb].size());
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

bool ColoringRegsAllocator::isInAdjSet(MachineOperand *a, MachineOperand *b) {
    auto finda_it = adj_set_.find(a);
    auto findb_it = adj_set_.find(b);
    if (finda_it != adj_set_.end() && findb_it != adj_set_.end()) {
        return finda_it->second.count(b) && findb_it->second.count(a);
    }
    return false;
}

bool ColoringRegsAllocator::isPrecolored(MachineOperand *operand) {
    return operand->getOperandType() == MachineOperand::MachineReg;
}

void ColoringRegsAllocator::addEdge(MachineOperand *a, MachineOperand *b) {
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

static void printRegStr(MachineOperand *operand) {
    if (operand->getOperandType() == MachineOperand::MachineReg) {
        printf("%s", dynamic_cast<MachineReg *>(operand)->machieReg2RegName().c_str());
    } else if (operand->getOperandType() == MachineOperand::VirtualReg) {
        printf("vreg%d", dynamic_cast<VirtualReg *>(operand)->getRegId());
    } else {
        printf("not valid operand: %d", operand->getOperandType());
    }
}

void ColoringRegsAllocator::build() {
    for (auto &mc_basicblock: curr_function_->getMachineBasicBlock()) {
        auto live = live_out_[mc_basicblock.get()];
        auto &mc_inst_list = mc_basicblock->getInstructionList();

        for (auto it = mc_inst_list.rbegin(); it != mc_inst_list.rend(); ++it) {

            MachineInst *mc_inst = it->get();
            auto defs = MachineInst::getDefs(mc_inst, allocate_float_);
            auto uses = MachineInst::getUses(mc_inst, allocate_float_);
            if (mc_inst->getMachineInstType() == MachineInst::Move) {
                auto move_inst = dynamic_cast<MoveInst *>(mc_inst);
                if (((move_inst->getMoveType() == MoveInst::F2F && allocate_float_)
                || (move_inst->getMoveType() == MoveInst::I2I && !allocate_float_))
                && isNeedAlloca(move_inst->getSrc())
                && isNeedAlloca(move_inst->getDst())) {
                    for (auto use: uses) {     // live :=  live \ use(I)
                        live.erase(use);
                        move_list_[use].insert(mc_inst);
                    }

                    for (auto def: defs) {
                        move_list_[def].insert(mc_inst);
                    }
                    worklist_moves_.insert(mc_inst);
                }
            }
            live.insert(defs.begin(), defs.end());

            for (auto def: defs) {          // 用于计算spill cost
                def_use_count_map_[def]++;
                // assert(mc_inst->getParent());
                while_loop_depth_map[def] = std::max(while_loop_depth_map[def], mc_inst->getParent()->getLoopDepth());
            }

            for (auto use: uses) {
                def_use_count_map_[use]++;
                // assert(mc_inst->getParent());
                while_loop_depth_map[use] = std::max(while_loop_depth_map[use], mc_inst->getParent()->getLoopDepth());
            }
            // add edges
            for (auto def: defs) {
                for (auto l: live) {
                    addEdge(l, def);
                }
            }

            for (auto def: defs) {
                live.erase(def);
            }

            live.insert(uses.begin(), uses.end());
        }
    }
}

bool ColoringRegsAllocator::moveRelated(MachineOperand *operand) {
    auto result = nodeMoves(operand);
    return !result.empty();
}

void ColoringRegsAllocator::mkWorkList() {      // 将不同类型的变量进行分类到不同的集合中
    for (auto init: initial_) {
        if (degree_[init] >= k_) {
            spill_work_list_.insert(init);
        } else if (moveRelated(init)) {         // 如果这个变量涉及到move语句
            freeze_work_list_.insert(init);
        } else {
            simplify_work_list_.insert(init);
        }
    }
}
// 获取一个节点的邻居的集合，具体算法是通过将原冲突图，去掉移除到栈上以及已经合并的部分
ColoringRegsAllocator::OperandSet ColoringRegsAllocator::adjacent(MachineOperand *operand) {
    OperandSet result = adj_list_[operand];
    for (auto node: select_stack_.set_) {
        result.erase(node);
    }
    for (auto node: coalesced_nodes_) {
        result.erase(node);
    }
    // 也就是说需要去除select_stack以及coalesced中的
    return result;
}

ColoringRegsAllocator::InstSet ColoringRegsAllocator::nodeMoves(MachineOperand *operand) {
    InstSet tmp_union_set;
    // std::set_union(active_moves_.begin(), active_moves_.end(), worklist_moves_.begin(), worklist_moves_.end(), tmp_union_set.begin());
    for (auto move: active_moves_) {
        tmp_union_set.insert(move);
    }
    for (auto move: worklist_moves_) {
        tmp_union_set.insert(move);
    }
    InstSet result;
    // std::set_intersection(move_list_.begin(), move_list_.end(), tmp_union_set.begin(), tmp_union_set.end(), result.begin());
    for (auto node: move_list_[operand]) {
        if (tmp_union_set.count(node)) {
            result.insert(node);
        }
    }
    return result;
}

void ColoringRegsAllocator::decrementDegree(MachineOperand *operand) {
    auto degree = degree_[operand];     // let d = degree[m]
    degree_[operand] = degree - 1;      // degree[m] := d - 1

    if (degree == k_) {     //
        auto tmp_set = adjacent(operand);
        tmp_set.insert(operand);        // {m} U adjacent
        enableMoves(tmp_set);

        spill_work_list_.erase(operand);

        if (moveRelated(operand)) {
            freeze_work_list_.insert(operand);
        } else {
            simplify_work_list_.insert(operand);
        }
    }
}

void ColoringRegsAllocator::enableMoves(const OperandSet &operand_set) {
    for (auto operand: operand_set) {
        auto node_moves = nodeMoves(operand);
        for (auto m: node_moves) {
            if (active_moves_.count(m)) {
                active_moves_.erase(m);
                worklist_moves_.insert(m);
            }
        }
    }
}

void ColoringRegsAllocator::simplify() {
    auto operand = *simplify_work_list_.begin();
    simplify_work_list_.erase(operand);

    select_stack_.push(operand);

    OperandSet adj = adjacent(operand);

    for (auto m: adj) {
        decrementDegree(m);
    }
}

MachineOperand *ColoringRegsAllocator::getAlias(MachineOperand *operand) {
    if (coalesced_nodes_.count(operand)) {
        assert(alias_.find(operand) != alias_.end());
        return getAlias(alias_[operand]);
    }
    return operand;
}

void ColoringRegsAllocator::addWorkList(MachineOperand *operand) {
    if (!isPrecolored(operand) && !moveRelated(operand) && degree_[operand] < k_) {
        freeze_work_list_.erase(operand);
        simplify_work_list_.insert(operand);
    }
}

bool ColoringRegsAllocator::ok(MachineOperand *t, MachineOperand *r) {
    return degree_[t] < k_ || isPrecolored(t) || isInAdjSet(t, r);
}

void ColoringRegsAllocator::combine(MachineOperand *u, MachineOperand *v) {
    if (freeze_work_list_.count(u)) {
        freeze_work_list_.erase(v);
    } else {
        spill_work_list_.erase(v);
    }
    coalesced_nodes_.insert(v);

    alias_.insert({v, u});
    for (auto node: move_list_[v]) {
        move_list_[u].insert(node);
    }
    // enableMoves({v});

    auto adj = adjacent(v);
    for (auto t: adj) {
        addEdge(t, u);
        decrementDegree(t);
    }

    if (degree_[u] >= k_ && freeze_work_list_.count(u)) {
        freeze_work_list_.erase(u);
        spill_work_list_.insert(u);
    }
}

bool ColoringRegsAllocator::conservative(const OperandSet &nodes) {
    int k = 0;
    for (auto node: nodes) {
        if (degree_[node] >= k_) {
            k++;
        }
    }
    return k < k_;
}

void ColoringRegsAllocator::coalesce() {
    auto m = *worklist_moves_.begin();
    worklist_moves_.erase(m);

    auto move_inst = dynamic_cast<MoveInst *>(m);
    assert(move_inst);

    auto u = getAlias(move_inst->getSrc());
    auto v = getAlias(move_inst->getDst());
    if (isPrecolored(v)) {
        std::swap(u, v);
    }

    bool okok = false;
    for (auto t : adjacent(v)) {
        if (ok(t, u)) {
            okok = true;
            break;
        }
    }

    auto join = adjacent(u);
    for (auto n : adjacent(v)) {
        join.insert(n);
    }

    if (u == v) {
        coalesced_moves_.insert(m);
        addWorkList(u);
    } else if (isPrecolored(v) || isInAdjSet(u, v)) {
        constrained_moves_.insert(m);
        addWorkList(u);
        addWorkList(v);
    } else if ((isPrecolored(u) && okok) || (!isPrecolored(u) && conservative(join)))  {
        coalesced_moves_.insert(m);
        assert(u && v);
        combine(u, v);
        addWorkList(u);
    } else {
        active_moves_.insert(m);
    }

}

void ColoringRegsAllocator::freeze() {
    auto u = *freeze_work_list_.begin();
    freeze_work_list_.erase(u);
    simplify_work_list_.insert(u);
    freezeMoves(u);
}

void ColoringRegsAllocator::finishAllocate() {
    if (allocate_float_ || (!allocate_float_ && !needAllocateForFloat())) {
        if ((curr_function_->getStackSize() + spilled_stack_size_) % 8 == 0) {
            spilled_stack_size_ += 4;
        }
        code_gen_->addInstAboutStack(curr_function_, curr_function_->getStackSize() + spilled_stack_size_);
    }

    for (auto &[reg, color]: color_) {
        if (auto vreg = dynamic_cast<VirtualReg *>(reg); vreg) {
            vreg->color(color);
        }
    }

    for (auto &bb: curr_function_->getMachineBasicBlock()) {
        auto &inst_list = bb->getInstructionListNonConst();
        for (auto inst_it = inst_list.begin(); inst_it != inst_list.end();) {
            auto inst = inst_it->get();
            auto mov_inst = dynamic_cast<MoveInst *>(inst);
            if (mov_inst && (mov_inst->getMoveType() == MoveInst::F2F || mov_inst->getMoveType() == MoveInst::I2I)) {
                assert(mov_inst);
                MachineReg::Reg src_mreg, dst_mreg;
                auto src_reg = mov_inst->getSrc();
                auto dst_reg = mov_inst->getDst();
                if (!isNeedAlloca(src_reg) || !isNeedAlloca(dst_reg)) {
                    ++inst_it;
                    continue;
                }
                if (src_reg->getOperandType() == MachineOperand::MachineReg) {
                    src_mreg = dynamic_cast<MachineReg *>(src_reg)->getReg();
                } else if (src_reg->getOperandType() == MachineOperand::VirtualReg) {
                    src_mreg = dynamic_cast<VirtualReg *>(src_reg)->getColoredReg();
                }

                if (dst_reg->getOperandType() == MachineOperand::MachineReg) {
                    dst_mreg = dynamic_cast<MachineReg *>(dst_reg)->getReg();
                } else if (dst_reg->getOperandType() == MachineOperand::VirtualReg) {
                    dst_mreg = dynamic_cast<VirtualReg *>(dst_reg)->getColoredReg();
                }

                if (dst_mreg == src_mreg) {
                    inst_list.erase(inst_it++);
                    continue;
                }
            }
            ++inst_it;
        }
    }

}

void ColoringRegsAllocator::freezeMoves(MachineOperand *operand) {
    for (auto m: nodeMoves(operand)) {
        auto move_inst = dynamic_cast<MoveInst *>(m);
        assert(move_inst);
        auto u = move_inst->getSrc();
        auto v = move_inst->getDst();

        if (active_moves_.count(m)) {
            active_moves_.erase(m);
        } else {
            worklist_moves_.erase(m);
        }

        frozen_moves_.insert(m);
        if (nodeMoves(v).empty() && degree_[v] < k_) {
            freeze_work_list_.erase(v);
            simplify_work_list_.insert(v);
        }
    }
}

void ColoringRegsAllocator::selectSpill() {
    MachineOperand *m = nullptr;          // 采用某种策略选出一个spill的节点
    int32_t min_cost = INT32_MAX;

    std::vector<MachineOperand *> spilled_nodes;

    for (auto spill: spill_work_list_) {            // 从中选出一个spill代价最小的
        if (already_spilled_.count(spill)) {
            spilled_nodes.push_back(spill);
            continue;
        }
        int32_t cost =
                def_use_count_map_[spill] * pow(10, while_loop_depth_map[spill]) / degree_[spill];
        if (cost < min_cost) {
            m = spill;
            min_cost = cost;
        }
    }

    for (auto spilled: spilled_nodes) {
        spill_work_list_.erase(spilled);
    }

    if (m) {
        spill_work_list_.erase(m);
        simplify_work_list_.insert(m);
        freezeMoves(m);
    }
}

void ColoringRegsAllocator::assignColors() {
    while (!select_stack_.empty()) {
        auto node = select_stack_.pop();

        std::set<MachineReg::Reg> ok_colors;     // 设置分配颜色的集合
        if (allocate_float_) {
            ok_colors = float_regs_set_;
        } else {
            ok_colors = int_regs_set_;
        }

        for (auto w: adj_list_[node]) {
            auto a = getAlias(w);
            if (colored_nodes_.count(a) || isPrecolored(a)) {
                ok_colors.erase(color_[a]);
            }
        }

        if (ok_colors.empty()) {
            spilled_nodes_.insert(node);
        } else {
            colored_nodes_.insert(node);
            auto color = *ok_colors.begin();
            color_[node] = color;
        }
    }

    for (auto node : coalesced_nodes_) {
        color_[node] = color_[getAlias(node)];
    }

}

void ColoringRegsAllocator::rewriteProgram() {

    for (auto spill_node: spilled_nodes_) {         // 确定为需要溢出的节点
        already_spilled_.insert(spill_node);            // 插入到已经溢出的队列
        spill_work_list_.erase(spill_node);
        spilled_stack_size_ += 4;           // spill维护的stack偏移量

        // printf("insert load or store inst......\n");
        for (auto &bb: curr_function_->getMachineBasicBlock()) {
            auto &inst_list = bb->getInstructionListNonConst();

            std::unordered_map<MachineInst *, std::vector<MachineInst *>> insert_before;
            std::unordered_map<MachineInst *, std::vector<MachineInst *>> insert_after;
            std::unordered_map<MachineInst *, MachineBasicBlock::MachineInstListIt> insert_it;

            for (auto it = inst_list.begin(); it != inst_list.end(); ++it) {
                auto inst = it->get();
                auto defs = MachineInst::getDefs(inst, allocate_float_);
                auto uses = MachineInst::getUses(inst, allocate_float_);

                MachineOperand::ValueType value_type = allocate_float_ ? MachineOperand::Float : MachineOperand::Int;

                if (defs.count(spill_node)) {
                    bool use_ip_base = false;
                    std::vector<MachineInst *> moves_offset_insts;
                    auto offset_reg = code_gen_->getImmOperandInBinary(- curr_function_->getStackSize() - spilled_stack_size_, bb.get(), &moves_offset_insts, allocate_float_, &use_ip_base);
                    auto store_vreg = code_gen_->createVirtualReg(curr_function_, value_type);
                    // printf("the new vreg is %d, insert before vreg%d inst\n", store_vreg->getRegId(), dynamic_cast<VirtualReg *>(spill_node)->getRegId());
                    assert(offset_reg);
                    MachineInst *store_inst;
                    if (!use_ip_base) {
                        store_inst = new StoreInst(MemIndexType::PostiveIndex, bb.get(), store_vreg, code_gen_->fp_reg_,
                                                   offset_reg);
                    } else {
                        store_inst = new StoreInst(bb.get(), store_vreg, offset_reg);
                    }
                    // store_inst->setValueType(allocate_float_ ? MachineInst::Float: MachineInst::Int);
                    already_spilled_.insert(store_vreg);
                    // MachineInst::replaceDefs(inst, dynamic_cast<VirtualReg *>(spill_node), store_vreg);
                    inst->replaceDefs(spill_node, store_vreg);
                    // insert指令
                    moves_offset_insts.push_back(store_inst);
                    insert_after.insert({inst, moves_offset_insts});
                    insert_it.insert({inst, it});
                }

                if (uses.count(spill_node)) {
                    bool use_ip_base = false;
                    std::vector<MachineInst *> moves_offset_insts;
                    auto offset_reg = code_gen_->getImmOperandInBinary(- curr_function_->getStackSize() - spilled_stack_size_, bb.get(), &moves_offset_insts, allocate_float_, &use_ip_base);
                    auto load_vreg = code_gen_->createVirtualReg(curr_function_, value_type);
                    // printf("the new vreg is %d, insert before vreg%d inst\n", load_vreg->getRegId(),  dynamic_cast<VirtualReg *>(spill_node)->getRegId());

                    MachineInst *load_inst;
                    if (!use_ip_base) {
                        load_inst = new LoadInst(bb.get(), load_vreg, code_gen_->fp_reg_, offset_reg);
                    } else {
                        load_inst = new LoadInst(bb.get(), load_vreg, offset_reg);
                    }
                    // load_inst->setValueType(allocate_float_ ? MachineInst::Float: MachineInst::Int);
                    already_spilled_.insert(load_vreg);
                    // MachineInst::replaceUses(inst, dynamic_cast<VirtualReg *>(spill_node), load_vreg);
                    inst->replaceUses(spill_node, load_vreg);
                    moves_offset_insts.push_back(load_inst);
                    insert_before[inst].insert(insert_before[inst].end(), moves_offset_insts.begin(), moves_offset_insts.end());
                    insert_it.insert({inst, it});
                }
                // assert(both_inserted < 2);
            }

            for (auto [inserted, insts]: insert_before) {
                auto find_inserted_it = insert_it.find(inserted);
                assert(find_inserted_it != insert_it.end());
                for (auto inst: insts) {
                    bb->insertInstructionBefore(find_inserted_it->second, inst);
                }
            }

            for (auto [inserted, insts]: insert_after) {
                auto find_inserted_it = insert_it.find(inserted);
                assert(find_inserted_it != insert_it.end());
                for (auto rit = insts.rbegin(); rit != insts.rend(); ++rit) {
                    bb->insertInstruction(find_inserted_it->second, *rit);
                }
            }
        }
    }
}


void ColoringRegsAllocator::allocate(MachineFunction *func) {
    allocate_float_ = false;
    k_ = int_regs_set_.size();
    /*for (auto &func: machine_module_->getMachineFunctions()) {
        runOnMachineFunction(func.get());
    }*/
    runOnMachineFunction(func);
    k_ = float_regs_set_.size();
    allocate_float_ = true;
    /*for (auto &func: machine_module_->getMachineFunctions()) {
        runOnMachineFunction(func.get());
    }*/
    runOnMachineFunction(func);

}

void ColoringRegsAllocator::allocate() {
    allocate_float_ = false;
    k_ = int_regs_set_.size();
    for (auto &func: machine_module_->getMachineFunctions()) {
        runOnMachineFunction(func.get());
    }
    k_ = float_regs_set_.size();
    allocate_float_ = true;
    for (auto &func: machine_module_->getMachineFunctions()) {
        runOnMachineFunction(func.get());
    }
}

void ColoringRegsAllocator::init() {
    // init the set of regs allocator
    pre_colored_.clear();
    initial_.clear();
    simplify_work_list_.clear();
    freeze_work_list_.clear();
    frozen_moves_.clear();
    spill_work_list_.clear();
    coalesced_nodes_.clear();
    colored_nodes_.clear();
    spilled_nodes_.clear();
    select_stack_.clear();
    // frozen_moves
    active_moves_.clear();
    worklist_moves_.clear();
    coalesced_moves_.clear();
    constrained_moves_.clear();

    adj_set_.clear();
    adj_list_.clear();
    degree_.clear();
    move_list_.clear();
    alias_.clear();
    color_.clear();
    def_use_count_map_.clear();
    while_loop_depth_map.clear();

}

void ColoringRegsAllocator::runOnMachineFunction(MachineFunction *function) {
    curr_function_ = function;
    // printf("begin a allcate on a function %d\n", allocate_float_);
    init();
    // printf("analyseLiveness......\n");
    analyseLiveness(function);

    // set
    auto machine_reg_map = machine_module_->getMachineRegMap();
    for (auto &[reg, reg_operand]: machine_reg_map) {       // 将所有的寄存器都放入进入比较合适
        if ((allocate_float_ && reg >= MachineReg::s0 && reg <= MachineReg::s31) || (!allocate_float_ && reg >= MachineReg::r0 && reg <= MachineReg::r15)) {
            color_[reg_operand] = reg;
            pre_colored_.push_back(reg_operand);
        }
    }

    // printf("the initial regs(%d) is here:\n", allocate_float_);
    for (auto vir_reg: function->getVirtualRegs()) {            // 需要根据当前是否处理float来分开处理
        if ((!allocate_float_ && vir_reg->getValueType() == MachineOperand::Int) || (allocate_float_ && vir_reg->getValueType() == MachineOperand::Float)) {
            // printRegStr(vir_reg);
            // printf("\n");
            initial_.insert(vir_reg);
        }
    }

    curr_function_ = function;

    if (initial_.empty()) {
        if (allocate_float_) {
            finishAllocate();
        }
        return;
    }

    build();
    mkWorkList();

    do {
        if (!simplify_work_list_.empty()) {
            // printf("simplify\n");
            simplify();
        } else if (!worklist_moves_.empty()) {
            // printf("coalesce\n");
            coalesce();
        } else if (!freeze_work_list_.empty()) {
            // printf("freeze\n");
            freeze();
        } else if (!spill_work_list_.empty()) {
            // printf("selectSpill, the spill size is %d\n", spill_work_list_.size());
            selectSpill();
        }
    } while (!simplify_work_list_.empty() || !worklist_moves_.empty() || !freeze_work_list_.empty() || !spill_work_list_.empty());

    assignColors();

    if (!spilled_nodes_.empty()) {
        rewriteProgram();
        runOnMachineFunction(function);

    } else {
        finishAllocate();
    }
}




