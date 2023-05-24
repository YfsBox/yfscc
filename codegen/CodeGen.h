//
// Created by 杨丰硕 on 2023/5/2.
//
#ifndef YFSCC_CODEGEN_H
#define YFSCC_CODEGEN_H

#include <fstream>
#include <unordered_map>
#include "Machine.h"
#include "MachineOperand.h"

class Value;
class Module;
class GlobalVariable;
class Constant;
class ConstantVar;
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

class VirtualReg;

class RegsAllocator;
class ColoringRegsAllocator;
class SimpleRegsAllocator;

class CodeGen {
public:
    friend class ColoringRegsAllocator;

    friend class SimpleRegsAllocator;

    using MachineModulePtr = std::unique_ptr<MachineModule>;

    using GlobalVarLabelPtr = std::unique_ptr<Label>;

    using Value2MachineRegMap = std::unordered_map<Value *, MachineOperand *>;

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

    MachineOperand *getImmOperandInBinary(int32_t value, MachineBasicBlock *bb, std::vector<MachineInst *> *moves = nullptr, bool is_float = false, bool *use_ip_base = nullptr, bool is_simple_ralloc = false);

    void addInstAboutStack(MachineFunction *function, int32_t offset, std::unordered_set<MachineReg::Reg> *regs = nullptr);

    void addRetBranchInExitBlocks(MachineFunction *function);

    void addPushInst(MachineBasicBlock *basicblock, std::unordered_set<MachineReg::Reg> *regs = nullptr);

    void addPopInst(MachineBasicBlock *basicblock, std::unordered_set<MachineReg::Reg> *regs = nullptr);

    static constexpr const int push_regs_offset_ = 100;

private:

    int moveStackOffset(int offset) {
        stack_offset_ += offset;
        return stack_offset_;
    }

    void setCurrMachineOperand(MachineOperand *operand) {
        curr_machine_operand_ = operand;
    }

    bool isImmNeedSplitMove(int32_t imm_value);

    bool canImmInBinary(int32_t value) {
        return value >= -1020 && value <= 1020;
    }

    void addMachineInst(MachineInst *inst) {
        curr_machine_basic_block_->addInstruction(inst);
    }

    MachineOperand *getCmpReusltInOperand(SetCondInstruction *set_cond_inst);

    MachineReg *getMachineReg(bool isfloat, int reg_no);

    VirtualReg *createVirtualReg(MachineOperand::ValueType value_type, Value *value = nullptr);

    VirtualReg *createVirtualReg(MachineFunction *function, MachineOperand::ValueType value_type);

    MachineOperand *constant2VirtualReg(int32_t const_value, bool can_be_imm);

    MachineOperand *loadGlobalVarAddr(GlobalVariable *global);

    MachineOperand *value2MachineOperand(Value *value, bool can_be_imm, bool *is_float = nullptr);

    int virtual_reg_id_;

    int stack_offset_;

    MachineOperand *sp_reg_;

    MachineOperand *fp_reg_;

    MachineOperand *lr_reg_;

    MachineModulePtr module_;

    MachineBasicBlock *curr_machine_basic_block_;

    MachineFunction *curr_machine_function_;

    MachineOperand *curr_machine_operand_;

    std::unordered_map<std::string, GlobalVarLabelPtr> global_var_map_;

    std::unordered_map<SetCondInstruction *, MachineBasicBlock::MachineInstListIt> set_cond_it_map_;

    Value2MachineRegMap value_machinereg_map_;
};

#endif //YFSCC_CODEGEN_H
