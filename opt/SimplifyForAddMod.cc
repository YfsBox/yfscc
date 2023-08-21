//
// Created by 杨丰硕 on 2023/8/21.
//
#include <numeric>
#include "SimplifyForAddMod.h"
#include "GlobalAnalysis.h"
#include "ComputeLoops.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"
#include "../ir/Instruction.h"

SimplifyForAddMod::SimplifyForAddMod(Module *module): Pass(module), compute_loops_(nullptr), user_analysis_(std::make_unique<UserAnalysis>()) {}

// 分析出循环中的两个归纳变量:(1)初始化为0，并且限制为n, 步长为1(每一步都是+1); (2)另外一个满足首先加一个固定的常量，然后在一个常量上取模的情况
std::list<Instruction *> SimplifyForAddMod::simpleLoopAnalysis(const std::shared_ptr<ComputeLoops::LoopInfo> &loopinfo) {
    // printf("begin simple loop analysis, the block size is %d\n", loopinfo->loop_body_.size());
    if (loopinfo->loop_body_.size() != 1) {
        return {};
    }
    auto body_block = *loopinfo->loop_body_.begin();
    if (body_block->getInstructionList().size() != 4 || loopinfo->iterator_var_phi_insts_.size() != 2) {
        return {};
    }
    // printf("begin analysis index value\n");
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
    // printf("can get index value\n");

    // 以上过程都是对index变量的分析，之后对另一个归纳变量进行分析，满足每一步都累加固定常量，并且对固定常量取模的形式
    // 首先找到这个phi指令
    PhiInstruction *another_introduction_value = nullptr;
    for (auto &[phi_value, init_value]: loopinfo->init_var_phi_insts_) {
        if (phi_value != index_value) {
            another_introduction_value = phi_value;
            break;
        }
    }

    Value *curr_value = another_introduction_value;
    // 检查是否只被循环里的一个add所use
    BinaryOpInstruction *add_inst_for_another = nullptr;
    for (auto inst: user_analysis_->getUserInsts(curr_value)) {
        if (inst->getParent() == body_block) {
            add_inst = dynamic_cast<BinaryOpInstruction *>(inst);
        }
    }
    if (!add_inst || add_inst->getRight()->getValueType() != ConstantValue) {
        return {};
    }
    // 检查mod表达式是否只被phi指令饮用
    BinaryOpInstruction *mod_inst_for_add = nullptr;
    for (auto inst: user_analysis_->getUserInsts(add_inst)) {
        if (inst->getParent() == body_block) {
            mod_inst_for_add = dynamic_cast<BinaryOpInstruction *>(inst);
        }
    }
    if (!mod_inst_for_add || mod_inst_for_add->getRight()->getValueType() != ConstantValue) {
        return {};
    }
    // 这个mod只有一个user，那就是最初的phi指令
    auto mod_inst_users = user_analysis_->getUserInsts(mod_inst_for_add);
    if (mod_inst_users.size() == 1 && *mod_inst_users.begin() == another_introduction_value) {
        // 存在一条链路 phi_var -> add_result = phi_var + const_value1 -> mod_result = add_result + const_value2 -> phi_var
        // 通过分析user得出
        std::list<Instruction *> result_list;

        int32_t add_num_value = dynamic_cast<ConstantVar *>(add_inst->getRight())->getIValue();
        int32_t mod_num_value = dynamic_cast<ConstantVar *>(mod_inst_for_add->getRight())->getIValue();

        auto lcm_number = std::lcm(add_num_value, mod_num_value);
        auto period = lcm_number / add_num_value;

        auto mod1_inst = new BinaryOpInstruction(InstructionType::ModType, INT_BTYPE, loopinfo->next_block_, index_limit_value, new ConstantVar(period), "sam.mod1");
        auto mul_inst = new BinaryOpInstruction(InstructionType::MulType, INT_BTYPE, loopinfo->next_block_, mod1_inst, new ConstantVar(add_num_value), "sam.mul");
        auto mod2_inst = new BinaryOpInstruction(InstructionType::ModType, INT_BTYPE, loopinfo->next_block_, mul_inst, new ConstantVar(mod_num_value), "sam.mod2");

        result_list.push_front(mod1_inst);
        result_list.push_front(mul_inst);
        result_list.push_front(mod2_inst);

        for (auto user: user_analysis_->getUserInsts(another_introduction_value)) {
            user->replaceWithValue(another_introduction_value, mod2_inst);
        }

        auto false_label = br_inst->getFalseLabel();
        br_inst->setHasCond(false);
        br_inst->setLable(false_label);
        body_block->clearPresuccessors();

        return result_list;
    }

    return {};
}

void SimplifyForAddMod::runOnFunction() {
    if (!compute_loops_) {
        compute_loops_ = std::make_unique<ComputeLoops>(module_);
        compute_loops_->run();
    }

    auto deep_loops_list = compute_loops_->getDeepestLoops(curr_func_);
    user_analysis_->analysis(curr_func_);
    for (auto &loop_info: deep_loops_list) {
        auto new_insts = simpleLoopAnalysis(loop_info);
        if (new_insts.size() == 3) {
            for (auto inst: new_insts) {
                // printf("add inst to block %s\n", loop_info->next_block_->getName().c_str());
                loop_info->next_block_->addFrontInstruction(inst);
            }
        }
    }
}