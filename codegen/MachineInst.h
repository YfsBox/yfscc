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
    using OperandPtr = std::unique_ptr<MachineOperand>;

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

    virtual std::string dump() = 0;

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
    MoveInst(MachineBasicBlock *parent, MachineOperand *src, MachineOperand *dst);

    ~MoveInst();

    MachineOperand *getSrc() const {
        return src_.get();
    }

    MachineOperand *getDst() const {
        return dst_.get();
    }

    std::string dump() override;
private:
    OperandPtr dst_;
    OperandPtr src_;
};

class ClzInst: public MachineInst {
public:
    ClzInst(MachineBasicBlock *parent, MachineOperand *dst);

    ~ClzInst();

    MachineOperand *getDst() const {
        return dst_.get();
    }

    std::string dump() override;
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
        return dst_.get();
    }

    MachineOperand *getSrc() const {
        return src_.get();
    }

    BinaryOp getBinaryOp() const {
        return binary_op_type_;
    }

    std::string dump() override;

private:
    BinaryOp binary_op_type_;
    OperandPtr dst_;
    OperandPtr src_;
};

class CmpInst: public MachineInst {
public:
    CmpInst(MachineBasicBlock *parent, MachineOperand *lhs, MachineOperand *rhs);

    ~CmpInst();

    std::string dump() override;
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

    std::string dump() override;
private:
    Label *call_label_;
};

class BranchInst: public MachineInst {
public:
    BranchInst(MachineBasicBlock *parent, Label *label);

    ~BranchInst();

    std::string dump() override;
private:
    Label *br_label_;
};

class RetInst: public MachineInst {
public:
    RetInst(MachineBasicBlock *parent);

    ~RetInst();

    std::string dump() override;
private:
};

class PushInst: public MachineInst {
public:
    using RegPtr = std::unique_ptr<MachineReg>;

    using PushRegsList = std::vector<RegPtr>;

    PushInst(MachineBasicBlock *parent);

    PushInst() = default;

    std::string dump() override;

    void addReg(MachineReg *reg);

    int getRegsSize() const {
        return regs_list_.size();
    }

    MachineReg *getReg(int idx) const {
        return regs_list_[idx].get();
    }

private:
    PushRegsList regs_list_;
};

class PopInst: public MachineInst {
public:
    PopInst(MachineBasicBlock *parent);

    ~PopInst() = default;

    std::string dump() override;
private:

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
        return base_.get();
    }

    MachineOperand *getOffset() const {
        return offset_.get();
    }

    MachineOperand *getDst() const {
        return dst_.get();
    }

    std::string dump() override;

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
        return dst_.get();
    }

    MachineOperand *getBase() const {
        return base_.get();
    }

    MachineOperand *getOffset() const {
        return offset_.get();
    }

    std::string dump() override;
private:
    MemIndexType index_type_;
    OperandPtr dst_;
    OperandPtr base_;
    OperandPtr offset_;
};

#endif //YFSCC_MACHINEINST_H
