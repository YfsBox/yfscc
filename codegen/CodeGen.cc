//
// Created by 杨丰硕 on 2023/5/2.
//
#include "CodeGen.h"

CodeGen::CodeGen(Module *ir_module):
    module_(std::make_unique<MachineModule>(ir_module)){

}

void CodeGen::visit(const Module *module) {

}

void CodeGen::visit(const BasicBlock *block) {

}

void CodeGen::visit(const Instruction *inst) {

}

void CodeGen::visit(const Constant *constant) {

}

void CodeGen::visit(const Function *function) {

}

void CodeGen::visit(const GEPInstruction *inst) {

}

void CodeGen::visit(const RetInstruction *inst) {

}

void CodeGen::visit(const CallInstruction *inst) {

}

void CodeGen::visit(const CastInstruction *inst) {

}

void CodeGen::visit(const LoadInstruction *inst) {

}

void CodeGen::visit(const ZextInstruction *inst) {

}

void CodeGen::visit(const GlobalVariable *global) {

}

void CodeGen::visit(const StoreInstruction *inst) {

}

void CodeGen::visit(const AllocaInstruction *inst) {

}

void CodeGen::visit(const BranchInstruction *inst) {

}

void CodeGen::visit(const MemSetInstruction *inst) {

}

void CodeGen::visit(const SetCondInstruction *inst) {

}

void CodeGen::visit(const UnaryOpInstruction *uinst) {

}

void CodeGen::visit(const BinaryOpInstruction *binst) {

}

