//
// Created by 杨丰硕 on 2023/6/28.
//

#include "GlobalValueNumber.h"
#include "GlobalAnalysis.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "../ir/GlobalVariable.h"

GlobalValueNumber::GlobalValueNumber(Module *module): Pass(module), user_analysis_(std::make_unique<UserAnalysis>()) {
    pass_name_ = "GlobalValueNumber";
}

void GlobalValueNumber::runOnFunction() {
    replace_values_map_.clear();
    value_number_map_.clear();
    visited_set_.clear();
    user_analysis_->analysis(curr_func_);

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            numberValue(inst_uptr.get());
        }
    }

    replaceValues();
}

Value *GlobalValueNumber::lookupOrAdd(const std::string &value_number, Value *value) {
    // printf("value %s look up name %s\n", value->getName().c_str(), value_number.c_str());
    if (value_number_map_.find(value_number) != value_number_map_.end()) {
        return value_number_map_[value_number];
    } else {
        value_number_map_[value_number] = value;
    }
    return value;
}

void GlobalValueNumber::replaceValues() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (replace_values_map_.count(inst)) {
                // printf("the inst %s replaced by %s\n", inst->getName().c_str(), dynamic_cast<Instruction *>(replace_values_map_[inst])->getName().c_str());
                // inst->replaceAllUseWith(replace_values_map_[inst]);
                for (auto user: user_analysis_->getUserInsts(inst)) {
                    if (auto user_inst = dynamic_cast<PhiInstruction *>(user); !user_inst) {
                        user->replaceWithValue(inst, replace_values_map_[inst]);
                    }
                }
            }
        }
    }
}

std::string GlobalValueNumber::getOperandNumber(const Value *operand) {
    std::string value_name;
    if (auto const_value = dynamic_cast<const ConstantVar *>(operand); const_value) {
        value_name = "C";
        value_name += const_value->getBasicType() == INT_BTYPE
                      ? std::to_string(const_value->getIValue()) : std::to_string(const_value->getFValue());
    } else if (auto global_value = dynamic_cast<const GlobalVariable *>(operand); global_value) {
        value_name = "G_" + global_value->getName();
    } else if (auto arg_value = dynamic_cast<const Argument *>(operand); arg_value) {
        value_name = "A_" + arg_value->getName();
    } else {
        value_name = "V_" + operand->getName();
    }
    return value_name;
}

std::string GlobalValueNumber::getValueNumber(const Value *value) {
    std::string value_number;
    if (auto inst_value = dynamic_cast<const Instruction *>(value); inst_value) {
        if (auto unary_inst = dynamic_cast<const UnaryOpInstruction *>(inst_value); unary_inst) {
            value_number = "U" + std::to_string(unary_inst->getInstType()) + "_" + unary_inst->getValue()->getName();
        }
        if (auto binary_inst = dynamic_cast<const BinaryOpInstruction *>(inst_value); binary_inst) {
            auto lhs_name = getOperandNumber(binary_inst->getLeft());
            auto rhs_name = getOperandNumber(binary_inst->getRight());

            auto binary_type = binary_inst->getInstType();
            if ((binary_type == AddType || binary_type == SubType || binary_type == OrType) && lhs_name > rhs_name) {
                std::swap(lhs_name, rhs_name);
            }

            value_number = "B_" + std::to_string(binary_inst->getInstType()) + "_" + lhs_name + "_" + rhs_name;
        }
        if (auto gep_inst = dynamic_cast<const GEPInstruction *>(inst_value); gep_inst) {
            auto base_name = gep_inst->getPtr()->getName();
            value_number = "GEP_" + base_name + "_";
            auto index_size = gep_inst->getIndexSize();
            for (int i = 0; i < index_size; ++i) {
                value_number += getOperandNumber(gep_inst->getIndexValue(i));
                if (i != index_size - 1) {
                    value_number += "_";
                }
            }
        }
        // 关于Call类型的指令是需要进行补充的
    }
    auto value_type = value->getValueType();
    if (value_type == ConstantValue || value_type == GlobalVariableValue || value_type == ArgumentValue) {
        value_number = getOperandNumber(value);
    }
    return value_number;
}

void GlobalValueNumber::numberValue(Value *value) {
    auto value_type = value->getValueType();
    if (value_type != InstructionValue && value_type != ArgumentValue && value_type != ConstantValue && value_type != GlobalVariableValue) {
        return;
    }
    auto value_inst = dynamic_cast<Instruction *>(value);
    if (value_inst) {
        // auto value_inst_type = value_inst->getInstType();
        if (!dynamic_cast<UnaryOpInstruction *>(value_inst) && !dynamic_cast<BinaryOpInstruction *>(value_inst) && !dynamic_cast<GEPInstruction *>(value_inst)) {
            return;
        }
    }

    if (visited_set_.count(value)) {
        return;
    }
    visited_set_.insert(value);


    if (value_inst) {
        for (int i = 0; i < value_inst->getOperandNum(); ++i) {
            auto operand = value_inst->getOperand(i);
            numberValue(operand);
        }

        auto value_name = getValueNumber(value_inst);
        auto look_inst = lookupOrAdd(value_name, value_inst);

        if (look_inst != value_inst) {
            replace_values_map_[value_inst] = look_inst;
        }
    }
    // 允许其中的合法
}