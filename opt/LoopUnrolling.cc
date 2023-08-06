//
// Created by 杨丰硕 on 2023/6/21.
//
#include "ComputeLoops.h"
#include "LoopUnrolling.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "../ir/Value.h"
#include "../ir/IrDumper.h"

Value *LoopUnrolling::getCopyValue(Value *value, Instruction *inst) {
    Value *copy_value = nullptr;
    if (last_iterate_var_map_.find(value) != last_iterate_var_map_.end()) {
        return last_iterate_var_map_[value];
    }
    if (value->getValueType() == ConstantValue) {
        auto const_var = dynamic_cast<ConstantVar *>(value);
        assert(const_var);
        if (const_var->getBasicType() == INT_BTYPE) {
            copy_value = new ConstantVar(const_var->getIValue());
        } else {
            copy_value = new ConstantVar(const_var->getFValue());
        }
    } else if (copy_insts_map_.count(value)) {
        copy_value = copy_insts_map_[value];
    } else {
        may_unfinished_copy_insts_.insert(inst);
        copy_value = value;
    }
    return copy_value;
}

Instruction *LoopUnrolling::getCopyInstruction(Instruction *inst, BasicBlock *basicblock, const std::string &new_name) {
    Instruction *new_inst = nullptr;
    auto inst_type = inst->getInstType();
    switch (inst_type) {
        case StoreType: {
            auto store_inst = dynamic_cast<StoreInstruction *>(inst);
            auto copy_value = getCopyValue(store_inst->getValue(), store_inst);
            auto copy_ptr = getCopyValue(store_inst->getPtr(), store_inst);

            new_inst = new StoreInstruction(basicblock, inst->getBasicType(), copy_value, copy_ptr);
            break;
        }
        case LoadType: {
            auto load_inst = dynamic_cast<LoadInstruction *>(inst);
            auto load_addr = getCopyValue(load_inst->getPtr(), load_inst);
            new_inst = new LoadInstruction(basicblock, load_inst->getBasicType(), load_addr, load_inst->getBasicType(), new_name);
            break;
        }
        case GEPType: {
            auto gep_inst = dynamic_cast<GEPInstruction *>(inst);
            std::vector<Value *> operands;
            for (int i = 0; i < gep_inst->getOperandNum(); ++i) {
                auto operand = gep_inst->getOperand(i);
                auto copy_operand = getCopyValue(operand, gep_inst);
                operands.push_back(copy_operand);
            }

            new_inst = new GEPInstruction(basicblock, gep_inst->getBasicType(), operands, gep_inst->getArrayDimension(), gep_inst->isPtrOffset(), new_name);
            break;
        }
        case MemSetType: {
            auto memset_inst = dynamic_cast<MemSetInstruction *>(inst);
            auto new_size = getCopyValue(memset_inst->getSize(), memset_inst);
            auto new_base = getCopyValue(memset_inst->getBase(), memset_inst);
            auto new_value = getCopyValue(memset_inst->getValue(), memset_inst);

            new_inst = new MemSetInstruction(basicblock, memset_inst->getBasicType(), new_base, new_size, new_value);
            break;
        }
        case BrType: {          // 这一部分还是需要调整的，只不过目前
            auto br_inst = dynamic_cast<BranchInstruction *>(inst);
            if (br_inst->isCondBranch()) {
                auto new_cond = getCopyValue(br_inst->getCond(), br_inst);
                auto new_true_label = getCopyValue(br_inst->getTrueLabel(), br_inst);
                auto new_false_label = getCopyValue(br_inst->getFalseLabel(), br_inst);
                new_inst = new BranchInstruction(basicblock, new_cond, new_true_label, new_false_label);
            } else {
                auto new_label = getCopyValue(br_inst->getLabel(), br_inst);
                new_inst = new BranchInstruction(basicblock, new_label);
            }
            basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
            break;
        }
        case SetCondType: {
            auto set_cond_inst = dynamic_cast<SetCondInstruction *>(inst);
            auto new_left = getCopyValue(set_cond_inst->getLeft(), set_cond_inst);
            auto new_right = getCopyValue(set_cond_inst->getRight(), set_cond_inst);

            new_inst = new SetCondInstruction(basicblock, set_cond_inst->getCmpType(), set_cond_inst->isFloatCmp(), new_left, new_right, new_name);
            break;
        }
        case CallType: {
            auto call_inst = dynamic_cast<CallInstruction *>(inst);
            std::vector<Value *> new_actuals;
            for (int i = 0; i < call_inst->getActualSize(); ++i) {
                auto new_actual = getCopyValue(call_inst->getActual(i), call_inst);
                new_actuals.push_back(new_actual);
            }

            new_inst = new CallInstruction(basicblock, call_inst->getFunction(), new_actuals, new_name);
            break;
        }
        case ZextType: {
            auto zext_inst = dynamic_cast<ZextInstruction *>(inst);
            auto new_left = getCopyValue(zext_inst->getValue(), zext_inst);

            new_inst = new ZextInstruction(basicblock, new_left, new_name);
            break;
        }
        case CastType: {
            auto cast_inst = dynamic_cast<CastInstruction *>(inst);
            auto new_value = getCopyValue(cast_inst->getValue(), cast_inst);

            new_inst = new CastInstruction(basicblock, cast_inst->isI2F(), new_value, new_name);
            break;
        }
        case PhiType: {
            auto phi_inst = dynamic_cast<PhiInstruction *>(inst);

            std::vector<Value *> new_phi_values;
            std::vector<BasicBlock *> new_basicblocks;

            for (int i = 0; i < phi_inst->getSize(); ++i) {
                auto value_basicblock = phi_inst->getValueBlock(i);
                auto new_phi_value = getCopyValue(value_basicblock.first, phi_inst);
                auto new_phi_basicblock = getCopyValue(value_basicblock.second, phi_inst);

                assert(new_phi_basicblock->getValueType() == BasicBlockValue);

                new_phi_values.push_back(new_phi_value);
                new_basicblocks.push_back(dynamic_cast<BasicBlock *>(new_phi_basicblock));
            }

            new_inst = new PhiInstruction(basicblock, phi_inst->getBasicType(), new_phi_values, new_basicblocks, new_name);
            break;
        }
        default:
            break;
    }

    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
        auto new_lhs = getCopyValue(binary_inst->getLeft(), binary_inst);
        auto new_rhs = getCopyValue(binary_inst->getRight(), binary_inst);

        new_inst = new BinaryOpInstruction(inst->getInstType(), inst->getBasicType(), basicblock, new_lhs, new_rhs, new_name);
    }
    if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        auto new_value = getCopyValue(unary_inst->getValue(), unary_inst);

        new_inst = new UnaryOpInstruction(inst->getInstType(), inst->getBasicType(), basicblock, new_value, new_name);
    }

    return new_inst;
}

void LoopUnrolling::copyOneBasicBlockForFullUnroll(const std::list<Instruction *> &origin_insts, BasicBlock *basicblock, int32_t unroll_index, const LoopUnrollingInfo &unrolling_info) {
    for (auto &inst: origin_insts) {
        if (inst->getInstType() == BrType) {
            continue;
        }
        Instruction *copy_inst = nullptr;
        std::string new_inst_name = inst->getName() + ".lu" + std::to_string(unroll_index);
           // 其他情况下直接拷贝
        copy_inst = getCopyInstruction(inst, basicblock, new_inst_name);

        assert(copy_inst);
        copy_insts_map_[inst] = copy_inst;
    }

    auto saved_last_iterate_map = last_iterate_var_map_;
    for (auto &[phi_inst, iterate_inst]: unrolling_info.loopinfo_->iterator_var_phi_insts_) {
        if (copy_insts_map_[iterate_inst]) {
            last_iterate_var_map_[phi_inst] = copy_insts_map_[iterate_inst];
        } else {
            last_iterate_var_map_[phi_inst] = saved_last_iterate_map[iterate_inst];
        }
    }
}

void LoopUnrolling::copyBodyBasicblocksForFullUnroll(const LoopUnrollingInfo &unroll_info,
                                                     int32_t unroll_index,
                                                     std::vector<BasicBlock *> &new_basicblocks) {
    new_basicblocks.reserve(unroll_info.loopinfo_->loop_body_.size());
    for (auto body_basicblock: unroll_info.loopinfo_->loop_body_list_) {
        copy_insts_map_[body_basicblock] = new BasicBlock(curr_func_, body_basicblock->getName() + ".lu" + std::to_string(unroll_index));
        auto curr_copy_body_bb = dynamic_cast<BasicBlock *>(copy_insts_map_[body_basicblock]);
        curr_copy_body_bb->setHasJump(body_basicblock->getHasJump());
        curr_copy_body_bb->setHasRet(false);
        curr_copy_body_bb->setWhileLoopDepth(body_basicblock->getWhileLoopDepth());
    }

    for (auto body_basicblock: unroll_info.loopinfo_->loop_body_list_) {
        auto curr_copy_body_bb = dynamic_cast<BasicBlock *>(copy_insts_map_[body_basicblock]);// 循环展开之后循环的深度应该降低1

        assert(curr_copy_body_bb);
        new_basicblocks.push_back(curr_copy_body_bb);
        auto &origin_insts = body_basicblock->getInstructionList();
        for (auto &inst: origin_insts) {
            std::string new_inst_name = inst->getName() + ".lu" + std::to_string(unroll_index);
            auto copy_inst = getCopyInstruction(inst.get(), curr_copy_body_bb, new_inst_name);
            assert(copy_inst);
            copy_insts_map_[inst.get()] = copy_inst;
            curr_copy_body_bb->addInstruction(copy_inst);
        }
    }
    auto saved_last_iterate_map = last_iterate_var_map_;
    for (auto &[phi_inst, iterate_inst]: unroll_info.loopinfo_->iterator_var_phi_insts_) {
        if (copy_insts_map_[iterate_inst]) {
            last_iterate_var_map_[phi_inst] = copy_insts_map_[iterate_inst];
        } else {
            last_iterate_var_map_[phi_inst] = saved_last_iterate_map[iterate_inst];
        }
    }
}

void LoopUnrolling::setLastIterateVarMap(const ComputeLoops::LoopInfoPtr &loopinfo) {
    for (auto &[phi_inst, iterator_var]: loopinfo->iterator_var_phi_insts_) {
        if (auto iter_phi_inst = dynamic_cast<PhiInstruction *>(iterator_var); loopinfo->init_var_phi_insts_.count(iter_phi_inst)) {
            last_iterate_var_map_[phi_inst] = loopinfo->init_var_phi_insts_[iter_phi_inst];
        } else {
            last_iterate_var_map_[phi_inst] = iterator_var;
        }
    }
}

void LoopUnrolling::replaceWithConstForFullUnroll(const LoopUnrollingInfo &unrolling_info) {
    for (auto &[header_phi_inst, iterate_value]: unrolling_info.loopinfo_->iterator_var_phi_insts_) {
        assert(header_phi_inst);
        auto value0 = header_phi_inst->getValue(0);
        auto value1 = header_phi_inst->getValue(1);
        if (value0->getValueType() == ConstantValue) {
            header_phi_inst->replaceAllUseWith(value0);
        } else if (value1->getValueType() == ConstantValue) {
            header_phi_inst->replaceAllUseWith(value1);
        }
    }
}

void LoopUnrolling::replaceVarInNextBlock(const LoopUnrollingInfo &unrollingInfo, BasicBlock *pre_basicblock) {
    BasicBlock *succ_bb_has_phi = unrollingInfo.loopinfo_->next_block_;
    while (succ_bb_has_phi->getInstructionList().size() == 1 && !succ_bb_has_phi->getSuccessorBlocks().empty()) {
        succ_bb_has_phi = *succ_bb_has_phi->getSuccessorBlocks().begin();
    }
    for (auto &inst_uptr: succ_bb_has_phi->getInstructionList()) {
        if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst_uptr.get()); phi_inst) {
            // 如果有一个来源于header中的phi指令，就需要进行替换，尝试找到并进行替换。
            for (int i = 0; i < phi_inst->getSize(); ++i) {
                auto phi_block = phi_inst->getBasicBlock(i);
                auto phi_value = phi_inst->getValue(i);
                phi_inst->replaceWithValue(unrollingInfo.loopinfo_->enter_block_, pre_basicblock);
                phi_inst->replaceWithValue(phi_value, getCopyValue(phi_value));
            }
        } else {
            for (auto &[phi_var, value]: unrollingInfo.loopinfo_->iterator_var_phi_insts_) {
                inst_uptr->replaceWithValue(phi_var, getCopyValue(phi_var));
            }
        }
    }
}

void LoopUnrolling::insertCopyInsts(const std::list<Instruction *> &origin_insts, BasicBlock *target_block, BasicBlock::InstructionListIt *insert_it) {
    assert(origin_insts.size() > 0);
    std::vector<Instruction *> inserted_insts;
    inserted_insts.reserve(origin_insts.size());
    for (auto inst: origin_insts) {
        auto copy_inst = dynamic_cast<Instruction *>(copy_insts_map_[inst]);
        if (copy_inst) {
            inserted_insts.push_back(copy_inst);
        }
    }

    if (insert_it) {
        for (auto insert_inst: inserted_insts) {
            target_block->insertInstruction(*insert_it, insert_inst);
        }
    } else {
        for (auto insert_inst: inserted_insts) {
            target_block->addInstruction(insert_inst);
        }
    }
}

void LoopUnrolling::unroll(ComputeLoops::LoopInfoPtr &loopinfo) {
    if (loopinfo->has_ret_or_break_) {
        return;
    }
    auto cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());

    if (!cmp_inst) {
        return;
    }
    if (cmp_inst->getBasicType() != INT_BTYPE) {
        return;
    }
    auto cmp_type = cmp_inst->getCmpType();
    if (cmp_type == SetCondInstruction::SetEQ || cmp_type == SetCondInstruction::SetNE) {
        return;
    }
    auto main_index_phi_var = loopinfo->getCondVarPhiInst();
    if (!main_index_phi_var || main_index_phi_var->getParent() != loopinfo->enter_block_) {
        return;
    }

    LoopUnrollingInfo unrolling_info(loopinfo);

    if (isFixedIterations(loopinfo, unrolling_info) && unrolling_info.cal_iteratorions_cnt_ < max_inst_cnt_for_fullunroll_ && unrolling_info.cal_iteratorions_cnt_ != 270) {
        if (loopinfo->loop_body_.size() == 1) {     // 如果是循环体只有一个basicblock的情况，就直接往里面append拷贝的指令
            auto body_basicblock = *loopinfo->loop_body_.begin();
            // 设置末尾的跳转语句，从而切换到next
            auto body_branch_inst = dynamic_cast<BranchInstruction *>(body_basicblock->getInstructionList().back().get());
            assert(body_branch_inst);

            body_branch_inst->replaceWithValue(body_branch_inst->getLabel(), loopinfo->next_block_);
            // 获取末尾branch的迭代器，用来插入循环展开的指令
            auto branch_inst_it = body_basicblock->getInstructionList().end();
            branch_inst_it--;

            assert(branch_inst_it->get() == body_branch_inst);

            std::list<Instruction *> origin_insts;
            for (auto &inst_uptr: body_basicblock->getInstructionList()) {
                origin_insts.push_back(inst_uptr.get());
            }
            setLastIterateVarMap(unrolling_info.loopinfo_);
            for (int i = 1; i < unrolling_info.cal_iteratorions_cnt_; ++i) {
                copy_insts_map_.clear();
                copyOneBasicBlockForFullUnroll(origin_insts, body_basicblock, i, unrolling_info);
                insertCopyInsts(origin_insts, body_basicblock, &branch_inst_it);
            }
            replaceVarInNextBlock(unrolling_info, body_basicblock);
            replaceWithConstForFullUnroll(unrolling_info);

        } else if (loopinfo->loop_body_.size() <= 30){            // 如果有多个基本块，那么就需要额外拷贝基本块了。
            auto branch_inst = dynamic_cast<BranchInstruction *>(loopinfo->enter_block_->getInstructionList().back().get());
            BasicBlock *body_enter = nullptr, *next_bb = nullptr;
            auto true_label_bb = dynamic_cast<BasicBlock *>(branch_inst->getTrueLabel());
            auto false_label_bb = dynamic_cast<BasicBlock *>(branch_inst->getFalseLabel());
            if (loopinfo->isInLoop(true_label_bb)) {
                body_enter = true_label_bb;
                next_bb = false_label_bb;
            } else {
                body_enter = false_label_bb;
                next_bb = true_label_bb;
            }           // 设置好next bb用于给
            auto blocks_it = curr_func_->getBlocks().begin();
            for (; blocks_it != curr_func_->getBlocks().end(); ++blocks_it) {
                if (blocks_it->get() == loopinfo->next_block_) {
                    break;
                }
            }
            // 定位到要插入点的basicblock iterator
            setLastIterateVarMap(unrolling_info.loopinfo_);
            auto branch_next_inst = dynamic_cast<BranchInstruction *>(loopinfo->exit_block_->getInstructionList().back().get());
            // printf("the exit block is %s\n", loopinfo->exit_block_->getName().c_str());
            assert(!branch_next_inst->isCondBranch());
            std::vector<BranchInstruction *> br_next_bb_insts;
            std::vector<BasicBlock *> enter_basicblocks;
            br_next_bb_insts.reserve(unrolling_info.cal_iteratorions_cnt_);
            enter_basicblocks.reserve(unrolling_info.cal_iteratorions_cnt_);
            br_next_bb_insts.push_back(branch_next_inst);

            for (int i = 1; i < unrolling_info.cal_iteratorions_cnt_; ++i) {
                std::vector<BasicBlock *> tmp_new_basicblocks;
                copy_insts_map_.clear();
                copyBodyBasicblocksForFullUnroll(unrolling_info, i, tmp_new_basicblocks);
                auto copyed_br_next_inst = dynamic_cast<BranchInstruction *>(getCopyValue(branch_next_inst));
                br_next_bb_insts.push_back(dynamic_cast<BranchInstruction *>(copyed_br_next_inst));
                enter_basicblocks.push_back(dynamic_cast<BasicBlock *>(getCopyValue(body_enter)));
                setCopyUnfinished(tmp_new_basicblocks);
                for (auto new_basicblock: tmp_new_basicblocks) {
                    curr_func_->insertBlock(blocks_it, new_basicblock);
                }
            }
            enter_basicblocks.push_back(loopinfo->next_block_);

            for (int i = 0; i < br_next_bb_insts.size(); ++i) {     // 设置每一个展开loop的跳转点
                auto br_next_bb_inst = br_next_bb_insts[i];
                br_next_bb_inst->replaceWithValue(br_next_bb_inst->getLabel(), enter_basicblocks[i]);
                // printf("the br next inst in bb %s set target %s\n", br_next_bb_inst->getParent()->getName().c_str(), branch_next_inst->getLabel()->getName().c_str());
            }

            auto pre_bb_for_next = dynamic_cast<BasicBlock *>(getCopyValue(unrolling_info.loopinfo_->next_block_));
            replaceVarInNextBlock(unrolling_info, pre_bb_for_next);
            replaceWithConstForFullUnroll(unrolling_info);
        }
    } else if (unrolling_info.limit_value_) {        // 限制为+1，或者-1的
        // 首先修改原循环的条件，以及stride，展开四部时，一般只需要条件修改为-3，并且把拷贝出来的部分插入就可以了
        // 需要加一个限制，那就是比较符号的限制
        if (loopinfo->loop_body_.size() == 1) {
            // 根据原本循环的方式拷贝得到一个小循环，小循环
            std::unordered_set<BasicBlock *> tail_loop;

            // 在原有循环的基础上进行加四，展开
            auto body_basicblock = *loopinfo->loop_body_.begin();
            // 设置末尾的跳转语句，从而切换到next
            auto body_branch_inst = dynamic_cast<BranchInstruction *>(body_basicblock->getInstructionList().back().get());
            assert(body_branch_inst);

            // 获取末尾branch的迭代器，用来插入循环展开的指令
            auto branch_inst_it = body_basicblock->getInstructionList().end();
            branch_inst_it--;

            assert(branch_inst_it->get() == body_branch_inst);
            // 收集原是的循环的指令
            std::list<Instruction *> origin_insts;
            for (auto &inst_uptr: body_basicblock->getInstructionList()) {
                origin_insts.push_back(inst_uptr.get());
            }
            std::list<Instruction *> enter_origin_insts;
            for (auto &inst_uptr: loopinfo->enter_block_->getInstructionList()) {
                enter_origin_insts.push_back(inst_uptr.get());
            }
            setLastIterateVarMap(unrolling_info.loopinfo_);

            for (int i = 1; i < 4; ++i) {
                copy_insts_map_.clear();
                copyOneBasicBlockForFullUnroll(origin_insts, body_basicblock, i, unrolling_info);
                insertCopyInsts(origin_insts, body_basicblock, &branch_inst_it);
            }
            auto tail_start_block = new BasicBlock(curr_func_, loopinfo->enter_block_->getName() + ".ul");
            auto tail_body_block = new BasicBlock(curr_func_, body_basicblock->getName() + ".ul");
            // 设置enter block中的phi指令

            auto &enter_block_insts = loopinfo->enter_block_->getInstructionList();
            auto inst_it = enter_block_insts.begin();
            auto new_limit_value = new BinaryOpInstruction(SubType, BasicType::INT_BTYPE, loopinfo->enter_block_, unrolling_info.limit_value_, new ConstantVar(3), unrolling_info.limit_value_->getName() + ".ul");
            BranchInstruction *enter_br_inst = nullptr;
            SetCondInstruction *enter_setcond_inst = nullptr;
            for (; inst_it != enter_block_insts.end(); ++inst_it) {
                auto inst = inst_it->get();
                if (inst->getInstType() == PhiType) {
                    auto phi_inst = dynamic_cast<PhiInstruction *>(inst);
                    for (int i = 0; i < phi_inst->getSize(); ++i) {
                        auto vb_pair = phi_inst->getValueBlock(i);
                        if (vb_pair.second == body_basicblock) {
                            phi_inst->replaceWithValue(vb_pair.first, copy_insts_map_[vb_pair.first]);
                        }
                    }
                } else if (inst->getInstType() == SetCondType) {
                    enter_setcond_inst = dynamic_cast<SetCondInstruction *>(inst);
                    inst->replaceWithValue(unrolling_info.limit_value_, new_limit_value);
                    break;
                }
            }
            // enter_br_inst->replaceWithValue(next_block_value, tail_start_block);
            loopinfo->enter_block_->insertInstruction(inst_it++, new_limit_value);
            enter_br_inst = dynamic_cast<BranchInstruction *>(inst_it->get());
            auto next_block_value = enter_br_inst->getTrueLabel() == body_basicblock ? enter_br_inst->getFalseLabel(): enter_br_inst->getTrueLabel();
            enter_br_inst->replaceWithValue(next_block_value, tail_start_block);
            // 还需要调整branch
            copy_insts_map_[body_basicblock] = tail_body_block;
            copy_insts_map_[loopinfo->enter_block_] = tail_start_block;
            // 在主循环后边插入之前拷贝的部分
            last_iterate_var_map_.clear();
            copyOneBasicBlockForFullUnroll(enter_origin_insts, tail_start_block, 0, unrolling_info);
            insertCopyInsts(enter_origin_insts, tail_start_block, nullptr);
            // auto tail_enter_br_inst = new BranchInstruction();
            // 加入enter br inst
            auto origin_enter_br_inst = dynamic_cast<BranchInstruction *>(enter_origin_insts.back());
            assert(origin_enter_br_inst);
            auto tail_enter_br_inst = new BranchInstruction(tail_start_block,
                                                            copy_insts_map_[origin_enter_br_inst->getCond()],
                                                            copy_insts_map_[origin_enter_br_inst->getTrueLabel()],
                                                            next_block_value);
            tail_start_block->setBranchInst(tail_enter_br_inst);
            tail_start_block->addInstruction(tail_enter_br_inst);
            assert(!tail_start_block->getInstructionList().empty());

            ComputeLoops::LoopInfoPtr tmp_loop_info = std::make_shared<ComputeLoops::LoopInfo>();
            tmp_loop_info->enter_block_ = tail_start_block;
            tmp_loop_info->exit_block_ = tail_body_block;
            tmp_loop_info->setInteratorVarPhiInsts();

            last_iterate_var_map_.clear();
            setLastIterateVarMap(tmp_loop_info);

            for (auto &[phi_inst, var]: tmp_loop_info->iterator_var_phi_insts_) {
                // printf("%s and %s\n", phi_inst->getName().c_str(), var->getName().c_str());
                last_iterate_var_map_[phi_inst] = phi_inst;
                copy_insts_map_[phi_inst] = phi_inst;
            }

            copyOneBasicBlockForFullUnroll(origin_insts, tail_body_block, 0, unrolling_info);
            insertCopyInsts(origin_insts, tail_body_block, nullptr);
            auto tail_body_br_inst = new BranchInstruction(tail_body_block, tail_start_block);
            tail_body_block->setBranchInst(tail_body_br_inst);
            tail_body_block->addInstruction(tail_body_br_inst);
            assert(!tail_body_block->getInstructionList().empty());
            // 调整tail start中的phi以及br指令
            std::unordered_map<PhiInstruction *, PhiInstruction *> enter_phi_copy_map;
            for (auto &[value, copy_value]: copy_insts_map_) {
                if (auto phi_inst = dynamic_cast<PhiInstruction *>(value); phi_inst && phi_inst->getParent() == loopinfo->enter_block_) {
                    enter_phi_copy_map[dynamic_cast<PhiInstruction *>(copy_value)] = phi_inst;
                }
            }


            for (auto &iter_pair: tmp_loop_info->iterator_var_phi_insts_) {
                auto origin_phi_inst = enter_phi_copy_map[iter_pair.first];
                for (int i = 0; i < iter_pair.first->getSize(); ++i) {
                    auto value_bb = iter_pair.first->getValueBlock(i);
                    if (value_bb.second != tail_body_block) {
                        iter_pair.first->replaceWithValue(value_bb.second, loopinfo->enter_block_);
                        iter_pair.first->replaceWithValue(value_bb.first, origin_phi_inst);
                    } else {
                        iter_pair.first->replaceWithValue(value_bb.first, last_iterate_var_map_[origin_phi_inst]);
                    }
                }
            }

            // 获取tail start中的set cond指令
            auto tail_enter_setcond_inst = dynamic_cast<SetCondInstruction *>(copy_insts_map_[enter_setcond_inst]);
            tail_enter_setcond_inst->replaceWithValue(new_limit_value, unrolling_info.limit_value_);

            // 寻找basic block的插入点
            auto blocks_insert_it = curr_func_->getBlocks().begin();
            for (; blocks_insert_it != curr_func_->getBlocks().end(); ++blocks_insert_it) {
                auto block = blocks_insert_it->get();
                if (block == body_basicblock) {
                    break;
                }
            }
            blocks_insert_it++;
            curr_func_->insertBlock(blocks_insert_it, tail_start_block);
            curr_func_->insertBlock(blocks_insert_it, tail_body_block);
            // 对于循环外部引用了while中phi的地方
            std::unordered_map<PhiInstruction *, Value *> replace_phi_map;
            for (auto &[value, copy_value]: copy_insts_map_) {
                auto phi_value = dynamic_cast<PhiInstruction *>(value);
                if (phi_value && value != copy_value) {
                    replace_phi_map[phi_value] = copy_value;
                }
            }

            for (auto &bb_uptr: curr_func_->getBlocks()) {
                auto bb = bb_uptr.get();
                if (bb == loopinfo->enter_block_ || bb == body_basicblock || bb == tail_start_block || bb == tail_body_block) {
                    continue;
                }
                auto &insts_list = bb_uptr->getInstructionList();
                for (auto &inst: insts_list) {
                    for (auto &[phi_inst, value]: replace_phi_map) {
                        inst->replaceWithValue(phi_inst, value);
                    }
                }
            }
        }
    }

}

bool LoopUnrolling::isFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo, LoopUnrollingInfo &unrolling_info) const {
    SetCondInstruction *cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());
    auto cmp_lhs = cmp_inst->getLeft();
    auto cmp_rhs = cmp_inst->getRight();
    auto phi_value_inst = loopinfo->getCondVarPhiInst();
    if (phi_value_inst && phi_value_inst->getSize() != 2) {
        // printf("phi value size is %d\n", phi_value_inst->getSize());
        return false;
    }
    int32_t loop_limit_value;
    int32_t init_value;

    Value *value_from_body = nullptr;
    Value *loop_limit_value_nonconst = nullptr;
    BasicBlock *bb_from_body = nullptr;
    // printf("the cmp inst is %s, and lhs is %s, rhs is %s\n", cmp_inst->getName().c_str(), cmp_lhs->getName().c_str(), cmp_rhs->getName().c_str());
    if (cmp_lhs->getValueType() == ConstantValue) {
        auto lhs_const_value = dynamic_cast<ConstantVar *>(cmp_lhs);
        loop_limit_value = lhs_const_value->getIValue();
    } else if (cmp_rhs->getValueType() == ConstantValue) {
        auto rhs_const_value = dynamic_cast<ConstantVar *>(cmp_rhs);
        loop_limit_value = rhs_const_value->getIValue();
    } else {
        // 一个是循环内部，一个是循环外部的
        if (cmp_lhs->getValueType() == InstructionValue && cmp_rhs->getValueType() == InstructionValue) {
            auto cmp_lhs_inst = dynamic_cast<Instruction *>(cmp_lhs);
            auto cmp_rhs_inst = dynamic_cast<Instruction *>(cmp_rhs);

            auto lhs_in_loop = loopinfo->isInLoop(cmp_lhs_inst->getParent());
            auto rhs_in_loop = loopinfo->isInLoop(cmp_rhs_inst->getParent());

            if (lhs_in_loop && !rhs_in_loop) {
                loop_limit_value_nonconst = cmp_rhs;
            } else if (!lhs_in_loop && rhs_in_loop) {
                loop_limit_value_nonconst = cmp_lhs;
            }

        } else if (cmp_lhs->getValueType() == InstructionValue) {
            loop_limit_value_nonconst = cmp_rhs;
        } else if (cmp_rhs->getValueType() == InstructionValue) {
            loop_limit_value_nonconst = cmp_lhs;
        }
    }

    auto phi_bb_value_pair0 = phi_value_inst->getValueBlock(0);
    auto phi_bb_value_pair1 = phi_value_inst->getValueBlock(1);

    // printf("check the phi inst %s init value\n", phi_value_inst->getName().c_str());
    if (phi_bb_value_pair0.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair0.first)->getIValue();
        value_from_body = phi_bb_value_pair1.first;
        bb_from_body = phi_bb_value_pair1.second;
    } else if (phi_bb_value_pair1.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair1.first)->getIValue();
        value_from_body = phi_bb_value_pair0.first;
        bb_from_body = phi_bb_value_pair0.second;
    } else {
        // printf("the init value is not const for phi inst %s\n", phi_value_inst->getName().c_str());
        return false;
    }           // 如果两边都没有这样的常量值，也就是说明循环的次数无法在编译期间确定

    if (!loopinfo->isInLoop(bb_from_body)) {        // 判断这个value是否来自循环之中
        return false;
    }
    // 之后追踪这个value，判断是否是采用了加减移动了固定布长的方式
    // 处于循环体中，并且是+一个const或者减一个const的方式
    auto inst_from_body = dynamic_cast<Instruction *>(value_from_body);
    assert(inst_from_body);

    Value *binary_lhs, *binary_rhs;
    BinaryOpInstruction *binary_inst = nullptr;
    if (inst_from_body->getInstType() == AddType || inst_from_body->getInstType() == SubType) {
        binary_inst = dynamic_cast<BinaryOpInstruction *>(inst_from_body);
    }
    if (!binary_inst) {
        return false;
    }

    binary_lhs = binary_inst->getLeft();
    binary_rhs = binary_inst->getRight();


    ConstantVar *const_var = nullptr;
    if (binary_lhs == phi_value_inst && binary_rhs->getValueType() == ConstantValue) {      // 推导出步数
        const_var = dynamic_cast<ConstantVar *>(binary_rhs);
    } else if (binary_inst->getInstType() == AddType && binary_lhs->getValueType() == ConstantValue && binary_rhs == phi_value_inst) {      // 推导出步数
        const_var = dynamic_cast<ConstantVar *>(binary_lhs);
    } else {
        return false;
    }
    int32_t stride_cnt = const_var->getIValue();

    unrolling_info.init_value_ = init_value;
    unrolling_info.limit_ = loop_limit_value;
    unrolling_info.limit_value_ = loop_limit_value_nonconst;
    unrolling_info.stride_ = stride_cnt;
    unrolling_info.iterator_inst = binary_inst;

    auto cmp_type = cmp_inst->getCmpType();

    if (!unrolling_info.limit_value_) {
        unrolling_info.cal_iteratorions_cnt_ = std::abs((loop_limit_value - init_value) / stride_cnt);
        if (cmp_type == SetCondInstruction::SetLE || cmp_type == SetCondInstruction::SetGE) {
            unrolling_info.cal_iteratorions_cnt_++;
        }
    } else {
        return false;
    }

    return true;
}

bool LoopUnrolling::isNotFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo) const {

    return false;
}

void LoopUnrolling::setCopyUnfinished(const std::vector<BasicBlock *> &new_basicblocks) {
    for (auto copyed_bb: new_basicblocks) {
        for (auto &inst_uptr: copyed_bb->getInstructionList()) {
            auto copyed_inst = dynamic_cast<Instruction *>(getCopyValue(inst_uptr.get()));
            assert(copyed_inst);
            if (may_unfinished_copy_insts_.count(inst_uptr.get())) {
                for (int i = 0; i < inst_uptr->getOperandNum(); ++i) {
                    auto operand = inst_uptr->getOperand(i);
                    auto copy_operand = copyed_inst->getOperand(i);
                    if (copy_operand) {
                        assert(getCopyValue(operand));
                        copyed_inst->setOperand(getCopyValue(operand), i);
                    }
                }
            }
        }
    }
}

void LoopUnrolling::runOnFunction() {
    if (!has_compute_loop_) {
        compute_loops_.release();
        compute_loops_ = std::make_unique<ComputeLoops>(module_);
        compute_loops_->run();
        has_compute_loop_ = true;
    }
    auto &loop_infos = compute_loops_->getDeepestLoops(curr_func_);
    for (auto &loop_info: loop_infos) {
        // printf("the enter bb is %s, and exit bb is %s, the body bb cnt is %d\n", loop_info->enter_block_->getName().c_str(), loop_info->exit_block_->getName().c_str(), loop_info->loop_body_.size());
        assert(loop_info);
        unroll(loop_info);
    }

    curr_func_->rebuildCfg();

}