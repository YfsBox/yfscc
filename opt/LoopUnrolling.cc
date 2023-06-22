//
// Created by 杨丰硕 on 2023/6/21.
//
#include "ComputeLoops.h"
#include "LoopUnrolling.h"
#include "../ir/BasicBlock.h"
#include "../ir/Constant.h"

void LoopUnrolling::unroll(ComputeLoops::LoopInfoPtr &loopinfo) {
    if (loopinfo->has_ret_or_break_) {
        return;
    }
    auto cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());
    assert(cmp_inst);
    if (cmp_inst->getBasicType() != INT_BTYPE) {
        return;
    }
    auto cmp_type = cmp_inst->getCmpType();
    if (cmp_type == SetCondInstruction::SetEQ || cmp_type == SetCondInstruction::SetNE) {
        return;
    }
    auto phi_var = loopinfo->getCondVarPhiInst();
    if (!phi_var || phi_var->getParent() != loopinfo->enter_block_) {
        return;
    }

    LoopUnrollingInfo unrolling_info(loopinfo);

    if (isFixedIterations(loopinfo, unrolling_info)) {
        printf("the loopinfo is %s, and the limit is %d, init is %d, stride is %d, iteration cnt is %d\n",
               loopinfo->enter_block_->getName().c_str(),
               unrolling_info.limit_,
               unrolling_info.init_value_,
               unrolling_info.stride_,
               unrolling_info.cal_iteratorions_cnt_);




    } else if (isNotFixedIterations(loopinfo)) {



    }


}

bool LoopUnrolling::isFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo, LoopUnrollingInfo &unrolling_info) const {
    SetCondInstruction *cmp_inst = dynamic_cast<SetCondInstruction *>(loopinfo->getSetCondInst());
    auto cmp_lhs = cmp_inst->getLeft();
    auto cmp_rhs = cmp_inst->getRight();
    auto phi_value_inst = loopinfo->getCondVarPhiInst();
    if (phi_value_inst->getSize() != 2) {
        return false;
    }
    int32_t loop_limit_value;
    int32_t init_value;

    Value *value_from_body = nullptr;
    BasicBlock *bb_from_body = nullptr;
    if (cmp_lhs->getValueType() == ConstantValue) {
        auto lhs_const_value = dynamic_cast<ConstantVar *>(cmp_lhs);
        loop_limit_value = lhs_const_value->getIValue();
    } else {
        auto rhs_const_value = dynamic_cast<ConstantVar *>(cmp_rhs);
        loop_limit_value = rhs_const_value->getIValue();
    }

    auto phi_bb_value_pair0 = phi_value_inst->getValueBlock(0);
    auto phi_bb_value_pair1 = phi_value_inst->getValueBlock(1);

    if (phi_bb_value_pair0.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair0.first)->getIValue();
        value_from_body = phi_bb_value_pair1.first;
        bb_from_body = phi_bb_value_pair1.second;
    } else if (phi_bb_value_pair1.first->getValueType() == ConstantValue) {
        init_value = dynamic_cast<ConstantVar *>(phi_bb_value_pair1.first)->getIValue();
        value_from_body = phi_bb_value_pair0.first;
        bb_from_body = phi_bb_value_pair0.second;
    } else {
        return false;
    }           // 如果两边都没有这样的常量值，也就是说明循环的次数无法在编译期间确定

    if (!loopinfo->isInLoop(bb_from_body)) {        // 判断这个value是否来自循环之中
        return false;
    }
    // 之后追踪这个value，判断是否是采用了加减移动了固定布长的方式
    // 处于循环体中，并且是+一个const或者减一个const的方式
    auto inst_from_body = dynamic_cast<Instruction *>(value_from_body);
    assert(inst_from_body);

    Value *binary_lhs, *binary_rhs;
    BinaryOpInstruction *binary_inst = nullptr;
    if (inst_from_body->getInstType() == AddType || inst_from_body->getInstType() == SubType) {
        binary_inst = dynamic_cast<BinaryOpInstruction *>(inst_from_body);
    }
    if (!binary_inst) {
        return false;
    }

    binary_lhs = binary_inst->getLeft();
    binary_rhs = binary_inst->getRight();


    ConstantVar *const_var = nullptr;
    if (binary_lhs == phi_value_inst && binary_rhs->getValueType() == ConstantValue) {      // 推导出步数
        const_var = dynamic_cast<ConstantVar *>(binary_rhs);
    } else if (binary_inst->getInstType() == AddType && binary_lhs->getValueType() == ConstantValue && binary_rhs == phi_value_inst) {      // 推导出步数
        const_var = dynamic_cast<ConstantVar *>(binary_lhs);
    } else {
        return false;
    }
    int32_t stride_cnt = const_var->getIValue();

    unrolling_info.init_value_ = init_value;
    unrolling_info.limit_ = loop_limit_value;
    unrolling_info.stride_ = stride_cnt;
    unrolling_info.cal_iteratorions_cnt_ = std::abs((loop_limit_value - init_value) / stride_cnt);

    return true;
}

bool LoopUnrolling::isNotFixedIterations(const ComputeLoops::LoopInfoPtr &loopinfo) const {

    return false;
}

void LoopUnrolling::runOnFunction() {
    if (!has_compute_loop_) {
        compute_loops_.release();
        compute_loops_ = std::make_unique<ComputeLoops>(module_);
        compute_loops_->run();
        has_compute_loop_ = true;
    }
    auto &loop_infos = compute_loops_->getDeepestLoops(curr_func_);
    for (auto &loop_info: loop_infos) {
        printf("the enter bb is %s, and exit bb is %s\n", loop_info->enter_block_->getName().c_str(), loop_info->exit_block_->getName().c_str());
        for (auto body_bb: loop_info->loop_body_) {
            printf("the body bb is %s\n", body_bb->getName().c_str());
        }

        unroll(loop_info);
    }

}