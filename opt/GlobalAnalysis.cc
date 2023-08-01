//
// Created by 杨丰硕 on 2023/6/15.
//

#include "GlobalAnalysis.h"
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"
#include "../ir/Instruction.h"
#include "../ir/BasicBlock.h"

GlobalAnalysis::GlobalAnalysis(Module *module): Pass(module), has_init_(false) {}

void GlobalAnalysis::initWorkList() {
    for (int i = 0; i < module_->getGlobalSize(); ++i) {
        auto global = module_->getGlobalVariable(i);
        if (!global->isConst() && !global->isArray()) {
            work_global_list_.insert(global);
        }
    }
}

void GlobalAnalysis::findGlobalOwnerByOneFunc() {
    for (auto global : work_global_list_) {
        std::unordered_set<Function *> use_func_set;
        auto user_map = global->getUserMap();
        for (auto user: user_map) {
            if (auto user_inst = dynamic_cast<Instruction *>(user); user_inst) {
                use_func_set.insert(user_inst->getParent()->getFunction());
            }
        }

        if (use_func_set.size() == 1) {
            auto func = *use_func_set.begin();
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