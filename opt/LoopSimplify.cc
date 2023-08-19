//
// Created by 杨丰硕 on 2023/8/18.
//
#include <cassert>
#include "LoopSimplify.h"
#include "ComputeLoops.h"
#include "GlobalAnalysis.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"

LoopSimplify::LoopSimplify(Module *module): Pass(module), compute_loops_(nullptr), user_analysis_(std::make_unique<UserAnalysis>()) {}

std::list<Instruction *> LoopSimplify::isSimpleLoop(const std::shared_ptr<ComputeLoops::LoopInfo> &loopinfo) {
    // body基本块数量只有一个，并且指令数量为3，包含一个循环变量a的迭代表达式，一个累加变量b的表达式
    if (loopinfo->loop_body_.size() != 1) {
        return {};
    }
    auto body_block = *loopinfo->loop_body_.begin();
    if (body_block->getInstructionList().size() != 3 || loopinfo->iterator_var_phi_insts_.size() != 2) {
        return {};
    }
    // printf("body insts size and iterator value ok\n");
    // 在有两个的情况下，如果一个是index的迭代变量，一个累加的变量
    // 从enter中找到cmp inst，之后得出index value
    PhiInstruction *index_value = nullptr;
    BranchInstruction *br_inst = nullptr;
    for (auto &inst_uptr: loopinfo->enter_block_->getInstructionList()) {
        if (auto setcond_inst = dynamic_cast<SetCondInstruction *>(inst_uptr.get()); setcond_inst) {
            if (dynamic_cast<PhiInstruction *>(setcond_inst->getLeft())) {
                index_value = dynamic_cast<PhiInstruction *>(setcond_inst->getLeft());
            } else {
                index_value = dynamic_cast<PhiInstruction *>(setcond_inst->getRight());
            }
        }
        if (auto tmp_br_inst = dynamic_cast<BranchInstruction *>(inst_uptr.get()); tmp_br_inst) {
            br_inst = tmp_br_inst;
        }
    }
    if (index_value == nullptr) {
        return {};
    }
    // printf("index value ok\n");
    // 计算出来循环的次数, 应该确保是可以用单个变量表示出来的
    auto init_index_value = loopinfo->init_var_phi_insts_[index_value];
    if (auto init_value = dynamic_cast<ConstantVar *>(init_index_value); !init_value || init_value->isFloat() || init_value->getIValue() != 0) {
        return {};
    }
    // printf("init value ok\n");
    Value *index_limit_value = nullptr;
    SetCondInstruction *cmp_cond_inst = nullptr;
    BinaryOpInstruction *add_inst = nullptr;
    assert(user_analysis_->getUserInsts(index_value).size() == 2);
    for (auto user: user_analysis_->getUserInsts(index_value)) {
        if (user->getInstType() == SetCondType) {
            cmp_cond_inst = dynamic_cast<SetCondInstruction *>(user);
        } else {
            add_inst = dynamic_cast<BinaryOpInstruction *>(user);
        }
    }
    // 符合条件的情况应该包含3个，一个对应
    assert(cmp_cond_inst);
    assert(add_inst);

    if (cmp_cond_inst->getLeft() == index_value) {
        index_limit_value = cmp_cond_inst->getRight();
    } else {
        index_limit_value = cmp_cond_inst->getLeft();
    } // 至此已经得到了循环的次数为index_limit_value

    ConstantVar *add_const_step = nullptr;      // 检查迭代表达式是否合法，在这里设置+1的条件限制
    if (add_inst->getLeft() == index_value) {
        add_const_step = dynamic_cast<ConstantVar *>(add_inst->getRight());
    } else {
        add_const_step = dynamic_cast<ConstantVar *>(add_inst->getLeft());
    }
    if (!add_const_step || add_const_step->isFloat() || add_const_step->getIValue() != 1) {
        return {};
    }

    // 之后分析另一个phi变量的情况
    PhiInstruction *another_introduction_value = nullptr;
    for (auto &[phi_value, init_value]: loopinfo->init_var_phi_insts_) {
        if (phi_value != index_value) {
            another_introduction_value = phi_value;
            break;
        }
    }
    assert(another_introduction_value);
    Instruction *iterate_inst = nullptr;
    auto another_user_insts = user_analysis_->getUserInsts(another_introduction_value);
    for (auto inst: another_user_insts) {
        if (inst->getParent() == body_block) {
            iterate_inst = inst;
        }
    }
    assert(iterate_inst);
    if (auto binary_iterate_inst = dynamic_cast<BinaryOpInstruction *>(iterate_inst); binary_iterate_inst->getInstType() == InstructionType::RshrType) {
        std::list<Instruction *> result_list;
        // 确定phi变量的初始值
        auto init_value = loopinfo->init_var_phi_insts_[another_introduction_value];

        auto mul_rshr_cnt = new BinaryOpInstruction(MulType, INT_BTYPE, loopinfo->next_block_, index_limit_value, binary_iterate_inst->getRight(), "ls.0");
        auto rshr_inst = new BinaryOpInstruction(RshrType, INT_BTYPE, loopinfo->next_block_, init_value, mul_rshr_cnt, "ls.1");

        result_list.push_back(rshr_inst);
        result_list.push_back(mul_rshr_cnt);

        // another_introduction_value->replaceAllUseWith(rshr_inst);
        for (auto user: another_user_insts) {
            user->replaceWithValue(another_introduction_value, rshr_inst);
        }

        auto false_label = br_inst->getFalseLabel();
        br_inst->setHasCond(false);
        br_inst->setLable(false_label);

        return result_list;
    }
    return {};
}

void LoopSimplify::runOnFunction() {
    if (!compute_loops_) {
        compute_loops_ = std::make_unique<ComputeLoops>(module_);
        compute_loops_->run();
    }
    // 找出当前函数内的循环体，满足简单的条件：(1)只包含两个循环体中的变量，一个循环的归纳变量a，一个累加的变量b；
    // (2)只包含一个基本块，body基本块除了跳转语句只包含两个语句，一个是归纳变量递增，另一个是变量b的累加
    // (3)a、b变量都只有一个user，也就是body中的迭代运算
    auto deep_loops_list = compute_loops_->getDeepestLoops(curr_func_);
    user_analysis_->analysis(curr_func_);
    for (auto &loop_info: deep_loops_list) {
        auto new_insts = isSimpleLoop(loop_info);
        if (new_insts.size() == 2) {
            for (auto inst: new_insts) {
                // printf("add inst to block %s\n", loop_info->next_block_->getName().c_str());
                loop_info->next_block_->addFrontInstruction(inst);
            }
        }
    }

}