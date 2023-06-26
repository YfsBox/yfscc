//
// Created by 杨丰硕 on 2023/6/9.
//
#include <cassert>
#include "ConstantPropagation.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "../ir/IrDumper.h"

bool ConstantPropagation::checkCanFold(Instruction *inst) {
    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
        auto left = binary_inst->getLeft();
        auto right = binary_inst->getRight();
        if (left->getValueType() == ConstantValue && right->getValueType() == ConstantValue) {
            return true;
        }
    }
    if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        auto value = unary_inst->getValue();
        if (value->getValueType() == ConstantValue) {
            return true;
        }
    }
    return false;
}

void ConstantPropagation::initForWorkList() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &inst_list = bb->getInstructionList();
        for (auto &inst: inst_list) {
            if (checkCanFold(inst.get())) {
                work_inst_list_.push(inst.get());
            }
        }
    }
}

void ConstantPropagation::runOnFunction() {
    initForWorkList();
    while (!work_inst_list_.empty()) {
        auto inst = work_inst_list_.front();
        work_inst_list_.pop();
        ConstantVar *const_var;
        if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
            auto binary_type = binary_inst->getInstType();
            ConstantVar *left = dynamic_cast<ConstantVar *>(binary_inst->getLeft());
            ConstantVar *right = dynamic_cast<ConstantVar *>(binary_inst->getRight());
            assert(left);
            assert(right);

            double left_value, right_value;

            if (binary_inst->getBasicType() == INT_BTYPE) {
                left_value = left->getIValue();
                right_value = right->getIValue();
            } else {
                left_value = left->getFValue();
                right_value = right->getFValue();
            }

            double cal_value;
            switch (binary_type) {
                case AddType:
                    cal_value = left_value + right_value;
                    break;
                case SubType:
                    cal_value = left_value - right_value;
                    break;
                case MulType:
                    cal_value = left_value * right_value;
                    break;
                case DivType:
                    cal_value = left_value / right_value;
                    break;
                case ModType:
                    cal_value = static_cast<int32_t>(left_value) % static_cast<int32_t>(right_value);
                    break;
                case OrType:
                    cal_value = static_cast<int32_t>(left_value) || static_cast<int32_t>(right_value);
                    break;
                case LshrType:
                    cal_value = static_cast<int32_t>(left_value) << static_cast<int32_t>(right_value);
                default:
                    break;
            }

            if (binary_inst->getBasicType() == INT_BTYPE) {
                const_var = new ConstantVar(static_cast<int32_t>(cal_value));
            } else {
                const_var = new ConstantVar(static_cast<float>(cal_value));
            }
        } else {
            auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst);
            assert(unary_inst);
            auto unary_operand = dynamic_cast<ConstantVar *>(unary_inst->getValue());
            double unary_value = unary_inst->getBasicType() == INT_BTYPE ? unary_operand->getIValue() : unary_operand->getFValue();
            if (unary_inst->getInstType() == NegType) {
                if (unary_inst->getBasicType() == INT_BTYPE) {
                    int64_t neg_value = -unary_operand->getIValue();
                    unary_value = static_cast<double>(neg_value);
                } else {
                    unary_value = -1 * unary_value;
                }
            } else if (unary_inst->getInstType() == NotType) {
                unary_value = !unary_value;
            }

            if (unary_inst->getBasicType() == INT_BTYPE) {
                const_var = new ConstantVar(static_cast<int32_t>(unary_value));
            } else {
                const_var = new ConstantVar(static_cast<float>(unary_value));
            }
        }
        // 将引用了该指令的指令进行替换
        for (auto inst_user: inst->getUserMap()) {
            inst_user->replaceWithValue(inst, const_var);
            auto user_inst = dynamic_cast<Instruction *>(inst_user);
            if (user_inst && checkCanFold(user_inst)) {
                work_inst_list_.push(user_inst);
            }
        }
    }
}