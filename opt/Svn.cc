//
// Created by 杨丰硕 on 2023/6/15.
//

#include "Svn.h"
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

        valuename = std::to_string(binary_inst->getInstType()) + "_" + lhs_name + "_" + rhs_name;
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
    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
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
        if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
            auto value_name = getValueName(inst);
            printf("the value number is %s for inst %s in bb %s\n", value_name.c_str(), inst->getName().c_str(),
                   basicblock->getName().c_str());
            auto lookup_inst = lookupOrAdd(value_name, inst);
            if (lookup_inst != inst) {
                replaced_map_[inst] = lookup_inst;
            }
        }
    }
}


void Svn::replaceValues() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (replaced_map_.count(inst)) {
                ir_dumper_->dump(inst);
                printf("relpace with\n");
                ir_dumper_->dump(replaced_map_[inst]);
                inst->replaceAllUseWith(replaced_map_[inst]);
            }
        }
    }
}

void Svn::runOnFunction() {
    visited_bbs_.clear();
    replaced_map_.clear();
    auto enter_block = curr_func_->getBlocks().begin()->get();
    work_list_.push_back(enter_block);

    while (!work_list_.empty()) {
        auto bb = work_list_.front();
        work_list_.pop_front();
        svn(bb);
    }

    replaceValues();
}