//
// Created by 杨丰硕 on 2023/6/21.
//
#include "ComputeLoops.h"
#include "LoopUnrolling.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "../ir/Value.h"
#include "../ir/IrDumper.h"

Value *LoopUnrolling::getCopyValue(Value *value) {
    Value *copy_value = nullptr;
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
            auto copy_value = getCopyValue(store_inst->getValue());
            auto copy_ptr = getCopyValue(store_inst->getPtr());

            new_inst = new StoreInstruction(basicblock, inst->getBasicType(), copy_value, copy_ptr);
            break;
        }
        case LoadType: {
            auto load_inst = dynamic_cast<const LoadInstruction *>(inst);
            auto load_addr = getCopyValue(load_inst->getPtr());
            new_inst = new LoadInstruction(basicblock, load_inst->getBasicType(), load_addr, load_inst->getBasicType(), new_name);
            break;
        }
        case GEPType: {
            auto gep_inst = dynamic_cast<const GEPInstruction *>(inst);
            std::vector<Value *> operands;
            for (int i = 0; i < gep_inst->getOperandNum(); ++i) {
                auto operand = gep_inst->getOperand(i);
                auto copy_operand = getCopyValue(operand);
                operands.push_back(copy_operand);
            }

            new_inst = new GEPInstruction(basicblock, gep_inst->getBasicType(), operands, gep_inst->getArrayDimension(), gep_inst->isPtrOffset(), new_name);
            break;
        }
        case MemSetType: {
            auto memset_inst = dynamic_cast<const MemSetInstruction *>(inst);
            auto new_size = getCopyValue(memset_inst->getSize());
            auto new_base = getCopyValue(memset_inst->getBase());
            auto new_value = getCopyValue(memset_inst->getValue());

            new_inst = new MemSetInstruction(basicblock, memset_inst->getBasicType(), new_base, new_size, new_value);
            break;
        }
        case BrType: {          // 这一部分还是需要调整的，只不过目前
            auto br_inst = dynamic_cast<const BranchInstruction *>(inst);
            if (br_inst->isCondBranch()) {
                auto new_cond = getCopyValue(br_inst->getCond());
                auto new_true_label = getCopyValue(br_inst->getTrueLabel());
                auto new_false_label = getCopyValue(br_inst->getFalseLabel());
                new_inst = new BranchInstruction(basicblock, new_cond, new_true_label, new_false_label);
            } else {
                auto new_label = getCopyValue(br_inst->getLabel());
                new_inst = new BranchInstruction(basicblock, new_label);
            }
            basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
            break;
        }
        case SetCondType: {
            auto set_cond_inst = dynamic_cast<const SetCondInstruction *>(inst);
            auto new_left = getCopyValue(set_cond_inst->getLeft());
            auto new_right = getCopyValue(set_cond_inst->getRight());

            new_inst = new SetCondInstruction(basicblock, set_cond_inst->getCmpType(), set_cond_inst->isFloatCmp(), new_left, new_right, new_name);
            break;
        }
        case CallType: {
            auto call_inst = dynamic_cast<const CallInstruction *>(inst);
            std::vector<Value *> new_actuals;
            for (int i = 0; i < call_inst->getActualSize(); ++i) {
                auto new_actual = getCopyValue(call_inst->getActual(i));
                new_actuals.push_back(new_actual);
            }

            new_inst = new CallInstruction(basicblock, call_inst->getFunction(), new_actuals, new_name);
            break;
        }
        case ZextType: {
            auto zext_inst = dynamic_cast<const ZextInstruction *>(inst);
            auto new_left = getCopyValue(zext_inst->getValue());

            new_inst = new ZextInstruction(basicblock, new_left, new_name);
            break;
        }
        case CastType: {
            auto cast_inst = dynamic_cast<const CastInstruction *>(inst);
            auto new_value = getCopyValue(cast_inst->getValue());

            new_inst = new CastInstruction(basicblock, cast_inst->isI2F(), new_value, new_name);
            break;
        }
        case PhiType: {
            auto phi_inst = dynamic_cast<const PhiInstruction *>(inst);

            std::vector<Value *> new_phi_values;
            std::vector<BasicBlock *> new_basicblocks;

            for (int i = 0; i < phi_inst->getSize(); ++i) {
                auto value_basicblock = phi_inst->getValueBlock(i);
                auto new_phi_value = getCopyValue(value_basicblock.first);
                auto new_phi_basicblock = getCopyValue(value_basicblock.second);

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
        auto new_lhs = getCopyValue(binary_inst->getLeft());
        auto new_rhs = getCopyValue(binary_inst->getRight());

        new_inst = new BinaryOpInstruction(inst->getInstType(), inst->getBasicType(), basicblock, new_lhs, new_rhs, new_name);
    }
    if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        auto new_value = getCopyValue(unary_inst->getValue());

        new_inst = new UnaryOpInstruction(inst->getInstType(), inst->getBasicType(), basicblock, new_value, new_name);
    }

    return new_inst;
}

void LoopUnrolling::copyOneBasicBlockForFullUnroll(const std::list<Instruction *> &origin_insts, BasicBlock *basicblock, int32_t unroll_index, const LoopUnrollingInfo &unrolling_info) {
    for (auto &inst: origin_insts) {
        if (inst->getInstType() == BrType) {
            continue;
        }
        auto *iterate_inst = dynamic_cast<BinaryOpInstruction *>(unrolling_info.iterator_inst);
        assert(iterate_inst);
        Instruction *copy_inst = nullptr;
        std::string new_inst_name = inst->getName() + ".lu" + std::to_string(unroll_index);
        if (iterate_inst == inst) {     // 需要变换const值
            auto lhs = iterate_inst->getLeft();
            auto rhs = iterate_inst->getRight();
            ConstantVar *const_var = nullptr;
            ConstantVar *new_const_var = nullptr;
            Value *new_lhs = nullptr, *new_rhs = nullptr;
            if (lhs->getValueType() == ConstantValue) {
                const_var = dynamic_cast<ConstantVar *>(lhs);
                new_const_var = new ConstantVar(const_var->getIValue());
                new_lhs = new_const_var;
                new_rhs = curr_condition_var_;
            } else {
                const_var = dynamic_cast<ConstantVar *>(rhs);
                new_const_var = new ConstantVar(const_var->getIValue());
                new_rhs = new_const_var;
                new_lhs = curr_condition_var_;
            }
            copy_inst = new BinaryOpInstruction(inst->getInstType(), inst->getBasicType(), basicblock, new_lhs, new_rhs, new_inst_name);
            copy_insts_map_[const_var] = new_const_var;
        } else {            // 其他情况下直接拷贝
            copy_inst = getCopyInstruction(inst, basicblock, new_inst_name);
        }
        assert(copy_inst);
        copy_insts_map_[inst] = copy_inst;
    }

}

void LoopUnrolling::unroll(ComputeLoops::LoopInfoPtr &loopinfo) {
    if (loopinfo->has_ret_or_break_) {
        return;
    }
    auto cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());

    assert(cmp_inst);
    if (cmp_inst->getBasicType() != INT_BTYPE) {
        return;
    }
    auto cmp_type = cmp_inst->getCmpType();
    if (cmp_type == SetCondInstruction::SetEQ || cmp_type == SetCondInstruction::SetNE) {
        return;
    }
    auto phi_var = loopinfo->getCondVarPhiInst();
    if (!phi_var || phi_var->getParent() != loopinfo->enter_block_) {
        return;
    }

    LoopUnrollingInfo unrolling_info(loopinfo);

    if (isFixedIterations(loopinfo, unrolling_info)) {
        /*printf("the loopinfo is %s, and the limit is %d, init is %d, stride is %d, iteration cnt is %d, the loop body cnt is %d\n",
               loopinfo->enter_block_->getName().c_str(),
               unrolling_info.limit_,
               unrolling_info.init_value_,
               unrolling_info.stride_,
               unrolling_info.cal_iteratorions_cnt_,
               loopinfo->loop_body_.size());*/

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

            curr_condition_var_ = unrolling_info.iterator_inst;
            for (int i = 1; i < unrolling_info.cal_iteratorions_cnt_; ++i) {
                copy_insts_map_.clear();
                std::list<Instruction *> unroll_insts;
                copy_insts_map_[phi_var] = curr_condition_var_;
                copyOneBasicBlockForFullUnroll(origin_insts, body_basicblock, i, unrolling_info);
                curr_condition_var_ = copy_insts_map_[unrolling_info.iterator_inst];

                std::vector<Instruction *> inserted_insts;
                inserted_insts.reserve(body_basicblock->getInstructionSize());
                for (auto &inst_uptr: body_basicblock->getInstructionList()) {
                    auto inst = inst_uptr.get();
                    auto copy_inst = dynamic_cast<Instruction *>(copy_insts_map_[inst]);
                    if (copy_inst) {
                        // ir_dumper_->dump(copy_inst);
                        // body_basicblock->insertInstruction(branch_inst_it, copy_inst);
                        inserted_insts.push_back(copy_inst);
                    }
                }

                for (auto insert_inst: inserted_insts) {
                    body_basicblock->insertInstruction(branch_inst_it, insert_inst);
                }
            }

            for (auto &inst_uptr: loopinfo->next_block_->getInstructionList()) {
                if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst_uptr.get()); phi_inst) {
                    phi_inst->replaceWithValue(loopinfo->enter_block_, body_basicblock);
                    phi_inst->replaceWithValue(phi_var, curr_condition_var_);
                }
            }

        } else {            // 如果有多个基本块，那么就需要额外拷贝基本块了。

        }




    } else if (isNotFixedIterations(loopinfo)) {



    }


}

bool LoopUnrolling::isFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo, LoopUnrollingInfo &unrolling_info) const {
    SetCondInstruction *cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());
    auto cmp_lhs = cmp_inst->getLeft();
    auto cmp_rhs = cmp_inst->getRight();
    auto phi_value_inst = loopinfo->getCondVarPhiInst();
    if (phi_value_inst->getSize() != 2) {
        return false;
    }
    int32_t loop_limit_value;
    int32_t init_value;

    Value *value_from_body = nullptr;
    BasicBlock *bb_from_body = nullptr;
    if (cmp_lhs->getValueType() == ConstantValue) {
        auto lhs_const_value = dynamic_cast<ConstantVar *>(cmp_lhs);
        loop_limit_value = lhs_const_value->getIValue();
    } else {
        auto rhs_const_value = dynamic_cast<ConstantVar *>(cmp_rhs);
        loop_limit_value = rhs_const_value->getIValue();
    }

    auto phi_bb_value_pair0 = phi_value_inst->getValueBlock(0);
    auto phi_bb_value_pair1 = phi_value_inst->getValueBlock(1);

    if (phi_bb_value_pair0.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair0.first)->getIValue();
        value_from_body = phi_bb_value_pair1.first;
        bb_from_body = phi_bb_value_pair1.second;
    } else if (phi_bb_value_pair1.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair1.first)->getIValue();
        value_from_body = phi_bb_value_pair0.first;
        bb_from_body = phi_bb_value_pair0.second;
    } else {
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
    unrolling_info.stride_ = stride_cnt;
    unrolling_info.iterator_inst = binary_inst;

    auto cmp_type = cmp_inst->getCmpType();
    unrolling_info.cal_iteratorions_cnt_ = std::abs((loop_limit_value - init_value) / stride_cnt);
    if (cmp_type == SetCondInstruction::SetLE || cmp_type == SetCondInstruction::SetGE) {
        unrolling_info.cal_iteratorions_cnt_++;
    }

    return true;
}

bool LoopUnrolling::isNotFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo) const {

    return false;
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
        /*printf("the enter bb is %s, and exit bb is %s\n", loop_info->enter_block_->getName().c_str(), loop_info->exit_block_->getName().c_str());
        for (auto body_bb: loop_info->loop_body_) {
            printf("the body bb is %s\n", body_bb->getName().c_str());
        }*/
        unroll(loop_info);
    }

    curr_func_->rebuildCfg();

}