//
// Created by 杨丰硕 on 2023/6/10.
//
#include <cassert>
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "../ir/Constant.h"
#include "../ir/IrDumper.h"
#include "InstCombine.h"

bool InstCombine::canCombineWithConst(BinaryOpInstruction *inst) {
    auto left = inst->getLeft();
    auto right = inst->getRight();
    auto binary_op_type = inst->getInstType();
    return (left->getValueType() == InstructionValue && right->getValueType() == ConstantValue)
    && (binary_op_type == AddType || binary_op_type == SubType || binary_op_type == MulType);
}

bool InstCombine::canCombineNonConst(BinaryOpInstruction *inst, Value **value) {
    auto left = inst->getLeft();
    auto right = inst->getRight();

    auto binary_op_type = inst->getInstType();
    if (binary_op_type == AddType) {
        if (auto left_inst = dynamic_cast<BinaryOpInstruction *>(left); left_inst) {
            if (left_inst->getInstType() == AddType && (left_inst->getLeft() == right || left_inst->getRight() == right)) {
                *value = right;
                return true;
            }
        }
        if (auto right_inst = dynamic_cast<BinaryOpInstruction *>(right); right_inst) {
            if (right_inst->getInstType() == AddType && (right_inst->getLeft() == left || right_inst->getRight() == left)) {
                *value = left;
                return true;
            }
        }
    }
    return false;
}

void InstCombine::initWorkList() {
    combine_value_cnt_.clear();
    for (auto &bb: curr_func_->getBlocks()) {
        auto &inst_list = bb->getInstructionList();
        for (auto &inst: inst_list) {
            if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst.get()); binary_inst) {
                auto left = binary_inst->getLeft();
                auto right = binary_inst->getRight();

                if (binary_inst->getInstType() != AddType && binary_inst->getInstType() != SubType && binary_inst->getInstType() != MulType) {
                    continue;
                }

                if (left->getValueType() == ConstantValue && right->getValueType() == InstructionValue && binary_inst->getInstType() != SubType) {
                    binary_inst->swapLeftAndRight();
                }
                Value *value;
                if (canCombineWithConst(binary_inst)) {
                    work_insts_withconst_set_.insert(binary_inst);
                } else if (canCombineNonConst(binary_inst, &value)) {
                    work_insts_nonconst_map_.insert({binary_inst, value});
                }
            }
        }
    }
}

void InstCombine::combineWithConst(BinaryOpInstruction *inst) {
    auto left_inst = dynamic_cast<Instruction *>(inst->getLeft());
    auto right_constvar = dynamic_cast<ConstantVar *>(inst->getRight());

    assert(left_inst);
    assert(right_constvar);

    auto left_binary_inst = dynamic_cast<BinaryOpInstruction *>(left_inst);
    if (left_binary_inst && canCombineWithConst(left_binary_inst)) {
        combineWithConst(left_binary_inst);
    } else {
        return;
    }

    auto left_op = left_binary_inst->getInstType();
    auto inst_op = inst->getInstType();

    double combined_right_value;
    double inst_right_value, left_binst_right_value;

    if (inst->getBasicType() == INT_BTYPE) {
        inst_right_value = right_constvar->getIValue();
        left_binst_right_value = dynamic_cast<ConstantVar *>(left_binary_inst->getRight())->getIValue();
    } else {
        inst_right_value = right_constvar->getFValue();
        left_binst_right_value = dynamic_cast<ConstantVar *>(left_binary_inst->getRight())->getFValue();
    }

    bool has_combined = false;
    ConstantVar *constvar;

    if (inst_op == left_op && inst_op != DivType) {
        switch (inst_op) {
            case AddType:
                combined_right_value = inst_right_value + left_binst_right_value;
                break;
            case SubType:
                combined_right_value = inst_right_value + left_binst_right_value;
                break;
            case MulType:
                combined_right_value = inst_right_value * left_binst_right_value;
                break;
            default:
                break;
        }
        has_combined = true;
    } else if (inst->getInstType() == AddType && left_binary_inst->getInstType() == SubType) {
        combined_right_value = inst_right_value - left_binst_right_value;
        has_combined = true;
    } else if (inst->getInstType() == SubType && left_binary_inst->getInstType() == AddType) {
        combined_right_value = inst_right_value - left_binst_right_value;
        has_combined = true;
    }

    if (has_combined) {
        if (inst->getBasicType() == INT_BTYPE) {
            constvar = new ConstantVar(static_cast<int32_t>(combined_right_value));
        } else {
            constvar = new ConstantVar(static_cast<float>(combined_right_value));
        }
        inst->replaceWithValue(inst->getLeft(), left_binary_inst->getLeft());
        inst->replaceWithValue(inst->getRight(), constvar);
        combined_insts_withconst_set_.insert(inst);
    }

}

void InstCombine::combineNonConst(BinaryOpInstruction *inst) {
    auto left_inst = dynamic_cast<BinaryOpInstruction *>(inst->getLeft());
    auto right_inst = dynamic_cast<BinaryOpInstruction *>(inst->getRight());

    assert(left_inst);
    assert(right_inst);

    bool has_sub_combine = false;
    if (left_inst) {
        if (left_inst->getInstType() == AddType && (left_inst->getLeft() == right_inst || left_inst->getRight() == right_inst)) {
            if (canCombineNonConst(left_inst)) {
                has_sub_combine = true;
                combineNonConst(left_inst);
            }
            if (left_inst->getLeft() == right_inst) {
                inst->replaceWithValue(left_inst, left_inst->getRight());
            } else {
                inst->replaceWithValue(left_inst, left_inst->getLeft());
            }
            combine_value_cnt_[right_inst]++;
            combined_insts_nonconst_set_.insert(inst);
        }
    }

    if (right_inst) {
        if (right_inst->getInstType() == AddType && (right_inst->getLeft() == left_inst || right_inst->getRight() == left_inst)) {
            if (canCombineNonConst(right_inst)) {
                has_sub_combine = true;
                combineNonConst(right_inst);
            }
            if (right_inst->getLeft() == left_inst) {
                inst->replaceWithValue(right_inst, right_inst->getRight());
            } else {
                inst->replaceWithValue(right_inst, right_inst->getLeft());
            }
            combine_value_cnt_[left_inst]++;
            combined_insts_nonconst_set_.insert(inst);
        }
    }

}

void InstCombine::runOnFunction() {
    initWorkList();

    for (auto inst: work_insts_withconst_set_) {
        if (combined_insts_withconst_set_.count(inst)) {
            continue;
        }
        combineWithConst(inst);
    }

    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        inserted_inst_.clear();
        combine_value_cnt_.clear();
        for (auto inst_it = insts_list.rbegin(); inst_it != insts_list.rend(); ++inst_it) {
            auto inst = inst_it->get();
            Value *common_value = nullptr;
            if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst);
            work_insts_nonconst_map_.count(binary_inst)
            && !combined_insts_nonconst_set_.count(binary_inst)) {
                combine_value_cnt_[binary_inst->getLeft()] = 1;
                combine_value_cnt_[binary_inst->getRight()] = 1;
                common_value = work_insts_nonconst_map_[binary_inst];
                assert(common_value);
                combineNonConst(binary_inst);
                std::string inserted_inst_name = "inst_com" + binary_inst->getName();
                ConstantVar *mul_cnt;
                if (binary_inst->getBasicType() == INT_BTYPE) {
                    mul_cnt = new ConstantVar(combine_value_cnt_[common_value]);
                    inserted_inst_[binary_inst] = new BinaryOpInstruction(MulType, INT_BTYPE, bb.get(), common_value, mul_cnt, inserted_inst_name);
                } else {
                    mul_cnt = new ConstantVar(static_cast<float>(combine_value_cnt_[common_value]));
                    inserted_inst_[binary_inst] = new BinaryOpInstruction(MulType, FLOAT_BTYPE, bb.get(), common_value, mul_cnt, inserted_inst_name);
                }
            }
        }
        // 正序遍历用来设置迭代器map
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
            auto inst = inst_it->get();
            if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst && inserted_inst_.count(binary_inst)) {
                inserted_it_[binary_inst] = inst_it;
            }
        }

        for (auto &[inserted_point, inserted_it]: inserted_it_) {
            auto inserted_inst = inserted_inst_[inserted_point];
            bb->insertInstruction(inserted_it, inserted_inst);
        }
    }
}
