//
// Created by 杨丰硕 on 2023/6/10.
//
#include <cassert>
#include "../ir/BasicBlock.h"
#include "../ir/Function.h"
#include "../ir/Constant.h"
#include "../ir/IrDumper.h"
#include "InstCombine.h"

bool InstCombine::canCombine(BinaryOpInstruction *inst) {
    auto left = inst->getLeft();
    auto right = inst->getRight();
    auto binary_op_type = inst->getInstType();
    return (left->getValueType() == InstructionValue && right->getValueType() == ConstantValue)
    && (binary_op_type == AddType || binary_op_type == SubType || binary_op_type == MulType);
}

void InstCombine::initWorkList() {
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

                if (!canCombine(binary_inst)) {
                    continue;
                }
                work_insts_set_.insert(binary_inst);
            }
        }
    }
}

void InstCombine::combine(BinaryOpInstruction *inst) {
    auto left_inst = dynamic_cast<Instruction *>(inst->getLeft());
    auto right_constvar = dynamic_cast<ConstantVar *>(inst->getRight());

    assert(left_inst);
    assert(right_constvar);

    auto left_binary_inst = dynamic_cast<BinaryOpInstruction *>(left_inst);
    if (left_binary_inst && canCombine(left_binary_inst)) {
        combine(left_binary_inst);
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
        ir_dumper_->dump(inst);
        combined_insts_set_.insert(inst);
    }

}

void InstCombine::runOnFunction() {
    initWorkList();

    for (auto inst: work_insts_set_) {
        if (combined_insts_set_.count(inst)) {
            continue;
        }
        combine(inst);
    }
}
