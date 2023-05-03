//
// Created by 杨丰硕 on 2023/5/2.
//
#ifndef YFSCC_CODEGEN_H
#define YFSCC_CODEGEN_H

#include <fstream>
#include <unordered_map>
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

    using GlobalVarLabelPtr = std::unique_ptr<Label>;

    CodeGen(Module *ir_module);

    ~CodeGen() = default;

    const MachineModule *getModule() const {
        return module_.get();
    }

    void visit(Module *module);

    void visit(GlobalVariable *global);

    void visit(Function *function);

    void visit(Constant *constant);

    void visit(Instruction *inst);

    void visit(BinaryOpInstruction *binst);

    void visit(UnaryOpInstruction *uinst);

    void visit(StoreInstruction *inst);

    void visit(LoadInstruction *inst);

    void visit(AllocaInstruction *inst);

    void visit(RetInstruction *inst);

    void visit(BasicBlock *block);

    void visit(MemSetInstruction *inst);

    void visit(GEPInstruction *inst);

    void visit(BranchInstruction *inst);

    void visit(SetCondInstruction *inst);

    void visit(CallInstruction *inst);

    void visit(ZextInstruction *inst);

    void visit(CastInstruction *inst);

    void codeGenerate() {
        visit(const_cast<Module *>(module_->getIRModule()));
    }

    MachineModule *getMCModule() const {
        return module_.get();
    }

private:
    int virtual_reg_id_;

    MachineModulePtr module_;

    MachineBasicBlock *curr_machine_basic_block_;

    MachineFunction *curr_machine_function_;

    std::unordered_map<std::string, GlobalVarLabelPtr> global_var_map_;

    std::unordered_map<int, Value *> virtual_reg_map_;
};

#endif //YFSCC_CODEGEN_H
