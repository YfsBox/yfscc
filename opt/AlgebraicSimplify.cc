//
// Created by 杨丰硕 on 2023/6/21.
//
#include <cmath>
#include "AlgebraicSimplify.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"
#include "../ir/Constant.h"
#include "../ir/Value.h"
#include "../ir/IrDumper.h"

void AlgebraicSimplify::runOnFunction() {
    removeInsts();
    replaceWithSimpleInst();
}

bool AlgebraicSimplify::isPowerOfTwo(int32_t num) {
    if (num <= 0) {
        return false;
    }
    return (num & (num - 1)) == 0;
}

void AlgebraicSimplify::removeInsts() {
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            auto inst = inst_uptr.get();

            if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
                auto inst_type = binary_inst->getInstType();
                auto lhs = binary_inst->getLeft();
                auto rhs = binary_inst->getRight();
                double value = 2;
                Value *replaced_value;
                if (inst_type == AddType) {
                    if (auto const_lhs = dynamic_cast<ConstantVar *>(lhs); const_lhs) {
                        value = const_lhs->isFloat() ? const_lhs->getFValue() : const_lhs->getIValue();
                        replaced_value = rhs;
                    } else if (auto const_rhs = dynamic_cast<ConstantVar *>(rhs); const_rhs) {
                        value = const_rhs->isFloat() ? const_rhs->getFValue() : const_rhs->getIValue();
                        replaced_value = lhs;
                    }
                    if (value == 0.0) {
                        wait_delete_insts_[inst] = replaced_value;
                    }
                } else if (inst_type == SubType) {
                    if (auto const_rhs = dynamic_cast<ConstantVar *>(rhs); const_rhs) {
                        value = const_rhs->getBasicType() == INT_BTYPE ? const_rhs->getIValue() : const_rhs->getFValue();
                        replaced_value = lhs;
                    }
                    if (value == 0.0) {
                        wait_delete_insts_[inst] = replaced_value;
                    }
                } else if (inst_type == MulType) {
                    if (auto const_lhs = dynamic_cast<ConstantVar *>(lhs); const_lhs) {
                        value = const_lhs->isFloat() ? const_lhs->getFValue() : const_lhs->getIValue();
                        replaced_value = rhs;
                    } else if (auto const_rhs = dynamic_cast<ConstantVar *>(rhs); const_rhs) {
                        value = const_rhs->isFloat() ? const_rhs->getFValue() : const_rhs->getIValue();
                        replaced_value = lhs;
                    }
                    if (value == 1) {
                        wait_delete_insts_[inst] = replaced_value;
                    }
                } else if (inst_type == DivType) {
                    if (auto const_rhs = dynamic_cast<ConstantVar *>(rhs); const_rhs) {
                        value = const_rhs->getBasicType() == INT_BTYPE ? const_rhs->getIValue() : const_rhs->getFValue();
                        replaced_value = lhs;
                    }
                    if (value == 1) {
                        wait_delete_insts_[inst] = replaced_value;
                    }
                }
            }
        }
    }

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &bb_insts_list = bb_uptr->getInstructionList();
        for (auto inst_it = bb_insts_list.begin(); inst_it != bb_insts_list.end();) {
            auto inst = inst_it->get();
            if (wait_delete_insts_.count(inst)) {
                inst->replaceAllUseWith(wait_delete_insts_[inst]);
                inst_it = bb_insts_list.erase(inst_it);
            } else {
                inst_it++;
            }
        }
    }

}

void AlgebraicSimplify::replaceWithSimpleInst() {
    for (auto &bb: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb->getInstructionList()) {
            if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst_uptr.get()); binary_inst) {
                auto lhs = binary_inst->getLeft();
                auto rhs = binary_inst->getRight();
                int32_t pcnt = 0;
                if (binary_inst->getInstType() == MulType && (lhs->getValueType() == ConstantValue || rhs->getValueType() == ConstantValue)) {
                    ConstantVar *const_var = nullptr;
                    ConstantVar *shift_var = nullptr;
                    if (lhs->getValueType() == ConstantValue) {
                        const_var = dynamic_cast<ConstantVar *>(lhs);
                        binary_inst->swapLeftAndRight();
                    } else {
                        const_var = dynamic_cast<ConstantVar *>(rhs);
                    }
                    assert(const_var);
                    int32_t const_var_value = const_var->getIValue();
                    if (isPowerOfTwo(const_var_value)) {
                        pcnt = static_cast<int32_t>(std::log2(const_var_value));
                        shift_var = new ConstantVar(pcnt);
                        inst_uptr->replaceWithValue(const_var, shift_var);
                        inst_uptr->setInstType(LshrType);
                        printf("%d replace by %d in inst %s for mul\n", const_var_value, pcnt, inst_uptr->getName().c_str());
                    }

                } /*else if (binary_inst->getInstType() == DivType && rhs->getValueType() == ConstantValue) {
                    ConstantVar *const_var = dynamic_cast<ConstantVar *>(rhs);
                    assert(const_var);
                    int32_t const_var_value = const_var->getIValue();
                    if (isPowerOfTwo(const_var_value)) {
                        pcnt = static_cast<int32_t>(std::log2(const_var_value));
                        auto shift_var = new ConstantVar(pcnt);
                        inst_uptr->replaceWithValue(const_var, shift_var);
                        inst_uptr->setInstType(RshrType);
                        printf("%d replace by %d in inst %s for div\n", const_var_value, pcnt, inst_uptr->getName().c_str());
                    }
                }*/
            }
        }
    }


}
