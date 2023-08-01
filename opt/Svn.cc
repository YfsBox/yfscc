//
// Created by 杨丰硕 on 2023/6/15.
//

#include "Svn.h"
#include "CallGraphAnalysis.h"
#include "GlobalAnalysis.h"
#include "../ir/BasicBlock.h"
#include "../ir/Instruction.h"
#include "../ir/Constant.h"
#include "../ir/IrDumper.h"

Svn::ValueName Svn::getValueName(Instruction *inst) {
    ValueName valuename;
    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
        auto lhs_name = getOperandName(binary_inst->getLeft());
        auto rhs_name = getOperandName(binary_inst->getRight());

        auto binary_type = inst->getInstType();
        if ((binary_type == AddType || binary_type == SubType || binary_type == OrType) && lhs_name > rhs_name) {
            std::swap(lhs_name, rhs_name);
        }

        valuename = "B_" + std::to_string(binary_inst->getInstType()) + "_" + lhs_name + "_" + rhs_name;
    } else if (auto gep_inst = dynamic_cast<GEPInstruction *>(inst); gep_inst) {
        auto base_name = gep_inst->getPtr()->getName();
        valuename = "GEP_" + base_name + "_";
        auto index_size = gep_inst->getIndexSize();
        for (int i = 0; i < index_size; ++i) {
            valuename += getOperandName(gep_inst->getIndexValue(i));
            if (i != index_size - 1) {
                valuename += "_";
            }
        }
    } else if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        auto value_name = getOperandName(unary_inst->getValue());
        auto unary_type = inst->getInstType();
        valuename = "U_" + std::to_string(unary_type) + "_" + value_name;
    } else if (auto call_inst_value = dynamic_cast<const CallInstruction *>(inst); call_inst_value && !callgraph_analysis_->hasSideEffect(call_inst_value->getFunction())) {
        valuename = "CALL_" + call_inst_value->getFunction()->getName();
        for (int i = 0; i < call_inst_value->getActualSize(); ++i) {
            auto actual = call_inst_value->getActual(i);
            valuename += "_" + actual->getName();
        }
    } else if (auto load_inst_value = dynamic_cast<const LoadInstruction *>(inst); load_inst_value) {
        valuename = "LD_" + load_inst_value->getPtr()->getName();
    }
    return valuename;
}

Svn::ValueName Svn::getOperandName(Value *value) {
    std::string value_name;
    if (auto const_value = dynamic_cast<ConstantVar *>(value); const_value) {
       value_name = "c";
       value_name += const_value->getBasicType() == INT_BTYPE
               ? std::to_string(const_value->getIValue()) : std::to_string(const_value->getFValue());
    } else {
       value_name = value->getName();
    }
    return value_name;
}

Instruction* Svn::lookupOrAdd(const ValueName &value_name, Instruction *inst) {
    if (inst->getInstType() == GEPType || dynamic_cast<BinaryOpInstruction *>(inst) || dynamic_cast<UnaryOpInstruction *>(inst) || dynamic_cast<CallInstruction *>(inst) || dynamic_cast<LoadInstruction *>(inst)) {
        auto &curr_table = binary_value_name_table_.back();
        if (curr_table.count(value_name)) {
            return curr_table[value_name];
        }
        curr_table[value_name] = inst;
    }
    return inst;
}

void Svn::svn(BasicBlock *basicblock) {
    visited_bbs_.insert(basicblock);
    // replaced_map_.clear();
    allocateTable();
    lvn(basicblock);
    replaceValues();

    for (auto succ_bb: basicblock->getSuccessorBlocks()) {
        if (succ_bb->getPreDecessorBlocks().size() == 1) {
            svn(succ_bb);
        } else if (!visited_bbs_.count(succ_bb)) {
            visited_bbs_.insert(succ_bb);
            work_list_.push_back(succ_bb);
        }
    }
    deallocateTable();
}

void Svn::lvn(BasicBlock *basicblock) {
    auto &insts_list = basicblock->getInstructionList();
    for (auto &inst_uptr: insts_list) {
        auto inst = inst_uptr.get();
        bool can_number_call = false;
        if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst) {
            can_number_call = !callgraph_analysis_->hasSideEffect(call_inst->getFunction());
        }
        bool load_ptr = false;
        if (auto load_inst = dynamic_cast<LoadInstruction *>(inst); load_inst) {
            load_ptr = load_inst->isFromSecondaryPtr();
        }
        if (dynamic_cast<BinaryOpInstruction *>(inst) || dynamic_cast<GEPInstruction *>(inst) || dynamic_cast<UnaryOpInstruction *>(inst) || can_number_call || load_ptr) {
            auto value_name = getValueName(inst);
            // printf("the value name is %s\n", value_name.c_str());
            auto lookup_inst = lookupOrAdd(value_name, inst);
            if (lookup_inst != inst) {
                replaced_map_[inst] = lookup_inst;
            }
        }
    }
}


void Svn::replaceValues() {
    UserAnalysis user_analysis;
    user_analysis.analysis(curr_func_);
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (replaced_map_.count(inst)) {
                user_analysis.replaceAllUseWith(inst, replaced_map_[inst]);
            }
        }
    }
}

void Svn::runOnFunction() {
    visited_bbs_.clear();
    replaced_map_.clear();
    callgraph_analysis_->analysis();
    auto enter_block = curr_func_->getBlocks().begin()->get();
    work_list_.push_back(enter_block);

    while (!work_list_.empty()) {
        auto bb = work_list_.front();
        work_list_.pop_front();
        svn(bb);
    }

    replaceValues();
}