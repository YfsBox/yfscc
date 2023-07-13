//
// Created by 杨丰硕 on 2023/7/13.
//
#include "GlobalAnalysis.h"
#include "SplitGEPInsts.h"
#include "../ir/Instruction.h"
#include "../ir/IrDumper.h"

SplitGEPInsts::SplitGEPInsts(Module *module): Pass(module) {}

void SplitGEPInsts::findAndSplit() {
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto bb = bb_uptr.get();
        auto &insts_list = bb->getInstructionList();
        for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
            if (auto gep_inst = dynamic_cast<GEPInstruction *>(inst_it->get()); gep_inst && gep_inst->getArrayDimension().size() + 1 == gep_inst->getIndexSize()) {
            // 多维的形式分解成
                auto array_dim_array = gep_inst->getArrayDimension();
                auto last_base = gep_inst->getPtr();
                auto dim_array_size = array_dim_array.size();
                for (int i = 0; i <= dim_array_size; ++i) {
                    auto split_gep_name = gep_inst->getName() + ".sp" + std::to_string(i);
                    auto array_dim = array_dim_array[i];
                    // 获取对应纬度的变量
                    auto dim_value = gep_inst->getIndexValue(i);
                    // 获取表示对应维度的数组
                    std::vector<int32_t> dim_array = std::vector(array_dim_array.begin() + i, array_dim_array.end());
                    // 生成新的GEP指令
                    auto new_gep_inst = new GEPInstruction(gep_inst->getParent(), gep_inst->getBasicType(), {last_base, dim_value}, dim_array,  i == dim_array_size - 1, split_gep_name);
                    // 加入到集合中
                    split_insts_map_[gep_inst].push_back(new_gep_inst);

                    last_base = new_gep_inst;
                }

                /*ir_dumper_->dump(gep_inst);
                printf("be split as here\n");
                for (int i = 0; i < split_insts_map_[gep_inst].size(); ++i) {
                    ir_dumper_->dump(split_insts_map_[gep_inst][i]);
                }*/

                insert_point_map_[gep_inst] = inst_it;
            }
        }
    }
}

void SplitGEPInsts::insertAndReplace() {
    UserAnalysis user_analysis;
    user_analysis.analysis(curr_func_);

    for (auto &[inst, insert_point]: insert_point_map_) {
        auto parent_bb = inst->getParent();
        for (auto split_inst: split_insts_map_[inst]) {
            parent_bb->insertInstruction(insert_point, split_inst);
        }
        auto replace_inst = split_insts_map_[inst].back();

        for (auto user_inst: user_analysis.getUserInsts(inst)) {
            user_inst->replaceWithValue(inst, replace_inst);
        }
    }

}

void SplitGEPInsts::runOnFunction() {
    insert_point_map_.clear();
    split_insts_map_.clear();

    findAndSplit();
    insertAndReplace();
}

