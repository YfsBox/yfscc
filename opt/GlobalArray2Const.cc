//
// Created by 杨丰硕 on 2023/8/17.
//
#include "GlobalAnalysis.h"
#include "GlobalArray2Const.h"
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"
#include <unordered_map>

GlobalArray2Const::GlobalArray2Const(Module *module): Pass(module) {}

void GlobalArray2Const::runOnFunction() {
    // 判断当前是否只剩了main函数，比较简单粗暴
    if (curr_func_->getParent()->getNotDeadFunctionCnt() == 1) {
        ArrayAnalysis array_analysis;
        array_analysis.analysis(curr_func_);

        // 1.首先判断对于global的数组，有没有相关的store指令
        std::unordered_set<GlobalVariable *> can_replace_globals;
        for (auto global: curr_func_->getParent()->getGlobalSet()) {
            auto global_array_info = array_analysis.getArrayInfo(const_cast<GlobalVariable *>(global));
            if (global_array_info && global_array_info->store_insts_.size() == 0) {
                can_replace_globals.insert(const_cast<GlobalVariable *>(global));
            }
        }
        // 2. 在没有store指令的情况下，就需要寻找从const index gep的load指令，之后采用值替换
        std::unordered_map<Value *, Value *> replace_map;
        for (auto global: can_replace_globals) {
            auto global_array_info = array_analysis.getArrayInfo(global);
            // 分析其中相关联的load指令，如果这个load指令来自一个index都是const的，就可以进行替换
            for (auto load_inst: global_array_info->load_insts_) {
                auto load_from_ptr = load_inst->getPtr();
                if (auto gep_inst_ptr = dynamic_cast<GEPInstruction *>(load_from_ptr); gep_inst_ptr && global_array_info->const_index_gep_insts_.count(gep_inst_ptr)) {
                    // 根据其中index值定位到具体的数组值
                    auto global_init_array = dynamic_cast<ConstantArray *>(global->getConstInit());
                    // 这里就只处理一维数组
                    if (gep_inst_ptr->getIndexSize() == 2) {
                        auto index1 = dynamic_cast<ConstantVar *>(gep_inst_ptr->getIndexValue(0))->getIValue();
                        assert(index1 == 0);
                        auto index2 = dynamic_cast<ConstantVar *>(gep_inst_ptr->getIndexValue(1))->getIValue();
                        assert(global_init_array->getInitValueMap().count(index2));
                        replace_map[load_inst] = global_init_array->getInitValueMap().at(index2).get();
                    }
                }
            }
        }
        // 3. 替换其中的load指令为常量
        UserAnalysis user_analysis;
        user_analysis.analysis(curr_func_);

        for (auto &[load_inst, const_value]: replace_map) {
            for (auto user: user_analysis.getUserInsts(load_inst)) {
                user->replaceWithValue(load_inst, const_value);
            }
        }

    }
}

