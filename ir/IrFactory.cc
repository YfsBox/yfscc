//
// Created by 杨丰硕 on 2023/4/17.
//
#include "BasicBlock.h"
#include "IrBuilder.h"
#include "Constant.h"
#include "IrFactory.h"
#include "Function.h"
#include "Instruction.h"
#include "GlobalVariable.h"

IrFactory::ValuePtr IrFactory::createFConstantVar(float value) {
    return std::make_unique<ConstantVar>(value);
}

IrFactory::ValuePtr IrFactory::createIConstantVar(int32_t value) {
    return std::make_unique<ConstantVar>(value);
}

IrFactory::ValuePtr IrFactory::createFConstantArray(int dimension) {
    return nullptr;
}

IrFactory::ValuePtr IrFactory::createIConstantArray(int dimension) {
    return nullptr;
}

IrFactory::ValuePtr IrFactory::createIGlobalVar(const std::string &name, int32_t initval) {
    return std::make_unique<GlobalVariable>(false, initval, name);
}

IrFactory::ValuePtr IrFactory::createFGlobalVar(const std::string &name, float initval) {
    return std::make_unique<GlobalVariable>(false, initval, name);
}

IrFactory::ValuePtr IrFactory::createFloatFunction(const std::vector<std::string> &params, const std::string &name) {
    auto func = std::make_unique<Function>(BasicType::FLOAT_BTYPE, context_->curr_module_, name);
    for (auto &param : params) {
        func->addArgument(BasicType::FLOAT_BTYPE, param);
    }
    return func;
}

IrFactory::ValuePtr IrFactory::createIntFunction(const std::vector<std::string> &params, const std::string &name) {
    auto func = std::make_unique<Function>(BasicType::INT_BTYPE, context_->curr_module_, name);
    for (auto &param : params) {
        func->addArgument(BasicType::INT_BTYPE, param);
    }
    return func;
}

IrFactory::ValuePtr IrFactory::createVoidFunction(const std::vector<std::string> &params, const std::string &name) {
    auto func = std::make_unique<Function>(BasicType::VOID_BTYPE, context_->curr_module_, name);
    for (auto &param : params) {
        func->addArgument(BasicType::VOID_BTYPE, param);
    }
    return func;
}

IrFactory::ValuePtr IrFactory::createAddInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::AddType, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));

}

IrFactory::ValuePtr IrFactory::createSubInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::SubType, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createMulInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::MulType, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createAndInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::AndType, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createBasicBlock(const std::string &name) {
    return std::make_unique<BasicBlock>(context_->curr_function_, name);
}

IrFactory::ValuePtr IrFactory::createBrInstruction(Value *label) {
    return std::make_unique<BranchInstruction>(context_->curr_bb_, label);
}

IrFactory::ValuePtr IrFactory::createCallInstruction(const std::vector<Value *> &actuals) {
    context_->ssa_no_++;
    return std::make_unique<CallInstruction>(context_->curr_bb_, context_->curr_function_, actuals, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createCondBrInstruction(Value *cond, Value *truelabel, Value *falselabel) {
    return std::make_unique<BranchInstruction>(context_->curr_bb_, cond, truelabel, falselabel);
}

IrFactory::ValuePtr IrFactory::createDivInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::DivType, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createEqFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetEQ, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createEqICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetEQ, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createNeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetNE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createNeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetNE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetLE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetLE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetGE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetGE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLtICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetLT, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLtFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetLT, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGtICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetGT, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGtFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<SetCondInstruction>(context_->curr_bb_, SetCondInstruction::CmpCondType::SetGT, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createPhiInstruction(const std::vector<Value *> &values,
                                                    const std::vector<BasicBlock *> &bbs) {
    context_->ssa_no_++;
    return std::make_unique<PhiInstruction>(context_->curr_bb_, values, bbs, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createI2FCastInstruction(Value *value) {
    context_->ssa_no_++;
    return std::make_unique<CastInstruction>(context_->curr_bb_, true, value, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createF2ICastInstruction(Value *value) {
    context_->ssa_no_++;
    return std::make_unique<CastInstruction>(context_->curr_bb_, false, value, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createOrInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::OrType, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createXorInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return std::make_unique<BinaryOpInstruction>(InstructionType::XorType, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createFAllocaInstruction(const std::string &name) {      // 返回的都是指针
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::FLOAT_BTYPE, name);
}

IrFactory::ValuePtr IrFactory::createFAllocaInstruction() {
    context_->ssa_no_++;
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::FLOAT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIAllocaInstruction() {
    context_->ssa_no_++;
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::INT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIAllocaInstruction(const std::string &name) {
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::INT_BTYPE, name);
}

IrFactory::ValuePtr IrFactory::createIArrayAllocaInstruction(size_t array_size, const std::string &name) {
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::INT_BTYPE, array_size, name);
}

IrFactory::ValuePtr IrFactory::createFArrayAllocaInstruction(size_t array_size, const std::string &name) {
    return std::make_unique<AllocaInstruction>(context_->curr_bb_, BasicType::FLOAT_BTYPE, array_size, name);
}

IrFactory::ValuePtr IrFactory::createILoadInstruction(Value *ptr) {
    context_->ssa_no_++;
    return std::make_unique<LoadInstruction>(context_->curr_bb_, ptr, BasicType::INT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createFLoadInstruction(Value *ptr) {
    context_->ssa_no_++;
    return std::make_unique<LoadInstruction>(context_->curr_bb_, ptr, BasicType::FLOAT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIStoreInstruction(Value *value, Value *ptr) {
    return std::make_unique<StoreInstruction>(context_->curr_bb_, value, ptr);
}

IrFactory::ValuePtr IrFactory::createFStoreInstruction(Value *value, Value *ptr) {
    return std::make_unique<StoreInstruction>(context_->curr_bb_, value, ptr);
}

IrFactory::ValuePtr IrFactory::createVoidRetInstruction() {
    return std::make_unique<RetInstruction>(context_->curr_bb_);
}

IrFactory::ValuePtr IrFactory::createIRetInstruction(Value *value) {
    return std::make_unique<RetInstruction>(context_->curr_bb_, value, BasicType::INT_BTYPE);
}

IrFactory::ValuePtr IrFactory::createFRetInstruction(Value *value) {
    return std::make_unique<RetInstruction>(context_->curr_bb_, value, BasicType::FLOAT_BTYPE);
}

IrFactory::ValuePtr IrFactory::createFGEPInstruction(Value *ptr, Value *offset) {
    context_->ssa_no_++;
    return std::make_unique<GEPInstruction>(context_->curr_bb_, BasicType::FLOAT_BTYPE, ptr, offset, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIGEPInstruction(Value *ptr, Value *offset) {
    context_->ssa_no_++;
    return std::make_unique<GEPInstruction>(context_->curr_bb_, BasicType::INT_BTYPE, ptr, offset, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createConstIGlobalVar(int32_t initval, const std::string &name) {
    return std::make_unique<GlobalVariable>(true, initval, name);
}

IrFactory::ValuePtr IrFactory::createConstFGlobalVar(float initval, const std::string &name) {
    return std::make_unique<GlobalVariable>(true, initval, name);
}

