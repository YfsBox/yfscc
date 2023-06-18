//
// Created by 杨丰硕 on 2023/6/14.
//

#include "FunctionInline.h"
#include "../ir/Function.h"
#include "../ir/BasicBlock.h"
#include "../ir/IrDumper.h"
#include "../ir/Argument.h"
#include "../ir/Module.h"
#include "../ir/GlobalVariable.h"
#include "../ir/Constant.h"

bool FunctionInline::canBeInline(Function *function) {
    if (call_graph_analysis_->isLibFunction(function)) {
        return false;
    }
    int32_t inst_size = 0;
    for (auto &bb_uptr: function->getBlocks()) {
        inst_size += bb_uptr->getInstructionSize();
    }
    if (inst_size > inline_insts_size || call_graph_analysis_->isRecursive(function) || call_cnt_map_[function] > 1) {
        return false;
    }
    return true;
}

FunctionInline::FunctionInline(Module *module):
        inline_point_cnt_(0),
        curr_caller_basicblock_(nullptr),
        curr_inlined_exit_basicblock_(nullptr),
        Pass(module),
        call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module_)){
}

void FunctionInline::setForUnfinishedCopyValue(Function *inlined_func) {
    for (auto &bb: inlined_func->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            auto copyed_inst = dynamic_cast<Instruction *>(getCopyValue(inst));
            assert(copyed_inst);

            for (int i = 0; i < inst->getOperandNum(); ++i) {
                auto operand = inst->getOperand(i);
                auto copyed_operand = copyed_inst->getOperand(i);
                if (!copyed_operand) {
                    copyed_inst->setOperand(getCopyValue(operand), i);
                }
            }
        }
    }
}

BasicBlock *FunctionInline::dfsSetCopyMap(BasicBlock *basic_block) {
    auto inlined_function = basic_block->getFunction();
    std::string inline_name_prefix = "il." + inlined_function->getName() + ".";

    auto new_basicblock = dynamic_cast<BasicBlock *>(getCopyValue(basic_block));

    new_basicblock->setHasJump(basic_block->getHasJump());
    new_basicblock->setHasRet(false);
    assert(curr_caller_basicblock_);
    new_basicblock->setWhileLoopDepth(curr_caller_basicblock_->getWhileLoopDepth() + basic_block->getWhileLoopDepth());


    auto &insts_list = basic_block->getInstructionList();
    for (auto &inst_uptr: insts_list) {
        Instruction *new_inst = nullptr;
        Instruction *inst = inst_uptr.get();
        auto new_inst_name = inline_name_prefix + inst->getName() + "." + std::to_string(inline_point_cnt_);
        switch (inst->getInstType()) {
            case StoreType: {
                // new_inst = new StoreInstruction(new_bb, inst->getBasicType(), );
                auto store_inst = dynamic_cast<const StoreInstruction *>(inst);
                auto new_value = getCopyValue(store_inst->getValue());
                auto new_ptr = getCopyValue(store_inst->getPtr());

                new_inst = new StoreInstruction(new_basicblock, inst->getBasicType(), new_value, new_ptr);
                break;
            }
            case AllocaType: {
                auto alloca_inst = dynamic_cast<const AllocaInstruction *>(inst);
                if (alloca_inst->isArray()) {
                    new_inst = new AllocaInstruction(new_basicblock, alloca_inst->getBasicType(), alloca_inst->isPtrPtr(), alloca_inst->getArrayDimensionSize(), new_inst_name);
                } else {
                    new_inst = new AllocaInstruction(new_basicblock, alloca_inst->getBasicType(), alloca_inst->isPtrPtr(), new_inst_name);
                }
                inlined_alloca_insts_.insert(new_inst);
                break;
            }
            case LoadType: {
                auto load_inst = dynamic_cast<const LoadInstruction *>(inst);
                auto load_addr = getCopyValue(load_inst->getPtr());
                new_inst = new LoadInstruction(new_basicblock, load_inst->getBasicType(), load_addr, load_inst->getBasicType(), new_inst_name);
                break;
            }
            case RetType: {         // 说明这个块是exit block,只是增加跳转语句，
                auto ret_inst = dynamic_cast<const RetInstruction *>(inst);
                auto ret_type = ret_inst->getRetType();
                if (ret_type != VOID_BTYPE) {
                    exitblock_retvalue_map_[new_basicblock] = getCopyValue(ret_inst->getRetValue());
                }
                copy_exit_blocks_[inlined_function].insert(new_basicblock);
                new_inst = new BranchInstruction(new_basicblock, curr_inlined_exit_basicblock_);

                new_basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
                break;
            }
            case GEPType: {
                auto gep_inst = dynamic_cast<const GEPInstruction *>(inst);
                std::vector<Value *> operands;
                for (int i = 0; i < gep_inst->getOperandNum(); ++i) {
                    auto operand = gep_inst->getOperand(i);
                    auto copy_operand = getCopyValue(operand);
                    operands.push_back(copy_operand);
                }

                new_inst = new GEPInstruction(new_basicblock, gep_inst->getBasicType(), operands, gep_inst->getArrayDimension(), gep_inst->isPtrOffset(), new_inst_name);
                break;
            }
            case MemSetType: {
                auto memset_inst = dynamic_cast<const MemSetInstruction *>(inst);
                auto new_size = getCopyValue(memset_inst->getSize());
                auto new_base = getCopyValue(memset_inst->getBase());
                auto new_value = getCopyValue(memset_inst->getValue());

                new_inst = new MemSetInstruction(new_basicblock, memset_inst->getBasicType(), new_base, new_size, new_value);
                break;
            }
            case BrType: {
                auto br_inst = dynamic_cast<const BranchInstruction *>(inst);
                if (br_inst->isCondBranch()) {
                    auto new_cond = getCopyValue(br_inst->getCond());
                    auto new_true_label = getCopyValue(br_inst->getTrueLabel());
                    auto new_false_label = getCopyValue(br_inst->getFalseLabel());
                    new_inst = new BranchInstruction(new_basicblock, new_cond, new_true_label, new_false_label);
                } else {
                    auto new_label = getCopyValue(br_inst->getLabel());
                    new_inst = new BranchInstruction(new_basicblock, new_label);
                }
                new_basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
                break;
            }
            case SetCondType: {
                auto set_cond_inst = dynamic_cast<const SetCondInstruction *>(inst);
                auto new_left = getCopyValue(set_cond_inst->getLeft());
                auto new_right = getCopyValue(set_cond_inst->getRight());

                new_inst = new SetCondInstruction(new_basicblock, set_cond_inst->getCmpType(), set_cond_inst->isFloatCmp(), new_left, new_right, new_inst_name);
                break;
            }
            case CallType: {
                auto call_inst = dynamic_cast<const CallInstruction *>(inst);
                std::vector<Value *> new_actuals;
                for (int i = 0; i < call_inst->getActualSize(); ++i) {
                    auto new_actual = getCopyValue(call_inst->getActual(i));
                    new_actuals.push_back(new_actual);
                }

                new_inst = new CallInstruction(new_basicblock, call_inst->getFunction(), new_actuals, new_inst_name);
                break;
            }
            case ZextType: {
                auto zext_inst = dynamic_cast<const ZextInstruction *>(inst);
                auto new_left = getCopyValue(zext_inst->getValue());

                new_inst = new ZextInstruction(new_basicblock, new_left, new_inst_name);
                break;
            }
            case CastType: {
                auto cast_inst = dynamic_cast<const CastInstruction *>(inst);
                auto new_value = getCopyValue(cast_inst->getValue());

                new_inst = new CastInstruction(new_basicblock, cast_inst->isI2F(), new_value, new_inst_name);
                break;
            }
            case PhiType: {
                auto phi_inst = dynamic_cast<const PhiInstruction *>(inst);

                std::vector<Value *> new_phi_values;
                std::vector<BasicBlock *> new_basicblocks;

                for (int i = 0; i < phi_inst->getSize(); ++i) {
                    auto value_basicblock = phi_inst->getValueBlock(i);
                    auto new_phi_value = getCopyValue(value_basicblock.first);
                    auto new_phi_basicblock = getCopyValue(value_basicblock.second);
                    assert(new_phi_basicblock->getValueType() == BasicBlockValue);

                    new_phi_values.push_back(new_phi_value);
                    new_basicblocks.push_back(dynamic_cast<BasicBlock *>(new_phi_basicblock));
                }

                new_inst = new PhiInstruction(new_basicblock, phi_inst->getBasicType(), new_phi_values, new_basicblocks, new_inst_name);
                break;
            }
            default:
                break;
        }
        if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
            auto new_lhs = getCopyValue(binary_inst->getLeft());
            auto new_rhs = getCopyValue(binary_inst->getRight());

            new_inst = new BinaryOpInstruction(inst->getInstType(), inst->getBasicType(), new_basicblock, new_lhs, new_rhs, new_inst_name);
        }
        if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
            auto new_value = getCopyValue(unary_inst->getValue());

            new_inst = new UnaryOpInstruction(inst->getInstType(), inst->getBasicType(), new_basicblock, new_value, new_inst_name);
        }

        assert(new_inst);
        // ir_dumper_->dump(new_inst);
        copy_value_map_[const_cast<Instruction *>(inst)] = new_inst;
    }

    for (auto succ_bb: basic_block->getSuccessorBlocks()) {
        if (!visited_basicblocks_.count(succ_bb)) {
            visited_basicblocks_.insert(succ_bb);
            dfsSetCopyMap(succ_bb);
        }
    }

    return new_basicblock;
}

void FunctionInline::replaceWithNextBasicBlock() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst_uptr.get()); phi_inst) {
                for (int i = 0; i < phi_inst->getSize(); ++i) {
                    auto basic_block = phi_inst->getBasicBlock(i);
                    if (next_block_map_.count(basic_block)) {
                        phi_inst->replaceWithValue(basic_block, next_block_map_[basic_block]);
                    }
                }
            }
        }
    }
}

void FunctionInline::inlineOnFunction() {
    for (auto call_inst: call_insts_for_inline_) {
        inline_point_cnt_++;
        ir_dumper_->dump(call_inst);
        auto owner_bb = call_inst->getParent();
        auto bb_it = insert_point_nextit_map_[owner_bb];
        auto enter_bb = call_inst->getFunction()->getBlocks().front().get();
        visited_basicblocks_.clear();
        visited_basicblocks_.insert(enter_bb);
        curr_caller_basicblock_ = bb_it->get();

        curr_inlined_exit_basicblock_ = new BasicBlock(curr_func_, curr_caller_basicblock_->getName() + ".ilnext." + std::to_string(inline_point_cnt_));
        curr_inlined_exit_basicblock_->setHasRet(curr_caller_basicblock_->getHasRet());
        curr_inlined_exit_basicblock_->setHasJump(curr_caller_basicblock_->getHasJump());
        curr_inlined_exit_basicblock_->setWhileLoopDepth(curr_caller_basicblock_->getWhileLoopDepth());
        curr_inlined_exit_basicblock_->setBranchInst(curr_caller_basicblock_->getBranchInst());

        next_block_map_[curr_caller_basicblock_] = curr_inlined_exit_basicblock_;

        preSetCopyMap(call_inst);
        dfsSetCopyMap(enter_bb);
        bb_it++;
        std::list<BasicBlock *> basicblock_list;
        generateBasicBlocks(call_inst->getFunction(), call_inst, basicblock_list);
        BasicBlock::bindBasicBlock(call_inst->getParent(), basicblock_list.front());
        // 将这些块插入
        for (auto bb: basicblock_list) {
            curr_func_->insertBlock(bb_it, bb);
        }
    }
    auto curr_func_enter_bb = curr_func_->getBlocks().front().get();

    for (auto inst: inlined_alloca_insts_) {
        inst->setParent(curr_func_enter_bb);
        curr_func_enter_bb->addFrontInstruction(inst);
    }

    replaceWithNextBasicBlock();
}

void FunctionInline::initForFunction() {
    inline_point_cnt_ = 0;
    call_cnt_map_.clear();
    call_insts_for_inline_.clear();
    insert_point_nextit_map_.clear();
    copy_value_map_.clear();
    inlined_alloca_insts_.clear();
    next_block_map_.clear();
}

void FunctionInline::preSetCopyMap(CallInstruction *call_inst) {
    auto call_function = call_inst->getFunction();
    for (int i = 0; i < call_inst->getActualSize(); ++i) {
        auto actual = call_inst->getActual(i);
        auto arg = call_function->getArgument(i);

        copy_value_map_[arg] = actual;
    }

    auto module = curr_func_->getParent();
    for (int i = 0; i < module->getGlobalSize(); ++i) {
        auto global = module->getGlobalVariable(i);
        copy_value_map_[global] = global;
    }
    std::string inline_name_prefix = "il." + call_function->getName() + ".";
    for (auto &bb_uptr: call_function->getBlocks()) {
        copy_value_map_[bb_uptr.get()] = new BasicBlock(curr_func_, inline_name_prefix + bb_uptr->getName() + "." + std::to_string(inline_point_cnt_));
    }
}

Value *FunctionInline::getCopyValue(Value *value) {
    Value *copy_value = nullptr;
    if (value->getValueType() == ConstantValue) {
        auto const_value = dynamic_cast<ConstantVar *>(value);
        assert(const_value);
        if (const_value->getBasicType() == INT_BTYPE) {
            copy_value = new ConstantVar(const_value->getIValue());
        } else {
            copy_value = new ConstantVar(const_value->getFValue());
        }
    } else {
        copy_value = copy_value_map_[value];
    }
    return copy_value;
}

void FunctionInline::generateBasicBlocks(Function *inlined_function, CallInstruction *call_inst, std::list<BasicBlock *> &bbs_list) {
    for (auto &bb_uptr: inlined_function->getBlocks()) {
        // ir_dumper_->dump(bb_uptr.get());
        auto bb = bb_uptr.get();
        assert(copy_value_map_.count(bb));
        auto new_bb = dynamic_cast<BasicBlock *>(getCopyValue(bb));
        assert(new_bb);

        bbs_list.push_back(new_bb);

        for (auto &inst_uptr: bb->getInstructionList()) {
            auto inst = inst_uptr.get();
            auto copyed_inst = getCopyValue(inst);
            if (inlined_alloca_insts_.count(dynamic_cast<Instruction *>(copyed_inst))) {
                continue;
            }
            new_bb->addInstruction(getCopyValue(inst));
        }
    }

    setForUnfinishedCopyValue(call_inst->getFunction());

    auto inlined_function_enter_bb = bbs_list.front();
    auto inlined_func_next_bb = curr_inlined_exit_basicblock_;

    auto &caller_insts_list = curr_caller_basicblock_->getInstructionList();
    PhiInstruction *next_bb_phi_inst = nullptr;
    if (call_inst->getBasicType() != VOID_BTYPE) {
        std::vector<BasicBlock *> bbs;
        std::vector<Value *> values;
        for (auto exit_bb: copy_exit_blocks_[call_inst->getFunction()]) {
            bbs.push_back(exit_bb);
            values.push_back(exitblock_retvalue_map_[exit_bb]);
        }
        next_bb_phi_inst = new PhiInstruction(inlined_func_next_bb, call_inst->getBasicType(), values, bbs, call_inst->getName());
        call_inst->replaceAllUseWith(next_bb_phi_inst);
    }
    // 定位到call inst所在的迭代器，并移除
    auto insts_it = caller_insts_list.begin();
    for (; insts_it != caller_insts_list.end() && insts_it->get() != call_inst; ++insts_it);
    insts_it = caller_insts_list.erase(insts_it);
    // 也移除call_inst之后的
    for (; insts_it != caller_insts_list.end();) {
        inlined_func_next_bb->addInstruction(std::move(*insts_it));
        insts_it = caller_insts_list.erase(insts_it);
    }
    if (next_bb_phi_inst) {
        inlined_func_next_bb->addFrontInstruction(next_bb_phi_inst);
    }
    auto branch_inlined_func_inst = new BranchInstruction(curr_caller_basicblock_, inlined_function_enter_bb);
    curr_caller_basicblock_->addInstruction(branch_inlined_func_inst);
    curr_caller_basicblock_->setBranchInst(branch_inlined_func_inst);
    curr_caller_basicblock_->setHasRet(false);
    curr_caller_basicblock_->setHasJump(true);

    bbs_list.push_back(inlined_func_next_bb);

    for (auto new_bb: bbs_list) {
        BranchInstruction *br_inst = new_bb->getBranchInst();
        if (br_inst && !new_bb->getHasRet()) {
            ir_dumper_->dump(br_inst);
            if (!br_inst->isCondBranch()) {
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getLabel()));
            } else {
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getTrueLabel()));
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getFalseLabel()));
            }
        }
    }
}

void FunctionInline::splitAndInsert(const std::list<BasicBlock *> &basicblocks) {
    auto inlined_func_enter_bb = basicblocks.front();
    // auto inlined_func_next_bb = new BasicBlock(curr_func_, );


}

void FunctionInline::collectCallPoint() {
    for (auto &bb: curr_func_->getBlocks()) {
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr: insts_list) {
            auto inst = inst_uptr.get();
            if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst) {
                auto call_func = call_inst->getFunction();
                call_cnt_map_[call_func]++;
            }
        }
    }

    auto &blocks_list = curr_func_->getBlocks();
    for (auto bb_it = blocks_list.begin(); bb_it != blocks_list.end(); ++bb_it) {
        auto &bb = *bb_it;
        auto &insts_list = bb->getInstructionList();
        for (auto &inst_uptr : insts_list) {
            auto inst = inst_uptr.get();
            if (auto call_inst = dynamic_cast<CallInstruction *>(inst); call_inst && canBeInline(call_inst->getFunction())) {
                call_insts_for_inline_.insert(call_inst);
                insert_point_nextit_map_[bb.get()] = bb_it;
            }
        }
    }

}

void FunctionInline::runOnFunction() {
    printf("inlined for function %s\n", curr_func_->getName().c_str());
    call_graph_analysis_->analysis();
    initForFunction();
    collectCallPoint();
    inlineOnFunction();

    for (auto &bb: curr_func_->getBlocks()) {
        bb->clearSuccessors();
        bb->clearPresuccessors();
    }
    curr_func_->bindBasicBlocks();
}