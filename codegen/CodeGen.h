//
// Created by 杨丰硕 on 2023/5/2.
//
#ifndef YFSCC_CODEGEN_H
#define YFSCC_CODEGEN_H

#include <fstream>
#include "Machine.h"

class Module;
class GlobalVariable;
class Constant;
class Function;
class Instruction;
class BinaryOpInstruction;
class UnaryOpInstruction;
class StoreInstruction;
class LoadInstruction;
class AllocaInstruction;
class BasicBlock;
class RetInstruction;
class MemSetInstruction;
class GEPInstruction;
class BranchInstruction;
class SetCondInstruction;
class CallInstruction;
class ZextInstruction;
class Argument;
class CastInstruction;

class CodeGen {
public:
    using MachineModulePtr = std::unique_ptr<MachineModule>;

    CodeGen(Module *ir_module);

    ~CodeGen() = default;

    const MachineModule *getModule() const {
        return module_.get();
    }

    void visit(const Module *module);

    void visit(const GlobalVariable *global);

    void visit(const Function *function);

    void visit(const Constant *constant);

    void visit(const Instruction *inst);

    void visit(const BinaryOpInstruction *binst);

    void visit(const UnaryOpInstruction *uinst);

    void visit(const StoreInstruction *inst);

    void visit(const LoadInstruction *inst);

    void visit(const AllocaInstruction *inst);

    void visit(const RetInstruction *inst);

    void visit(const BasicBlock *block);

    void visit(const MemSetInstruction *inst);

    void visit(const GEPInstruction *inst);

    void visit(const BranchInstruction *inst);

    void visit(const SetCondInstruction *inst);

    void visit(const CallInstruction *inst);

    void visit(const ZextInstruction *inst);

    void visit(const CastInstruction *inst);

    void codeGenerate() {
        visit(module_->getIRModule());
    }

    MachineModule *getMCModule() const {
        return module_.get();
    }

private:
    MachineModulePtr module_;

};

#endif //YFSCC_CODEGEN_H
