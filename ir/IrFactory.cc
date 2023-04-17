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
    return nullptr;
}

IrFactory::ValuePtr IrFactory::createCondBrInstruction(Value *cond, Value *truelabel, Value *falselabel) {

}

IrFactory::ValuePtr IrFactory::createDivInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createEqFCmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createEqICmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createNeICmpInstruction(Value *left, Value *right) {


}

IrFactory::ValuePtr IrFactory::createNeFCmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createLeFCmpInstruction(Value *left, Value *right) {


}

IrFactory::ValuePtr IrFactory::createLeICmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createGeFCmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createGeICmpInstruction(Value *left, Value *right) {


}

IrFactory::ValuePtr IrFactory::createLtICmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createLtFCmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createGtICmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createGtFCmpInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createPhiInstruction(const std::vector<Value *> &values,
                                                    const std::vector<BasicBlock *> &bbs) {

}

IrFactory::ValuePtr IrFactory::createI2FCastInstruction(Value *value) {

}

IrFactory::ValuePtr IrFactory::createF2ICastInstruction(Value *value) {


}

IrFactory::ValuePtr IrFactory::createOrInstruction(Value *left, Value *right) {


}

IrFactory::ValuePtr IrFactory::createXorInstruction(Value *left, Value *right) {

}

IrFactory::ValuePtr IrFactory::createFAllocaInstruction() {

}

IrFactory::ValuePtr IrFactory::createIAllocaInstruction() {

}

IrFactory::ValuePtr IrFactory::createIArrayAllocaInstruction(size_t array_size) {

}

IrFactory::ValuePtr IrFactory::createFArrayAllocaInstruction(size_t array_size) {

}

IrFactory::ValuePtr IrFactory::createILoadInstruction(Value *ptr) {

}

IrFactory::ValuePtr IrFactory::createFLoadInstruction(Value *ptr) {


}

IrFactory::ValuePtr IrFactory::createStoreInstruction(Value *value, Value *ptr) {


}

IrFactory::ValuePtr IrFactory::createVoidRetInstruction() {

}

IrFactory::ValuePtr IrFactory::createRetInstruction(Value *value) {

}


