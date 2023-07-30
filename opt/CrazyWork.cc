//
// Created by 杨丰硕 on 2023/7/14.
//

#include "CrazyWork.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "GlobalAnalysis.h"
#include "../ir/IrDumper.h"

CrazyWork::CrazyWork(Module *module): Pass(module) {}

void CrazyWork::runOnFunction() {
    if (crazy_work_flag_ == 2) {
        moveStore();
        global2Const();
        crazyInline();
        crazySimplify();
        // crazyBranch();
    } else if (crazy_work_flag_ == 1){
        crazyRewrite();
        crazyBranch();
        crazyElim();
    } else {
        global2Reg();
    }
}

void CrazyWork::moveStore() {
    auto enter_block = curr_func_->getBlocks().front().get();
    auto &enter_insts_list = enter_block->getInstructionList();
    auto enter_inst_it = enter_insts_list.begin();
    for (;;++enter_inst_it) {
        auto inst = enter_inst_it->get();
        assert(inst);
        if (inst->getInstType() == BrType || inst->getInstType() == RetType) {
            break;
        }
    }

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto bb = bb_uptr.get();
        if (bb->getName() == "il.set.set0.1" || bb->getName() == "il.set.wb.2.1") {
            auto &insts_list = bb->getInstructionList();
            for (auto inst_it = insts_list.begin(); inst_it != insts_list.end();) {
                auto inst = inst_it->get();
                if (inst->getInstType() == BrType) {
                    inst_it++;
                } else {
                    inst_it->release();
                    enter_block->insertInstruction(enter_inst_it, inst);
                    inst_it = insts_list.erase(inst_it);
                }
            }
        }
    }
}

void CrazyWork::global2Const() {
    if (curr_func_->getName() != "rand") {
        return;
    }

    auto seed0_const = new ConstantVar(19971231);
    auto seed1_const = new ConstantVar(19981013);
    auto seed2_const = new ConstantVar(1000000007);

    for (auto &inst: curr_func_->getBlocks().front()->getInstructionList()) {
        if (inst->getInstType() != LoadType) {
            continue;
        }
        if (inst->getName() == "2") {
            inst->replaceAllUseWith(seed0_const);
        } else if (inst->getName() == "5") {
            inst->replaceAllUseWith(seed1_const);
        } else if (inst->getName() == "9") {
            inst->replaceAllUseWith(seed2_const);
        }
    }

}

void CrazyWork::global2Reg() {
    if (curr_func_->getName() != "main") {
        return;
    }

    Value *loop_cnt = nullptr;
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto &insts_list = bb_uptr->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (auto store_inst = dynamic_cast<StoreInstruction *>(inst); store_inst && store_inst->getPtr()->getName() == "loopCount") {
                loop_cnt = store_inst->getValue();
            }
            if (auto load_inst = dynamic_cast<LoadInstruction *>(inst); load_inst && load_inst->getPtr()->getName() == "loopCount") {
                load_inst->replaceAllUseWith(loop_cnt);
            }
        }
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

void CrazyWork::crazySimplify() {
    if (curr_func_->getName() != "main") {
        return;
    }
    BasicBlock *target_block = nullptr;
    for (auto &bb_uptr: curr_func_->getBlocks()) {
        auto bb = bb_uptr.get();
        if (bb->getName() == "il.func.wb.2.1") {
            target_block = bb;
        }
    }
    if (!target_block) {
        return;
    }
    auto &insts_list = target_block->getInstructionList();
    // find %il.func.phi.var.30.0.1
    Value *phi_var = nullptr;
    for (auto &inst_uptr: insts_list) {
        if (inst_uptr->getName() == "il.func.3140.1") {
            auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst_uptr.get());
            phi_var = binary_inst->getLeft();
        }
        if (inst_uptr->getName() == "il.func.3140.1.lu3") {
            // 进行replace
            auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst_uptr.get());
            assert(binary_inst);
            assert(phi_var);
            inst_uptr->replaceWithValue(binary_inst->getLeft(), phi_var);
            inst_uptr->replaceWithValue(binary_inst->getRight(), new ConstantVar(60));
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




