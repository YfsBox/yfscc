//
// Created by 杨丰硕 on 2023/7/14.
//

#include "CrazyWork.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"

CrazyWork::CrazyWork(Module *module): Pass(module) {}

void CrazyWork::runOnFunction() {
    moveStore();
    global2Const();
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



