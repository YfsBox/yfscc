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
        Vneg,
        Cvt,
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

    MachineInst::ValueType getValueType() const {
        return value_type_;
    }


    static std::vector<MachineOperand *> getUses(MachineInst *inst);

    static std::vector<MachineOperand *> getDefs(MachineInst *inst);

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

    enum MoveCond {
        MoveNoCond,
        MoveNe,
        MoveEq,
        MoveLt,
        MoveGt,
        MoveGe,
        MoveLe,
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

    void setMoveCond(MoveCond mov_cond) {
        mov_cond_ = mov_cond;
    }

    MoveCond getMoveCond() const {
        return mov_cond_;
    }

private:
    MoveCond mov_cond_;
    MoveType mov_type_;
    OperandPtr dst_;
    OperandPtr src_;
};

class ClzInst: public MachineInst {
public:
    ClzInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *src);

    ~ClzInst();

    MachineOperand *getDst() const {
        return dst_;
    }

    MachineOperand *getSrc() const {
        return src_;
    }
    
private:
    OperandPtr dst_;
    OperandPtr src_;
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
        IAsl,
        IAsr,
        IRsb,
    };

    BinaryInst(MachineBasicBlock *parent, BinaryOp op, MachineOperand *dst, MachineOperand *lhs, MachineOperand *rhs);

    ~BinaryInst();

    MachineOperand *getDst() const {
        return dst_;
    }

    MachineOperand *getLeft() const {
        return lhs_;
    }

    MachineOperand *getRight() const {
        return rhs_;
    }

    BinaryOp getBinaryOp() const {
        return binary_op_type_;
    }


private:
    BinaryOp binary_op_type_;
    OperandPtr dst_;
    OperandPtr lhs_;
    OperandPtr rhs_;
};

class CmpInst: public MachineInst {
public:
    CmpInst(MachineBasicBlock *parent, MachineOperand *lhs = nullptr, MachineOperand *rhs = nullptr);

    ~CmpInst();

    void setLhs(OperandPtr lhs) {
        lhs_ = lhs;
    }

    void setRhs(OperandPtr rhs) {
        rhs_ = rhs;
    }

    OperandPtr getLhs() const {
        return lhs_;
    }

    OperandPtr getRhs() const {
        return rhs_;
    }

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
        BrNoCond,
        BrEq,
        BrNe,
        BrLe,
        BrGe,
        BrLt,
        BrGt,
    };

    enum BranchType {
        B,
        Bl,
        Bx,
    };

    BranchInst(MachineBasicBlock *parent, MachineOperand *operand, BranchCond cond = BrNoCond, BranchType branch_type = B);

    ~BranchInst();

    BranchCond getBranchCond() const {
        return br_cond_;
    }

    void setBrCond(BranchCond cond) {
        br_cond_ = cond;
    }

    BranchType getBranchType() const {
        return br_type_;
    }

    MachineOperand *getOperand() const {
        return br_operand_;
    }

private:
    BranchCond br_cond_;

    BranchType br_type_;

    MachineOperand *br_operand_;
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

    PushInst(MachineBasicBlock *parent, MachineInst::ValueType = Undef);

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
    PopInst(MachineBasicBlock *parent, MachineInst::ValueType value_type = Undef);

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
    StoreInst(MemIndexType index_type, MachineBasicBlock *parent, MachineOperand *value, MachineOperand *base, MachineOperand *offset);

    StoreInst(MachineBasicBlock *parent, MachineOperand *value, MachineOperand *base);

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

    MachineOperand *getValue() const {
        return value_;
    }

private:
    MemIndexType index_type_;
    OperandPtr value_;
    OperandPtr base_;
    OperandPtr offset_;
};

class LoadInst: public MachineInst {
public:
    LoadInst(MachineBasicBlock *parent, MachineOperand *dst, MachineOperand *base, MachineOperand *offset = nullptr);

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

class VnegInst: public MachineInst {
public:
    VnegInst(MachineBasicBlock *parent, OperandPtr dst, OperandPtr src);

    ~VnegInst();

    MachineOperand *getSrc() const {
        return src_;
    }

    MachineOperand *getDst() const {
        return dst_;
    }

private:
    OperandPtr src_;
    OperandPtr dst_;
};

class CvtInst: public MachineInst {
public:
    enum CvtType {
        I2F,
        F2I,
    };

    CvtInst(MachineBasicBlock *parent, CvtType cvt_type, OperandPtr dst, OperandPtr src);

    ~CvtInst() = default;

    MachineOperand *getSrc() const {
        return src_;
    }

    MachineOperand *getDst() const {
        return dst_;
    }

    CvtType getCvtType() const {
        return cvt_type_;
    }

private:
    CvtType cvt_type_;

    OperandPtr src_;

    OperandPtr dst_;
};

#endif //YFSCC_MACHINEINST_H
