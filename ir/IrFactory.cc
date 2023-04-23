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

IrContext *IrFactory::context_ = nullptr;

void IrFactory::InitContext(IrContext *context) {
    context_ = context;
}

IrFactory::ValuePtr IrFactory::createArgument(bool is_float, Function *function, const std::string &name) {
    return new Argument(is_float, function, name);
}

IrFactory::ValuePtr IrFactory::createArrayArgument(bool is_float, Function *function, const std::vector<int32_t> &dimension,
                                                   const std::string &name) {
    return new Argument(is_float, dimension, function, name);
}


IrFactory::ValuePtr IrFactory::createFConstantVar(float value) {
    return new ConstantVar(value);
}

IrFactory::ValuePtr IrFactory::createIConstantVar(int32_t value) {
    return new ConstantVar(value);
}

IrFactory::ValuePtr IrFactory::createFConstantArray(const std::vector<int32_t> &dimensions, const std::string &name) {
    return new ConstantArray(true, dimensions, name);
}

IrFactory::ValuePtr IrFactory::createIConstantArray(const std::vector<int32_t> &dimensions, const std::string &name) {
    return new ConstantArray(false, dimensions, name);
}

IrFactory::ValuePtr IrFactory::createIGlobalVar(const std::string &name, int32_t initval) {
    return new GlobalVariable(false, initval, name);
}

IrFactory::ValuePtr IrFactory::createFGlobalVar(const std::string &name, float initval) {
    return new GlobalVariable(false, initval, name);
}

IrFactory::ValuePtr IrFactory::createIGlobalArray(bool is_const, ConstantArray *constarray, const std::string &name) {
    return new GlobalVariable(is_const, constarray, name);
}

IrFactory::ValuePtr IrFactory::createFGlobalArray(bool is_const, ConstantArray *constarray, const std::string &name) {
    return new GlobalVariable(is_const, constarray, name);
}

IrFactory::ValuePtr IrFactory::createFloatFunction(const std::string &name) {
    return new Function(BasicType::FLOAT_BTYPE, context_->curr_module_, name);
}

IrFactory::ValuePtr IrFactory::createIntFunction(const std::string &name) {
    return new Function(BasicType::INT_BTYPE, context_->curr_module_, name);
}

IrFactory::ValuePtr IrFactory::createVoidFunction(const std::string &name) {
    return new Function(BasicType::VOID_BTYPE, context_->curr_module_, name);
}

IrFactory::ValuePtr IrFactory::createAddInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::AddType,btype, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));

}

IrFactory::ValuePtr IrFactory::createSubInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::SubType, btype, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createMulInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::MulType, btype, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createAndInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::AndType, btype, context_->curr_bb_,
                                                 left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createModInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::ModType, btype, context_->curr_bb_,
                                   left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createBasicBlock(const std::string &name) {
    return new BasicBlock(context_->curr_function_, name);
}

IrFactory::ValuePtr IrFactory::createBrInstruction(Value *label) {
    return new BranchInstruction(context_->curr_bb_, label);
}

IrFactory::ValuePtr IrFactory::createCallInstruction(const std::vector<Value *> &actuals) {
    context_->ssa_no_++;
    return new CallInstruction(context_->curr_bb_, context_->curr_function_, actuals, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createCondBrInstruction(Value *cond, Value *truelabel, Value *falselabel) {
    return new BranchInstruction(context_->curr_bb_, cond, truelabel, falselabel);
}

IrFactory::ValuePtr IrFactory::createDivInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction(InstructionType::DivType, btype, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createEqFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetEQ, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createEqICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetEQ, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createNeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetNE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createNeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetNE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetLE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetLE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGeFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetGE, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGeICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetGE, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLtICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetLT, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createLtFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetLT, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGtICmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetGT, false, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createGtFCmpInstruction(Value *left, Value *right) {
    context_->ssa_no_++;
    return new SetCondInstruction (context_->curr_bb_, SetCondInstruction::CmpCondType::SetGT, true, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createPhiInstruction(BasicType basic_type, const std::vector<Value *> &values,
                                                    const std::vector<BasicBlock *> &bbs) {
    context_->ssa_no_++;
    return new PhiInstruction (context_->curr_bb_, basic_type, values, bbs, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createI2FCastInstruction(Value *value) {
    context_->ssa_no_++;
    return new CastInstruction (context_->curr_bb_, true, value, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createF2ICastInstruction(Value *value) {
    context_->ssa_no_++;
    return new CastInstruction (context_->curr_bb_, false, value, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createOrInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction (InstructionType::OrType, btype, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createXorInstruction(Value *left, Value *right, BasicType btype) {
    context_->ssa_no_++;
    return new BinaryOpInstruction (InstructionType::XorType, btype, context_->curr_bb_, left, right, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createFAllocaInstruction(const std::string &name) {      // 返回的都是指针
    return new AllocaInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, name);
}

IrFactory::ValuePtr IrFactory::createFAllocaInstruction() {
    context_->ssa_no_++;
    return new AllocaInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIAllocaInstruction() {
    context_->ssa_no_++;
    return new AllocaInstruction (context_->curr_bb_, BasicType::INT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIAllocaInstruction(const std::string &name) {
    return new AllocaInstruction (context_->curr_bb_, BasicType::INT_BTYPE, name);
}

IrFactory::ValuePtr IrFactory::createIArrayAllocaInstruction(size_t array_size, const std::string &name) {
    return new AllocaInstruction (context_->curr_bb_, BasicType::INT_BTYPE, array_size, name);
}

IrFactory::ValuePtr IrFactory::createFArrayAllocaInstruction(size_t array_size, const std::string &name) {
    return new AllocaInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, array_size, name);
}

IrFactory::ValuePtr IrFactory::createILoadInstruction(Value *ptr) {
    context_->ssa_no_++;
    return new LoadInstruction (context_->curr_bb_, BasicType::INT_BTYPE, ptr, BasicType::INT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createFLoadInstruction(Value *ptr) {
    context_->ssa_no_++;
    return new LoadInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, ptr, BasicType::FLOAT_BTYPE, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIStoreInstruction(Value *value, Value *ptr) {
    return new StoreInstruction (context_->curr_bb_, BasicType::INT_BTYPE, value, ptr);
}

IrFactory::ValuePtr IrFactory::createFStoreInstruction(Value *value, Value *ptr) {
    return new StoreInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, value, ptr);
}

IrFactory::ValuePtr IrFactory::createVoidRetInstruction() {
    return new RetInstruction (context_->curr_bb_);
}

IrFactory::ValuePtr IrFactory::createIRetInstruction(Value *value) {
    return new RetInstruction (context_->curr_bb_, BasicType::INT_BTYPE, value);
}

IrFactory::ValuePtr IrFactory::createFRetInstruction(Value *value) {
    return new RetInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, value);
}

IrFactory::ValuePtr IrFactory::createFGEPInstruction(Value *ptr, Value *offset) {
    context_->ssa_no_++;
    return new GEPInstruction (context_->curr_bb_, BasicType::FLOAT_BTYPE, ptr, offset, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createIGEPInstruction(Value *ptr, Value *offset) {
    context_->ssa_no_++;
    return new GEPInstruction (context_->curr_bb_, BasicType::INT_BTYPE, ptr, offset, std::to_string(context_->ssa_no_));
}

IrFactory::ValuePtr IrFactory::createConstIGlobalVar(int32_t initval, const std::string &name) {
    return new GlobalVariable (true, initval, name);
}

IrFactory::ValuePtr IrFactory::createConstFGlobalVar(float initval, const std::string &name) {
    return new GlobalVariable(true, initval, name);
}

IrFactory::ValuePtr IrFactory::createINotInstruction(Value *value) {
    context_->ssa_no_++;
    return new UnaryOpInstruction(NotType, BasicType::INT_BTYPE, context_->curr_bb_, value);
}

IrFactory::ValuePtr IrFactory::createFNotInstruction(Value *value) {
    context_->ssa_no_++;
    return new UnaryOpInstruction(NotType, BasicType::FLOAT_BTYPE, context_->curr_bb_, value);
}

IrFactory::ValuePtr IrFactory::createINegInstruction(Value *value) {
    context_->ssa_no_++;
    return new UnaryOpInstruction(NegType, BasicType::INT_BTYPE, context_->curr_bb_, value);
}

IrFactory::ValuePtr IrFactory::createFNegInstruction(Value *value) {
    context_->ssa_no_++;
    return new UnaryOpInstruction(NegType, BasicType::FLOAT_BTYPE, context_->curr_bb_, value);
}

