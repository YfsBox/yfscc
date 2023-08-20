//
// Created by 杨丰硕 on 2023/6/15.
//

#include "GlobalAnalysis.h"
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"
#include "../ir/Instruction.h"
#include "../ir/BasicBlock.h"

GlobalAnalysis::GlobalAnalysis(Module *module): Pass(module), has_init_(false) {
    pass_name_ = "GlobalValueAnalysis";
}

void GlobalAnalysis::initWorkList() {
    for (int i = 0; i < module_->getGlobalSize(); ++i) {
        auto global = module_->getGlobalVariable(i);
        if (!global->isConst() && !global->isArray()) {
            work_global_list_.insert(global);
        }
    }
}

void GlobalAnalysis::findGlobalOwnerByOneFunc() {
    std::unordered_map<GlobalVariable *, std::unordered_set<Function *> > use_function_map;
    for (int i = 0; i < module_->getFuncSize(); ++i) {
        auto function = module_->getFunction(i);
        if (function->isDead()) {
            continue;
        }
        UserAnalysis user_analysis;
        user_analysis.analysis(function);
        for (auto global: work_global_list_) {
            if (!user_analysis.getUserInsts(global).empty()) {
                use_function_map[global].insert(function);
            }
        }
    }

    for (auto global: work_global_list_) {
        if (use_function_map[global].size() == 1) {
            auto func = *use_function_map[global].begin();
            if (func->getName() == "main") {
                function_global_map_[func].insert(global);
            }
        }
    }
}

void GlobalAnalysis::runOnFunction() {
    if (!has_init_) {
        initWorkList();
        findGlobalOwnerByOneFunc();
        has_init_ = true;
    }
    // 首先创建alloca以及相应的init指令
    auto enter_block = curr_func_->getBlocks().front().get();
    std::vector<Instruction *> alloca_and_store_insts;
    std::unordered_map<GlobalVariable *, AllocaInstruction *> global2alloca_map;
    for (auto global: function_global_map_[curr_func_]) {
        auto basic_type = global->getBasicType();
        auto global_name = global->getName();
        auto alloca_inst = new AllocaInstruction(enter_block, basic_type, false, global_name);
        auto store_inst = new StoreInstruction(enter_block, basic_type, global->getConstInit(), alloca_inst, global_name);

        alloca_and_store_insts.push_back(alloca_inst);
        alloca_and_store_insts.push_back(store_inst);
        global2alloca_map[global] = alloca_inst;
    }

    for (auto rit = alloca_and_store_insts.rbegin(); rit != alloca_and_store_insts.rend(); ++rit) {
        enter_block->addFrontInstruction(*rit);
    }

    // 首先收集所有load目标全局遍历的指令
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            if (auto load_inst = dynamic_cast<LoadInstruction *>(inst_uptr.get()); load_inst) {
                auto load_addr = load_inst->getPtr();
                if (auto global_addr = dynamic_cast<GlobalVariable *>(load_addr); global_addr && function_global_map_[curr_func_].count(global_addr)) {
                    auto global_alloca = global2alloca_map[global_addr];
                    load_inst->replaceWithValue(global_addr, global_alloca);
                }
            }

            if (auto store_inst = dynamic_cast<StoreInstruction *>(inst_uptr.get()); store_inst) {
                auto store_addr = store_inst->getPtr();
                if (auto global_addr = dynamic_cast<GlobalVariable *>(store_addr); global_addr && function_global_map_[curr_func_].count(global_addr)) {
                    auto global_alloca = global2alloca_map[global_addr];
                    store_inst->replaceWithValue(global_addr, global_alloca);
                }
            }
        }
    }
}

UserAnalysis::UserInsts &UserAnalysis::getUserInsts(Value *value) {
    return user_map_[value];
}

void UserAnalysis::analysis(Function *function) {
    curr_func_ = function;
    user_map_.clear();
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            for (int i = 0; i < inst->getOperandNum(); ++i) {
                auto operand = inst->getOperand(i);
                if (operand->getValueType() ==  InstructionValue || operand->getValueType() == ArgumentValue || operand->getValueType() == GlobalVariableValue) {
                    user_map_[operand].insert(inst);
                }
            }
        }
    }
}

void UserAnalysis::replaceAllUseWith(Value *value, Value *replaced_value) {
    for (auto user: getUserInsts(value)) {
        user->replaceWithValue(value, replaced_value);
    }
}

ArrayAnalysis::ArrayAnalysis(): curr_func_(nullptr) {}


void ArrayAnalysis::analysis(Function *function) {
    curr_func_ = function;

    std::unordered_set<Value *> array_values;
    for (auto global: function->getParent()->getGlobalSet()) {
        if (global->isArray()) {
            array_values.insert(const_cast<GlobalVariable *>(global));
        }
    }

    auto enter_block = function->getBlocks().begin()->get();
    for (auto &inst_uptr: enter_block->getInstructionList()) {
        if (auto alloca_inst = dynamic_cast<AllocaInstruction *>(inst_uptr.get()); alloca_inst && alloca_inst->isArray()) {
            array_values.insert(inst_uptr.get());
        }
    }

    for (auto array_value: array_values) {
        array_info_map_[array_value] = std::make_unique<ArrayInfo>();
        array_info_map_[array_value]->array_base_ = array_value;
    }

    // 收集其中的gep指令，需要都是const的index才可以
    UserAnalysis user_analysis;
    user_analysis.analysis(curr_func_);

    for (auto array_value: array_values) {
        for (auto user: user_analysis.getUserInsts(array_value)) {
            if (auto gep_inst = dynamic_cast<GEPInstruction *>(user); gep_inst) {
                // 如果都是const的index
                bool has_not_const = false;
                for (int i = 0; i < gep_inst->getIndexSize(); ++i) {
                    auto index_value = gep_inst->getIndexValue(i);
                    if (auto const_var = dynamic_cast<ConstantVar *>(index_value); !const_var) {
                        has_not_const = true;
                    }
                }
                if (!has_not_const) {
                    array_info_map_[array_value]->const_index_gep_insts_.insert(gep_inst);
                    gep_owner_array_map_[gep_inst] = array_value;
                }

                // 收集所有个该数组所关联的GEP指令有关的store以及load语句
                auto gep_inst_users = user_analysis.getUserInsts(gep_inst);


                for (auto gep_user: user_analysis.getUserInsts(gep_inst)) {
                    if (auto store_inst = dynamic_cast<StoreInstruction *>(gep_user); store_inst) {
                        array_info_map_[array_value]->store_insts_.push_back(store_inst);
                    }
                    if (auto load_inst = dynamic_cast<LoadInstruction *>(gep_user); load_inst) {
                        array_info_map_[array_value]->load_insts_.insert(load_inst);
                    }
                }
            }
        }
    }
    // 收集相关的memset指令
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        for (auto &inst_uptr: bb_uptr->getInstructionList()) {
            auto inst = inst_uptr.get();
            if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst && call_inst->getFunction()->getName() == "memset") {
                auto memset_base_ptr = call_inst->getActual(0);
                if (gep_owner_array_map_.count(memset_base_ptr)) {
                    auto base_ptr = gep_owner_array_map_[memset_base_ptr];
                    array_info_map_[base_ptr]->memset_call_insts_.insert(call_inst);
                }
            }
        }
    }

}