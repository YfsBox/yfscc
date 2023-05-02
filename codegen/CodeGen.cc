//
// Created by 杨丰硕 on 2023/5/2.
//
#include "CodeGen.h"

CodeGen::CodeGen(std::ofstream &out, Module *ir_module):
    out_stream_(out),
    module_(std::make_unique<MachineModule>(ir_module)){

}

void CodeGen::visit(Module *module) {

}

void CodeGen::visit(BasicBlock *block) {

}

void CodeGen::visit(Instruction *inst) {

}

void CodeGen::visit(Constant *constant) {

}

void CodeGen::visit(Function *function) {

}

void CodeGen::visit(GEPInstruction *inst) {

}

void CodeGen::visit(RetInstruction *inst) {

}

void CodeGen::visit(CallInstruction *inst) {

}

void CodeGen::visit(CastInstruction *inst) {

}

void CodeGen::visit(LoadInstruction *inst) {

}

void CodeGen::visit(ZextInstruction *inst) {

}

void CodeGen::visit(GlobalVariable *global) {

}

void CodeGen::visit(StoreInstruction *inst) {

}

void CodeGen::visit(AllocaInstruction *inst) {

}

void CodeGen::visit(BranchInstruction *inst) {

}

void CodeGen::visit(MemSetInstruction *inst) {

}

void CodeGen::visit(SetCondInstruction *inst) {

}

void CodeGen::visit(UnaryOpInstruction *uinst) {

}

void CodeGen::visit(BinaryOpInstruction *binst) {

}

