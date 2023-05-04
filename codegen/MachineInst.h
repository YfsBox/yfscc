//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINEINST_H
#define YFSCC_MACHINEINST_H

#include <iostream>
#include <memory>
#include <vector>

class MachineOperand;
class MachineBasicBlock;
class MachineReg;
class Label;

class MachineInst {
public:
    using OperandPtr = MachineOperand *;

    enum ValueType {
        Undef,
        Int,
        Float,
    };

    enum MachineInstType {
        Binary,
        Move,
        Clz,
        Ret,
        Branch,
        Cmp,
        Call,
        Push,
        Pop,
        Load,
        Store,
    };

    MachineInst(MachineInstType mtype, ValueType value_type, MachineBasicBlock *parent);

    virtual ~MachineInst();

    MachineInstType getMachineInstType() const {
        return machine_inst_type_;
    }

    MachineBasicBlock *getParent() const {
        return parent_;
    }

    void setValueType(ValueType vtype) {
        value_type_ = vtype;
    }

protected:

    ValueType getValueType(MachineOperand *operand);

    ValueType value_type_;

    MachineInstType machine_inst_type_;

    MachineBasicBlock *parent_;
};

class MoveInst: public MachineInst {
public:
    enum MoveType {
        I2I,
        F2F,
        L2I,
        H2I,
        F_I,
    };

    MoveInst(MachineBasicBlock *parent, MoveType movtype, MachineOperand *src, MachineOperand *dst);

    MoveInst(MachineBasicBlock *parent, MachineOperand *src, MachineOperand *dst);

    ~MoveInst();

    MachineOperand *getSrc() const {
        return src_;
    }

    MachineOperand *getDst() const {
        return dst_;
    }

    MoveType getMoveType() const {
        return mov_type_;
    }

private:
    MoveType mov_type_;
    OperandPtr dst_;
    OperandPtr src_;
};

class ClzInst: public MachineInst {
public:
    ClzInst(MachineBasicBlock *parent, MachineOperand *dst);

    ~ClzInst();

    MachineOperand *getDst() const {
        return dst_;
    }
    
private:
    OperandPtr dst_;
};

class BinaryInst: public MachineInst {
public:
    enum BinaryOp {
        IAdd,
        ISub,
        IMul,
        IDiv,
        FAdd,
        FSub,
        FMul,
        FDiv,
        Mod,
        ILsl,
        ILsr,
    };

    BinaryInst(MachineBasicBlock *parent, BinaryOp op, MachineOperand *dst, MachineOperand *src);

    ~BinaryInst();

    MachineOperand *getDst() const {
        return dst_;
    }

    MachineOperand *getSrc() const {
        return src_;
    }

    BinaryOp getBinaryOp() const {
        return binary_op_type_;
    }



private:
    BinaryOp binary_op_type_;
    OperandPtr dst_;
    OperandPtr src_;
};

class CmpInst: public MachineInst {
public:
    CmpInst(MachineBasicBlock *parent, MachineOperand *lhs, MachineOperand *rhs);

    ~CmpInst();


private:
    OperandPtr lhs_;
    OperandPtr rhs_;
};

class CallInst: public MachineInst {
public:
    CallInst(MachineBasicBlock *parent, Label *label);

    ~CallInst();

    Label *getCallLabel() const {
        return call_label_;
    }


private:
    Label *call_label_;
};

class BranchInst: public MachineInst {
public:
    enum BranchCond {
        BrEq,
        BrNe,
        BrLe,
        BrGe,
        BrLt,
        BrGt,
    };

    BranchInst(MachineBasicBlock *parent, Label *label);

    ~BranchInst();

    BranchCond getBranchCond() const {
        return br_cond_;
    }

    void setBrCond(BranchCond cond) {
        br_cond_ = cond;
    }

private:
    BranchCond br_cond_;

    Label *br_label_;
};

class RetInst: public MachineInst {
public:
    RetInst(MachineBasicBlock *parent);

    ~RetInst();

private:
};

class PushInst: public MachineInst {
public:
    using RegPtr = MachineReg *;

    using PushRegsList = std::vector<RegPtr>;

    PushInst(MachineBasicBlock *parent);

    ~PushInst() = default;

    void addReg(MachineReg *reg) {
        regs_list_.push_back(reg);
    }

    int getRegsSize() const {
        return regs_list_.size();
    }

    MachineReg *getReg(int idx) const {
        return regs_list_[idx];
    }

private:
    PushRegsList regs_list_;
};

class PopInst: public MachineInst {
public:
    PopInst(MachineBasicBlock *parent);

    ~PopInst() = default;

    void addReg(MachineReg *reg) {
        regs_list_.push_back(reg);
    }

    int getRegsSize() const {
        return regs_list_.size();
    }

    MachineReg *getReg(int idx) const {
        return regs_list_[idx];
    }

private:
    std::vector<MachineReg *> regs_list_;
};

enum MemIndexType {
    PostiveIndex,
    NegativeIndex,
};

class StoreInst: public MachineInst {
public:
    StoreInst(MemIndexType index_type, MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset);

    ~StoreInst();

    MemIndexType getIndexType() const {
        return index_type_;
    }

    MachineOperand *getBase() const {
        return base_;
    }

    MachineOperand *getOffset() const {
        return offset_;
    }

    MachineOperand *getDst() const {
        return dst_;
    }



private:
    MemIndexType index_type_;
    OperandPtr dst_;
    OperandPtr base_;
    OperandPtr offset_;
};

class LoadInst: public MachineInst {
public:
    LoadInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset);

    ~LoadInst();

    MachineOperand *getDst() const {
        return dst_;
    }

    MachineOperand *getBase() const {
        return base_;
    }

    MachineOperand *getOffset() const {
        return offset_;
    }

private:
    MemIndexType index_type_;
    OperandPtr dst_;
    OperandPtr base_;
    OperandPtr offset_;
};

#endif //YFSCC_MACHINEINST_H
