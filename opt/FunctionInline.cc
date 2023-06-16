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

bool FunctionInline::canBeInline(Function *function) {
    if (call_graph_analysis_->isLibFunction(function)) {
        return false;
    }
    int32_t inst_size = 0;
    /*for (auto &bb_uptr: function->getBlocks()) {
        inst_size += bb_uptr->getInstructionSize();
    }*/
    if (/*inst_size > inline_insts_size ||*/call_graph_analysis_->isRecursive(function) || call_cnt_map_[function] > 1) {
        return false;
    }
    return true;
}

FunctionInline::FunctionInline(Module *module):
        curr_caller_basicblock_(nullptr),
        curr_inlined_exit_basicblock_(nullptr),
        Pass(module),
        call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module_)){
}

BasicBlock *FunctionInline::dfsSetCopyMap(BasicBlock *basic_block) {
    auto inlined_function = basic_block->getFunction();
    std::string inline_name_prefix = "il." + inlined_function->getName() + ".";

    auto new_basicblock = dynamic_cast<BasicBlock *>(copy_value_map_[basic_block]);
    assert(new_basicblock);

    new_basicblock->setHasJump(basic_block->getHasJump());
    new_basicblock->setHasRet(basic_block->getHasRet());
    new_basicblock->setWhileLoopDepth(curr_caller_basicblock_->getWhileLoopDepth() + basic_block->getWhileLoopDepth());


    auto &insts_list = basic_block->getInstructionList();
    for (auto &inst_uptr: insts_list) {
        Instruction *new_inst;
        auto inst = inst_uptr.get();
        auto new_inst_name = inline_name_prefix + inst->getName();
        switch (inst->getInstType()) {
            case StoreType: {
                // new_inst = new StoreInstruction(new_bb, inst->getBasicType(), );
                auto store_inst = dynamic_cast<StoreInstruction *>(inst);
                auto new_value = copy_value_map_[store_inst->getValue()];
                auto new_ptr = copy_value_map_[store_inst->getPtr()];

                new_inst = new StoreInstruction(curr_caller_basicblock_, inst->getBasicType(), new_value, new_ptr);
                break;
            }
            case AllocaType: {
                auto alloca_inst = dynamic_cast<AllocaInstruction *>(inst);
                std::string alloca_inst_name = alloca_inst->getName();
                if (alloca_inst->isArray()) {
                    new_inst = new AllocaInstruction(new_basicblock, alloca_inst->getBasicType(), alloca_inst->isPtrPtr(), alloca_inst->getArrayDimensionSize(), new_inst_name);
                } else {
                    new_inst = new AllocaInstruction(new_basicblock, alloca_inst->getBasicType(), alloca_inst->isPtrPtr(), new_inst_name);
                }
                break;
            }
            case LoadType: {
                auto load_inst = dynamic_cast<LoadInstruction *>(inst);
                auto load_addr = copy_value_map_[load_inst];
                new_inst = new LoadInstruction(new_basicblock, load_inst->getBasicType(), load_addr, load_inst->getBasicType(), new_inst_name);
                break;
            }
            case RetType: {         // 说明这个块是exit block,只是增加跳转语句，
                auto ret_inst = dynamic_cast<RetInstruction *>(inst);
                auto ret_type = ret_inst->getRetType();
                if (ret_type != VOID_BTYPE) {
                    exitblock_retvalue_map_[new_basicblock] = ret_inst->getRetValue();
                }
                copy_exit_blocks_[inlined_function].insert(new_basicblock);
                new_inst = new BranchInstruction(curr_caller_basicblock_, curr_inlined_exit_basicblock_);

                new_basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
                break;
            }
            case GEPType: {
                auto gep_inst = dynamic_cast<GEPInstruction *>(inst);
                std::vector<Value *> new_indexes;
                auto new_ptr = copy_value_map_[gep_inst->getPtr()];
                for (int i = 0; i < gep_inst->getIndexSize(); ++i) {
                    auto index = gep_inst->getIndexValue(i);
                    auto new_index = copy_value_map_[index];
                    new_indexes.push_back(new_index);
                }
                new_inst = new GEPInstruction(curr_caller_basicblock_, gep_inst->getBasicType(), new_ptr, gep_inst->isPtrOffset(), new_indexes, new_inst_name);
                break;
            }
            case MemSetType: {
                auto memset_inst = dynamic_cast<MemSetInstruction *>(inst);
                auto new_size = copy_value_map_[memset_inst->getSize()];
                auto new_base = copy_value_map_[memset_inst->getBase()];
                auto new_value = copy_value_map_[memset_inst->getValue()];

                new_inst = new MemSetInstruction(curr_caller_basicblock_, memset_inst->getBasicType(), new_base, new_size, new_value);
                break;
            }
            case BrType: {
                auto br_inst = dynamic_cast<BranchInstruction *>(inst);
                if (br_inst->isCondBranch()) {
                    auto new_cond = copy_value_map_[br_inst->getCond()];
                    auto new_true_label = copy_value_map_[br_inst->getTrueLabel()];
                    auto new_false_label = copy_value_map_[br_inst->getFalseLabel()];
                    new_inst = new BranchInstruction(curr_caller_basicblock_, new_cond, new_true_label, new_false_label);
                } else {
                    auto new_label = copy_value_map_[br_inst->getLabel()];
                    new_inst = new BranchInstruction(curr_caller_basicblock_, new_label);
                }
                new_basicblock->setBranchInst(dynamic_cast<BranchInstruction *>(new_inst));
                break;
            }
            case SetCondType: {
                auto set_cond_inst = dynamic_cast<SetCondInstruction *>(inst);
                auto new_left = copy_value_map_[set_cond_inst->getLeft()];
                auto new_right = copy_value_map_[set_cond_inst->getRight()];

                new_inst = new SetCondInstruction(curr_caller_basicblock_, set_cond_inst->getCmpType(), set_cond_inst->isFloatCmp(), new_left, new_right, new_inst_name);
                break;
            }
            case CallType: {
                auto call_inst = dynamic_cast<CallInstruction *>(inst);
                std::vector<Value *> new_actuals;
                for (int i = 0; i < call_inst->getActualSize(); ++i) {
                    auto new_actual = copy_value_map_[call_inst->getActual(i)];
                    new_actuals.push_back(new_actual);
                }

                new_inst = new CallInstruction(curr_caller_basicblock_, call_inst->getFunction(), new_actuals, new_inst_name);
                break;
            }
            case ZextType: {
                auto zext_inst = dynamic_cast<ZextInstruction *>(inst);
                auto new_left = copy_value_map_[zext_inst->getValue()];

                new_inst = new ZextInstruction(curr_caller_basicblock_, new_left, new_inst_name);
                break;
            }
            case CastType: {
                auto cast_inst = dynamic_cast<CastInstruction *>(inst);
                auto new_value = copy_value_map_[cast_inst->getValue()];

                new_inst = new CastInstruction(curr_caller_basicblock_, cast_inst->isI2F(), new_value, new_inst_name);
                break;
            }
            case PhiType: {
                auto phi_inst = dynamic_cast<PhiInstruction *>(inst);

                std::vector<Value *> new_phi_values;
                std::vector<BasicBlock *> new_basicblocks;

                for (int i = 0; i < phi_inst->getSize(); ++i) {
                    auto value_basicblock = phi_inst->getValueBlock(i);
                    auto new_phi_value = copy_value_map_[value_basicblock.first];
                    auto new_phi_basicblock = copy_value_map_[value_basicblock.second];
                    assert(new_phi_basicblock->getValueType() == BasicBlockValue);

                    new_phi_values.push_back(new_phi_value);
                    new_basicblocks.push_back(dynamic_cast<BasicBlock *>(new_phi_basicblock));
                }

                new_inst = new PhiInstruction(curr_caller_basicblock_, phi_inst->getBasicType(), new_phi_values, new_basicblocks, new_inst_name);
                break;
            }
            default:
                break;
        }
        new_basicblock->addInstruction(new_inst);
        copy_value_map_[inst] = new_inst;
    }

    for (auto succ_bb: basic_block->getSuccessorBlocks()) {
        if (!visited_basicblocks_.count(succ_bb)) {
            visited_basicblocks_.insert(succ_bb);
            dfsSetCopyMap(succ_bb);
        }
    }

    return new_basicblock;
}

void FunctionInline::inlineOnFunction() {
    for (auto call_inst: call_insts_for_inline_) {
        auto owner_bb = call_inst->getParent();
        auto bb_it = insert_point_nextit_map_[owner_bb];
        auto enter_bb = call_inst->getFunction()->getBlocks().front().get();
        visited_basicblocks_.clear();
        visited_basicblocks_.insert(enter_bb);
        preSetCopyMap(call_inst);
        dfsSetCopyMap(enter_bb);
        bb_it++;
        std::list<BasicBlock *> basicblock_list;
        generateBasicBlocks(call_inst->getFunction(), basicblock_list);
        for (auto &insert_bb: basicblock_list) {
            curr_func_->insertBlock(bb_it, insert_bb);
        }
    }
}

void FunctionInline::initForFunction() {
    call_cnt_map_.clear();
    call_insts_for_inline_.clear();
    insert_point_nextit_map_.clear();
    copy_value_map_.clear();
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
        copy_value_map_[bb_uptr.get()] = new BasicBlock(curr_func_, inline_name_prefix + bb_uptr->getName());
    }
}

void FunctionInline::generateBasicBlocks(Function *inlined_function, std::list<BasicBlock *> &bbs_list) {
    for (auto &bb_uptr: inlined_function->getBlocks()) {
        auto bb = bb_uptr.get();
        assert(copy_value_map_.count(bb));
        auto new_bb = dynamic_cast<BasicBlock *>(copy_value_map_[bb]);
        assert(new_bb);

        bbs_list.push_back(new_bb);

        for (auto &inst_uptr: bb->getInstructionList()) {
            auto inst = inst_uptr.get();
            assert(copy_value_map_.count(inst));
            new_bb->addInstruction(copy_value_map_[inst]);
        }
    }

    for (auto new_bb: bbs_list) {
        BranchInstruction *br_inst = new_bb->getBranchInst();
        if (br_inst && !new_bb->getHasRet()) {
            if (!br_inst->isCondBranch()) {
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getLabel()));
            } else {
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getTrueLabel()));
                BasicBlock::bindBasicBlock(new_bb, dynamic_cast<BasicBlock *>(br_inst->getFalseLabel()));
            }
        }
    }
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
    call_graph_analysis_->analysis();
    initForFunction();
    if (canBeInline(curr_func_)) {
        collectCallPoint();
        inlineOnFunction();
    }
}