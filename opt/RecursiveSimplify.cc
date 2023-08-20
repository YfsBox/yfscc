//
// Created by 杨丰硕 on 2023/8/21.
//

#include "RecursiveSimplify.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"
#include "../ir/GlobalVariable.h"
#include "../ir/IrDumper.h"

static BasicType getBasicType(Value *value) {
    if (auto arg = dynamic_cast<Argument *>(value); arg) {
        return arg->getBasicType();
    } else if (auto inst = dynamic_cast<Instruction *>(value); inst) {
        return inst->getBasicType();
    } else if (auto global = dynamic_cast<GlobalVariable *>(value); global) {
        return global->getBasicType();
    }
    return UNVALID_BTYPE;
}

RecursiveSimplify::RecursiveSimplify(Module *module): Pass(module) {}

bool RecursiveSimplify::isRecursiveCurrFunction(CallInstruction *inst) const {
    return inst->getFunction() == curr_func_;
}

int RecursiveSimplify::getOnlyDiffArgumentIndex(CallInstruction *call_inst1, CallInstruction *call_inst2) const {
    std::vector<int> diff_indexs;
    for (int i = 0; i < call_inst1->getActualSize(); ++i) {
        auto arg1 = call_inst1->getActual(i);
        auto arg2 = call_inst2->getActual(i);

        if (arg1 != arg2) {
            diff_indexs.push_back(i);
        }
    }

    if (diff_indexs.size() == 1) {          // 唯一不同的index
        return diff_indexs[0];
    }
    return -1;      // 不合法的情况，也就是不只是一个不同的情况，这种情况返回-1
}

void RecursiveSimplify::runOnFunction() {
    if (curr_func_->getName() == "main") {
        return;
    }

    Instruction *last_inst = nullptr, *last_last_inst = nullptr;

    SetCondInstruction *cmp_inst = nullptr;
    BranchInstruction *branch_inst = nullptr;
    BasicBlock *then_block = new BasicBlock(curr_func_, "rs.then");
    BasicBlock *else_block = new BasicBlock(curr_func_, "rs.else");
    Value *diff_arg1 = nullptr, *diff_arg2 = nullptr;

    bool can_break = false;

    for (auto &bb_uptr: curr_func_->getBlocks()) {
        if (!bb_uptr->getSuccessorBlocks().empty()) {       // 表示必定是含ret指令的
            continue;
        }
        auto &insts_list = bb_uptr->getInstructionList();
        auto insert_it = insts_list.begin();
        auto inst_it = insts_list.begin();

        for (; inst_it != insts_list.end(); ++inst_it) {
            if (last_last_inst && last_inst) {
                auto last_call_inst = dynamic_cast<CallInstruction *>(last_last_inst);
                auto curr_call_inst = dynamic_cast<CallInstruction *>(inst_it->get());

                if (last_call_inst && curr_call_inst && isRecursiveCurrFunction(last_call_inst) &&
                        isRecursiveCurrFunction(curr_call_inst)) {
                    int diff_index = getOnlyDiffArgumentIndex(last_call_inst, curr_call_inst);
                    if (diff_index >= 0) {
                        diff_arg1 = last_call_inst->getActual(diff_index);
                        diff_arg2 = curr_call_inst->getActual(diff_index);
                        // 增加一个判断是否相等的icmp指令
                        insert_it = inst_it;
                        cmp_inst = new SetCondInstruction(bb_uptr.get(), SetCondInstruction::SetNE, getBasicType(diff_arg1) == FLOAT_BTYPE, diff_arg1, diff_arg2, "rs.cmp" + diff_arg1->getName() + "." + diff_arg2->getName());
                        // 一个条件跳转语句
                        branch_inst = new BranchInstruction(bb_uptr.get(), cmp_inst, then_block, else_block);

                        can_break = true;
                        break;
                    }
                }
            }
            last_last_inst = last_inst;
            last_inst = inst_it->get();
        }
        if (!can_break) {
            continue;
        }

        // 退出来之后，知道末尾ret的部分，都是加入到then里面
        std::list<Instruction *> insts_move_then;
        auto tmp_inst_it = inst_it;
        while (inst_it != insts_list.end()) {
            auto inst = inst_it->get();
            insts_move_then.push_back(inst);
            inst_it++;
        }

        if (insts_move_then.size() > 3) {
            break;
        }
        // printf("begin remove from old block %s\n", bb_uptr->getName().c_str());
        while (tmp_inst_it != insts_list.end()) {           // 从原本的位置移除
            tmp_inst_it->release();
            tmp_inst_it = insts_list.erase(tmp_inst_it);
        }

        for (auto then_inst: insts_move_then) {
            then_block->addInstruction(then_inst);
        }

        bb_uptr->addInstruction(cmp_inst);          // 在当前块的末尾
        bb_uptr->addInstruction(branch_inst);
        bb_uptr->setBranchInst(branch_inst);
        bb_uptr->setHasJump(true);
        bb_uptr->setHasRet(false);

        // 之后再往else中增加新的指令
        auto ret_value_inst = new BinaryOpInstruction(SubType, getBasicType(diff_arg1), else_block, diff_arg2, last_last_inst);
        else_block->addInstruction(ret_value_inst);
        auto ret_inst = new RetInstruction(else_block, ret_value_inst->getBasicType(), ret_value_inst);
        else_block->addInstruction(ret_inst);

        break;
    }

    if (can_break) {

        curr_func_->addBasicBlock(then_block);
        curr_func_->addBasicBlock(else_block);

        curr_func_->rebuildCfg();
    }



}