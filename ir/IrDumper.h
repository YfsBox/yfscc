//
// Created by 杨丰硕 on 2023/4/20.
//

#ifndef YFSCC_IRDUMPER_H
#define YFSCC_IRDUMPER_H

#include <iostream>

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

private:

    std::string getBasicType(Instruction *inst) const;

    std::string dumpValue(Value *value) const;

    std::string getOptype(Instruction *inst) const;

    std::ostream &out_;
};

#endif //YFSCC_IRDUMPER_H
