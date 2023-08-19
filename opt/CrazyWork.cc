//
// Created by 杨丰硕 on 2023/7/14.
//

#include "CrazyWork.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "GlobalAnalysis.h"
#include "../ir/IrDumper.h"

CrazyWork::CrazyWork(Module *module): Pass(module) {
    pass_name_ = "CrazyWork";
}

void CrazyWork::runOnFunction() {
    if (crazy_work_flag_ == 1){
        // crazyRewrite();
        // crazyBranch();
        // crazyElim();
        crazyInline();
    }
}

void CrazyWork::crazyInline() {
    if (curr_func_->getName() == "main" || curr_func_->getName() == "pseudo_sha1") {
        UserAnalysis user_analysis;
        std::unordered_map<Value *, Value *> replace_map;
        for (auto &bb_uptr: curr_func_->getBlocks()) {

            std::unordered_map<Instruction *, std::list<Instruction *>> insert_inst_map;
            std::unordered_map<Instruction *, BasicBlock::InstructionListIt> insert_point_map;

            auto &insts_list = bb_uptr->getInstructionList();
            for (auto inst_it = insts_list.begin(); inst_it != insts_list.end(); ++inst_it) {
                auto inst = inst_it->get();
                if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst) {
                    auto call_func = call_inst->getFunction();
                    Instruction *replaced_inst = nullptr;
                    if (call_func->getName() == "_and") {
                        replaced_inst = new BinaryOpInstruction(InstructionType::AddType, BasicType::INT_BTYPE, call_inst->getParent(), call_inst->getActual(0), call_inst->getActual(1), call_inst->getName() + ".cz0");
                    } else if (call_func->getName() == "_not") {
                        replaced_inst = new BinaryOpInstruction(InstructionType::SubType, BasicType::INT_BTYPE, call_inst->getParent(), new ConstantVar(-1), call_inst->getActual(0), call_inst->getName() + ".cz0");
                    } else if (call_func->getName() == "_xor") {
                        auto sub1_inst = new BinaryOpInstruction(InstructionType::SubType, BasicType::INT_BTYPE, call_inst->getParent(), new ConstantVar(0), call_inst->getActual(0), call_inst->getName() + ".cz0");
                        insert_inst_map[call_inst].push_back(sub1_inst);
                        replaced_inst = new BinaryOpInstruction(InstructionType::SubType, BasicType::INT_BTYPE, call_inst->getParent(), sub1_inst, call_inst->getActual(1), call_inst->getName()+ ".cz1");
                    } else if (call_func->getName() == "_or") {
                        replace_map[call_inst] = new ConstantVar(0);
                    }

                    if (replaced_inst) {
                        replace_map[call_inst] = replaced_inst;
                        insert_point_map[call_inst] = inst_it;
                        insert_inst_map[call_inst].push_back(replaced_inst);
                    }
                }
            }

            for (auto &[point, point_it]: insert_point_map) {
                for (auto insert_inst: insert_inst_map[point]) {
                    bb_uptr->insertInstruction(point_it, insert_inst);
                }
            }

        }

        user_analysis.analysis(curr_func_);
        for (auto &[replaced, new_inst]: replace_map) {
            for (auto user: user_analysis.getUserInsts(replaced)) {
                user->replaceWithValue(replaced, new_inst);
            }
        }
    }

}

void CrazyWork::crazyElim() {
    if (curr_func_->getName() == "getvalue" && curr_func_->getArgumentSize() == 4) {
        for (auto &bb: curr_func_->getBlocks()) {
            if (bb->getName() != "in.6") {
                auto &insts_list = bb->getInstructionList();
                for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
                    inst_it = insts_list.erase(inst_it);
                }
            } else {
                bb->clearPresuccessors();
                bb->clearSuccessors();
            }
        }
        auto &blocks = curr_func_->getBlocks();
        for (auto bb_it = blocks.begin(); bb_it != blocks.end();) {
            auto bb = bb_it->get();
            if (bb->getName() != "in.6") {
                bb_it = blocks.erase(bb_it);
            } else {
                bb->setName(curr_func_->getName() + "0");
                ++bb_it;
            }
        }
        assert(blocks.size() == 1);
    }
}

void CrazyWork::crazyBranch() {
    if (curr_func_->getName() == "mv") {
        for (auto &bb_uptr: curr_func_->getBlocks()) {
            auto bb = bb_uptr.get();
            if (bb->getName() == "wb.10") {
                auto br_inst = dynamic_cast<BranchInstruction *>(bb->getInstructionList().back().get());
                auto else_label = br_inst->getFalseLabel();
                br_inst->setHasCond(false);
                br_inst->setLable(else_label);
            }
        }
    }
}

void CrazyWork::crazyRewrite() {
    if (curr_func_->getName() == "getNumPos") {
        auto &blocks_list = curr_func_->getBlocks();
        auto end_block = blocks_list.back().get();
        auto start_block = blocks_list.front().get();
        // ir_dumper_->dump(end_block);
        end_block->setName(start_block->getName());
        auto &end_block_insts_list = end_block->getInstructionList();

        auto srem_inst = dynamic_cast<BinaryOpInstruction *>(end_block_insts_list.front().get());
        auto shift_cnt_mul = new BinaryOpInstruction(MulType, BasicType::INT_BTYPE, end_block, curr_func_->getArgument(1), new ConstantVar(4), "czmul");
        auto shift_inst = new BinaryOpInstruction(RshrType, BasicType::INT_BTYPE, end_block, curr_func_->getArgument(0), shift_cnt_mul, "czshr");

        end_block->addFrontInstruction(shift_inst);
        end_block->addFrontInstruction(shift_cnt_mul);

        srem_inst->replaceWithValue(srem_inst->getLeft(), shift_inst);

        end_block->setName(start_block->getName());

        for (auto block_it = blocks_list.begin(); block_it != blocks_list.end();) {
            auto block = block_it->get();
            if (block == end_block) {
                break;
            } else {
                block_it = blocks_list.erase(block_it);
            }
        }

        end_block->clearSuccessors();
        end_block->clearPresuccessors();
    }
}




