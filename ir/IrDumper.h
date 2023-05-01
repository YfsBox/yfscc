//
// Created by 杨丰硕 on 2023/4/20.
//

#ifndef YFSCC_IRDUMPER_H
#define YFSCC_IRDUMPER_H

#include <iostream>
#include <unordered_map>

class Module;
class GlobalVariable;
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

class IrDumper {
public:
    explicit IrDumper(std::ostream &out);

    ~IrDumper();

    void dump(Module *module);

    void dump(GlobalVariable *global);

    void dump(Function *function);

    void dump(Constant *constant);

    void dump(Instruction *inst);

    void dump(BinaryOpInstruction *binst);

    void dump(UnaryOpInstruction *uinst);

    void dump(StoreInstruction *inst);

    void dump(LoadInstruction *inst);

    void dump(AllocaInstruction *inst);

    void dump(RetInstruction *inst);

    void dump(BasicBlock *block);

    void dump(MemSetInstruction *inst);

    void dump(GEPInstruction *inst);

    void dump(BranchInstruction *inst);

    void dump(SetCondInstruction *inst);

    void dump(CallInstruction *inst);

    void dump(ZextInstruction *inst);

    void dump(CastInstruction *inst);

private:

    std::string getBasicType(Instruction *inst) const;

    std::string getBasicType(BasicType basic_type) const;

    std::string dumpValue(BasicType basic_type, Value *value) const;

    std::string dumpValue(Value *value) const;

    std::string getOptype(Instruction *inst, BasicType basic_type) const;

    std::string getCmpCondType(SetCondInstruction *inst) const;

    std::string getArrayType(const std::vector<int32_t> &dimension, BasicType basic_type);

    std::string getVarType(BasicType btype, Value *value, bool isptr);

    std::string getStrTypeAsOperand(Value *value);

    std::string getStrTypeAsOperand(Instruction *inst);

    std::string getStrTypeAsOperand(GlobalVariable *global);

    std::string getStrTypeAsOperand(Argument *argument);

    void setGlobalArrayInitListMap(ConstantArray *const_array, std::vector<int32_t> &init_list_map);

    void dumpGlobalArrayInitListMap(BasicType btype, std::vector<int32_t> dimension, std::vector<int32_t>& init_list_map, int l, int r);

    std::ostream &out_;
};

#endif //YFSCC_IRDUMPER_H
