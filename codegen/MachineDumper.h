//
// Created by 杨丰硕 on 2023/5/3.
//

#ifndef YFSCC_MACHINEDUMPER_H
#define YFSCC_MACHINEDUMPER_H

#include <fstream>
#include <unordered_set>
#include <vector>

class MachineModule;
class MachineFunction;
class MachineBasicBlock;
class MachineInst;
class MachineOperand;

class MoveInst;
class ClzInst;
class BinaryInst;
class CmpInst;
class CallInst;
class BranchInst;
class RetInst;
class PushInst;
class PopInst;
class StoreInst;
class LoadInst;

class VirtualReg;
class ImmNumber;
class Label;
class MachineReg;

class GlobalVariable;

class MachineDumper {
public:
    using GlobalVarSet = std::vector<const GlobalVariable *>;

    MachineDumper(MachineModule *module, const std::string &file_name);

    ~MachineDumper();

    void dump(const MachineModule *module);

    void dump(const MachineFunction *function);

    void dump(const MachineBasicBlock *basicblock);

    void dump(const MachineInst *inst);

    void dump(const MachineOperand *operand);

    void dump(const MoveInst *inst);

    void dump(const ClzInst *inst);

    void dump(const BinaryInst *inst);

    void dump(const CmpInst *inst);

    void dump(const CallInst *inst);

    void dump(const BranchInst *inst);

    void dump(const RetInst *inst);

    void dump(const PushInst *inst);

    void dump(const PopInst *inst);

    void dump(const StoreInst *inst);

    void dump(const LoadInst *inst);

    void dump(const VirtualReg *operand);

    void dump(const MachineReg *operand);

    void dump(const ImmNumber *operand);

    void dump(const Label *operand);

    void dump() {
        dump(module_);
        dumpGlobals();
    }

    void dumpGlobals();

    std::string getFileName() const {
        return out_file_name_;
    }

private:
    MachineModule *module_;

    std::string out_file_name_;

    std::ofstream fout_;

    GlobalVarSet global_set_;
};


#endif //YFSCC_MACHINEDUMPER_H
