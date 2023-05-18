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
#include "../ir/BasicBlock.h"

RegsAllocator::RegsAllocator(MachineModule *module, CodeGen *codegen):
    machine_module_(module),
    code_gen_(codegen){
    int_regs_set_ = {
            MachineReg::r0, MachineReg::r1, MachineReg::r2, MachineReg::r3, MachineReg::r4,
            MachineReg::r5, MachineReg::r6, MachineReg::r7, MachineReg::r8, MachineReg::r9,
            MachineReg::r10, MachineReg::r12, MachineReg::r14,
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
}

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

void RegsAllocator::analyseLiveness(MachineFunction *function) {            // 数据流分析中，会对float和int类型的变量进行区分
    live_in_.clear();           // 其中在获取def、use时，也就需要区分不同的类型
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
    return operand->getOperandType() == MachineOperand::MachineReg;
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

static void printRegStr(MachineOperand *operand) {
    if (operand->getOperandType() == MachineOperand::MachineReg) {
        printf("%s", dynamic_cast<MachineReg *>(operand)->machieReg2RegName().c_str());
    } else if (operand->getOperandType() == MachineOperand::VirtualReg) {
        printf("vreg%d", dynamic_cast<VirtualReg *>(operand)->getRegId());
    } else {
        printf("not valid operand: %d", operand->getOperandType());
    }
}

void RegsAllocator::build() {
    for (auto &mc_basicblock: curr_function_->getMachineBasicBlock()) {
        auto live = live_out_[mc_basicblock.get()];
        auto &mc_inst_list = mc_basicblock->getInstructionList();

        for (auto it = mc_inst_list.rbegin(); it != mc_inst_list.rend(); ++it) {

            MachineInst *mc_inst = it->get();
            auto defs = MachineInst::getDefs(mc_inst, allocate_float_);
            auto uses = MachineInst::getUses(mc_inst, allocate_float_);
            if (mc_inst->getMachineInstType() == MachineInst::Move) {
                auto move_inst = dynamic_cast<MoveInst *>(mc_inst);
                if (isNeedAlloca(move_inst->getSrc()) && isNeedAlloca(move_inst->getDst())) {
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
            // live := use(I) U (live \ def(I))
            // std::set_difference(live.begin(), live.end(), defs.begin(), defs.end(), tmp_set.end());
            for (auto def: defs) {
                live.erase(def);
            }

            live.insert(uses.begin(), uses.end());
        }
    }
}

bool RegsAllocator::moveRelated(MachineOperand *operand) {
    auto result = nodeMoves(operand);
    return !result.empty();
}

void RegsAllocator::mkWorkList() {      // 将不同类型的变量进行分类到不同的集合中
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
RegsAllocator::OperandSet RegsAllocator::adjacent(MachineOperand *operand) {
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

RegsAllocator::InstSet RegsAllocator::nodeMoves(MachineOperand *operand) {
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

void RegsAllocator::decrementDegree(MachineOperand *operand) {
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

void RegsAllocator::enableMoves(const OperandSet &operand_set) {
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

void RegsAllocator::simplify() {
    auto operand = *simplify_work_list_.begin();
    simplify_work_list_.erase(operand);

    select_stack_.push(operand);

    OperandSet adj = adjacent(operand);

    for (auto m: adj) {
        decrementDegree(m);
    }
}

MachineOperand *RegsAllocator::getAlias(MachineOperand *operand) {
    if (coalesced_nodes_.count(operand)) {
        assert(alias_.find(operand) != alias_.end());
        return getAlias(alias_[operand]);
    }
    return operand;
}

void RegsAllocator::addWorkList(MachineOperand *operand) {
    if (!isPrecolored(operand) && !moveRelated(operand) && degree_[operand] < k_) {
        freeze_work_list_.erase(operand);
        simplify_work_list_.insert(operand);
    }
}

bool RegsAllocator::ok(MachineOperand *t, MachineOperand *r) {
    return degree_[t] < k_ || isPrecolored(t) || isInAdjSet(t, r);
}

void RegsAllocator::combine(MachineOperand *u, MachineOperand *v) {
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

bool RegsAllocator::conservative(const OperandSet &nodes) {
    int k = 0;
    for (auto node: nodes) {
        if (degree_[node] >= k_) {
            k++;
        }
    }
    return k < k_;
}

void RegsAllocator::coalesce() {
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

void RegsAllocator::freeze() {
    auto u = *freeze_work_list_.begin();
    freeze_work_list_.erase(u);
    simplify_work_list_.insert(u);
    freezeMoves(u);
}

void RegsAllocator::freezeMoves(MachineOperand *operand) {
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

void RegsAllocator::selectSpill() {
    MachineOperand *m;          // 采用某种策略选出一个spill的节点
    int32_t min_cost = INT32_MAX;

    for (auto spill: spill_work_list_) {            // 从中选出一个spill代价最小的
        if (already_spilled_.count(spill)) {
            continue;
        }
        int32_t cost =
                def_use_count_map_[spill] * pow(10, while_loop_depth_map[spill]) / degree_[spill];
        if (cost < min_cost) {
            m = spill;
            min_cost = cost;
        }
    }

    spill_work_list_.erase(m);
    simplify_work_list_.insert(m);
    freezeMoves(m);
}

void RegsAllocator::assignColors() {
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

void RegsAllocator::rewriteProgram() {
    for (auto spill_node: spilled_nodes_) {         // 确定为需要溢出的节点
        already_spilled_.insert(spill_node);            // 插入到已经溢出的队列
        spilled_stack_size_ += 4;           // spill维护的stack偏移量

        for (auto &bb: curr_function_->getMachineBasicBlock()) {
            auto &inst_list = bb->getInstructionListNonConst();

            std::unordered_map<MachineInst *, MachineInst *> insert_before;
            std::unordered_map<MachineInst *, MachineInst *> insert_after;
            std::unordered_map<MachineInst *, MachineBasicBlock::MachineInstListIt> insert_it;

            for (auto it = inst_list.begin(); it != inst_list.end(); ++it) {
                auto inst = it->get();
                auto defs = MachineInst::getDefs(inst, allocate_float_);
                auto uses = MachineInst::getUses(inst, allocate_float_);

                MachineOperand::ValueType value_type = allocate_float_ ? MachineOperand::Float : MachineOperand::Int;

                if (defs.count(spill_node)) {
                    auto store_vreg = code_gen_->createVirtualReg(curr_function_, value_type);
                    auto store_inst = new StoreInst(MemIndexType::PostiveIndex, bb.get(), store_vreg, code_gen_->sp_reg_, new ImmNumber(- curr_function_->getStackSize() - spilled_stack_size_));
                    already_spilled_.insert(store_vreg);
                    MachineInst::replaceDefs(inst, dynamic_cast<VirtualReg *>(spill_node), store_vreg);
                    // insert指令
                    insert_after.insert({inst, store_inst});
                    insert_it.insert({inst, it});
                }

                if (uses.count(spill_node)) {
                    auto load_vreg = code_gen_->createVirtualReg(curr_function_, value_type);
                    auto load_inst = new LoadInst(bb.get(), load_vreg, code_gen_->sp_reg_, new ImmNumber(- curr_function_->getStackSize() - spilled_stack_size_));
                    already_spilled_.insert(load_vreg);
                    MachineInst::replaceUses(inst, dynamic_cast<VirtualReg *>(spill_node), load_vreg);
                    // insert指令
                    insert_before.insert({inst, load_inst});
                    insert_it.insert({inst, it});
                }
            }

            for (auto [inserted, inst]: insert_before) {
                auto find_inserted_it = insert_it.find(inserted);
                assert(find_inserted_it != insert_it.end());
                assert(inst->getParent());
                bb->insertInstructionBefore(find_inserted_it->second, inst);
            }

            for (auto [inserted, inst]: insert_after) {
                auto find_inserted_it = insert_it.find(inserted);
                assert(find_inserted_it != insert_it.end());
                assert(inst->getParent());
                bb->insertInstruction(find_inserted_it->second, inst);
            }

        }
    }

}


void RegsAllocator::allocate() {
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

void RegsAllocator::init() {
    // init the set of regs allocator
    pre_colored_.clear();
    initial_.clear();
    simplify_work_list_.clear();
    freeze_work_list_.clear();
    spill_work_list_.clear();
    coalesced_nodes_.clear();
    colored_nodes_.clear();
    spilled_nodes_.clear();
    already_spilled_.clear();
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

    spilled_stack_size_ = 0;
}

void RegsAllocator::runOnMachineFunction(MachineFunction *function) {
    // printf("begin a allcate on a function\n");
    init();
    analyseLiveness(function);

    // set
    auto machine_reg_map = machine_module_->getMachineRegMap();
    for (auto &[reg, reg_operand]: machine_reg_map) {       // 将所有的寄存器都放入进入比较合适
        if ((allocate_float_ && reg >= MachineReg::s0 && reg <= MachineReg::s31) || (!allocate_float_ && reg >= MachineReg::r0 && reg <= MachineReg::r15)) {
            color_[reg_operand] = reg;
            pre_colored_.push_back(reg_operand);
        }
    }

    for (auto vir_reg: function->getVirtualRegs()) {            // 需要根据当前是否处理float来分开处理
        if ((!allocate_float_ && vir_reg->getValueType() == MachineOperand::Int) || (allocate_float_ && vir_reg->getValueType() == MachineOperand::Float)) {
            initial_.insert(vir_reg);
        }
    }

    /*printf("---------------------inital virtual regs------------------------\n");
    for (auto init: initial_) {
        auto vreg = dynamic_cast<VirtualReg*> (init);
        assert(vreg);
        printf("vreg%d\t", vreg->getRegId());
    }
    printf("\n");*/

    if (initial_.empty()) {
        return;
    }

    curr_function_ = function;
    build();
    /*printf("-----------------the interface graph-------------------\n");
    for (auto node_list: adj_set_) {
        if (node_list.first->getOperandType() == MachineOperand::MachineReg) {
            printf("%s", dynamic_cast<MachineReg*>(node_list.first)->machieReg2RegName().c_str());
        } else {
            printf("vreg%d", dynamic_cast<VirtualReg *>(node_list.first)->getRegId());
        }
        printf(":\t");
        for (auto node: node_list.second) {
            if (node->getOperandType() == MachineOperand::MachineReg) {
                printf("%s", dynamic_cast<MachineReg*>(node)->machieReg2RegName().c_str());
            } else {
                printf("vreg%d", dynamic_cast<VirtualReg *>(node)->getRegId());
            }
            printf("\t");
        }
        printf("\n");
    }*/

    mkWorkList();

    do {
        if (!simplify_work_list_.empty()) {
            simplify();
        } else if (!worklist_moves_.empty()) {
            coalesce();
        } else if (!freeze_work_list_.empty()) {
            freeze();
        } else if (!spill_work_list_.empty()) {
            selectSpill();
        }
    } while (!simplify_work_list_.empty() || !worklist_moves_.empty() || !freeze_work_list_.empty() || !spill_work_list_.empty());

    assignColors();

    if (!spilled_nodes_.empty()) {
        rewriteProgram();
        runOnMachineFunction(function);
    } else {
        /*printf("-------------the colors is here-------------\n");
        for (auto &[reg, color]: color_) {
            if (reg->getOperandType() == MachineOperand::MachineReg) {
                auto mc_reg = dynamic_cast<MachineReg *>(reg);
                printf("%s: colored %d\n", mc_reg->machieReg2RegName().c_str(), color);
            } else if (reg->getOperandType() == MachineOperand::VirtualReg){
                auto vreg = dynamic_cast<VirtualReg *>(reg);
                printf("vreg%d: colored %d\n", vreg->getRegId(), color);
            }
        }
        printf("-------------the moves is here-------------\n");
        for (auto &mov: coalesced_moves_) {
            auto mov_inst = dynamic_cast<MoveInst *>(mov);
            printf("mov inst:\t");
            printRegStr(mov_inst->getDst());
            printf(",\t");
            printRegStr(mov_inst->getSrc());
            printf("\n");
        }*/

        for (auto &[reg, color]: color_) {
            if (auto vreg = dynamic_cast<VirtualReg *>(reg); vreg) {
                vreg->color(color);
            }
        }
    }
}




