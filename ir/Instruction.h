//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_INSTRUCTION_H
#define YFSCC_INSTRUCTION_H

#include "User.h"
#include "Function.h"
#include "../common/Types.h"


enum InstructionType {
    RetType,
    BrType,
    AddType,
    SubType,
    MulType,
    ModType,
    DivType,
    AndType,
    LshrType,
    RshrType,
    OrType,
    XorType,
    AllocaType,
    LoadType,
    StoreType,
    CallType,
    SetCondType,
    PhiType,
    CastType,
    GEPType,
    NegType,
    NotType,
    MemSetType,
    ZextType,
};

class BasicBlock;

class Instruction: public User {
public:
    using OperandSet = std::unordered_set<Value *>;

    explicit Instruction() = default;

    Instruction(InstructionType type, BasicType basic_type, bool isptr, bool isbool, BasicBlock *block, const std::string &name = "");

    virtual ~Instruction();

    BasicBlock *getParent() const {
        return parent_;
    }

    void setParent(BasicBlock *parent) {
        parent_ = parent;
    }

    InstructionType getInstType() const {
        return inst_type_;
    }

    BasicType getBasicType() const {
        return basic_type_;
    }

    virtual OperandSet getUses() = 0;

    virtual OperandSet getDefs() {
        return {this};
    }

    void replaceAllUseWith(Value *value);

    void setInstType(InstructionType inst_type) {
        inst_type_ = inst_type;
    }

protected:
    BasicType basic_type_;
    InstructionType inst_type_;
    BasicBlock *parent_;
};

class BinaryOpInstruction: public Instruction {
public:
    BinaryOpInstruction(InstructionType type, BasicType basic_type, BasicBlock *block, Value *left, Value *right, const std::string &name = "");

    ~BinaryOpInstruction();

    Value *getLeft() const;

    Value *getRight() const;

    void replaceLeft(Value *value);

    void replaceRight(Value *value);

    void swapLeftAndRight();

    OperandSet getUses() override;

private:
};

class UnaryOpInstruction: public Instruction {
public:
    UnaryOpInstruction(InstructionType type, BasicType basic_type, BasicBlock *block, Value *value, const std::string &name = "");

    ~UnaryOpInstruction();

    Value *getValue() const;

    OperandSet getUses() override;

private:
};

class StoreInstruction: public Instruction {
public:
    StoreInstruction(BasicBlock *block, BasicType basic_type, Value *value, Value *ptr, const std::string &name = "");

    ~StoreInstruction();

    Value *getValue() const {
        return getOperand(0);
    }

    Value *getPtr() const {
        return getOperand(1);
    }

    OperandSet getUses() override;

    OperandSet getDefs() override {
        return {};
    }

private:
};

class LoadInstruction: public Instruction {
public:
    LoadInstruction(BasicBlock *block, BasicType basic_type, Value *ptr, BasicType type, const std::string &name = "");

    ~LoadInstruction();

    Value *getPtr() const;

    bool isFromSecondaryPtr() const;

    const std::vector<int32_t> &getArrayDimensionSize() const;

    void setArrayDimension();

    OperandSet getUses() override;

private:
    std::vector<int32_t> array_dimension_number_;
};

class AllocaInstruction: public Instruction {
public:
    AllocaInstruction(BasicBlock *block, BasicType type, bool isptrptr, const std::string &name = "");

    AllocaInstruction(BasicBlock *block, BasicType type, bool isptrptr, const std::vector<int32_t> &dimension, const std::string &name = "");

    ~AllocaInstruction();

    bool isArray() const {
        return !array_dimension_size_.empty();
    }

    bool isPtrPtr() const {
        return is_ptr_ptr_;
    }

    size_t getValueSize() {
        size_t value_size = 1;
        for (auto dimension: array_dimension_size_) {
            value_size *= dimension;
        }
        return value_size;
    }

    size_t getDimensionSize() const {
        return array_dimension_size_.size();
    }

    int32_t getDimensionSize(int idx) const {
        return array_dimension_size_[idx];
    }

    const std::vector<int32_t> &getArrayDimensionSize() const {
        return array_dimension_size_;
    }

    OperandSet getUses() override;

private:
    bool is_ptr_ptr_;
    std::vector<int32_t> array_dimension_size_;
};

class RetInstruction: public Instruction {
public:
    RetInstruction(BasicBlock *block, const std::string &name = "");

    RetInstruction(BasicBlock *block, BasicType basic_type, Value *value, const std::string &name = "");

    ~RetInstruction();

    Value *getRetValue() const {
        return getOperand(0);
    }

    BasicType getRetType() const {
        return getBasicType();
    }

    bool isRetVoid() const {
        return getOperandNum() == 0;
    }

    OperandSet getUses() override;

    OperandSet getDefs() override {
        return {};
    }

private:

};

class BranchInstruction: public Instruction {
public:
    BranchInstruction(BasicBlock *block, Value *cond, Value *true_label, Value *false_label, const std::string &name = "");

    BranchInstruction(BasicBlock *block, Value *label, const std::string &name = "");

    ~BranchInstruction();

    bool isCondBranch() const {
        return is_cond_;
    }

    Value *getCond() const {
        if (!is_cond_) {
            return nullptr;
        }
        return getOperand(0);
    }

    Value *getTrueLabel() const {
        if (!is_cond_) {
            return nullptr;
        }
        return getOperand(1);
    }

    Value *getFalseLabel() const {
        if (!is_cond_) {
            return nullptr;
        }
        return getOperand(2);
    }

    Value *getLabel() const {
        if (is_cond_) {
            return nullptr;
        }
        return getOperand(0);
    }

    void setTrueLabel(Value *value) {
        if (getOperandNum() >= 3) {
            setOperand(value, 1);
        }
    }

    void setFalseLabel(Value *value) {
        if (getOperandNum() >= 3) {
            setOperand(value, 2);
        }
    }

    OperandSet getUses() override;

    OperandSet getDefs() override {
        return {};
    }

private:
    bool is_cond_;
};

class CallInstruction: public Instruction {
public:
    CallInstruction(BasicBlock *block, Function *function, std::vector<Value *> actuals, const std::string &name);

    ~CallInstruction();

    size_t getActualSize() const {
        return getOperandNum();
    }

    Function *getFunction() const {
        return function_;
    }

    Value *getActual(int idx) const {
        return getOperand(idx);
    }

    OperandSet getUses() override;

    OperandSet getDefs() override {
        if (function_->getRetType() == FLOAT_BTYPE || function_->getRetType() == INT_BTYPE) {
            return {this};
        }
        return {};
    }

private:
    Function *function_;
};

class SetCondInstruction: public Instruction {
public:
    enum CmpCondType {
        SetEQ,
        SetNE,
        SetLE,
        SetGE,
        SetLT,
        SetGT,
    };

    SetCondInstruction(BasicBlock *block, CmpCondType cmptype, bool is_float, Value *left, Value *right, const std::string &name);

    ~SetCondInstruction();

    bool isFloatCmp() const {
        return is_float_;
    }

    CmpCondType getCmpType() const {
        return cmp_cond_type_;
    }

    Value *getLeft() const {
        return getOperand(0);
    }

    Value *getRight() const {
        return getOperand(1);
    }

    OperandSet getUses() override;

private:
    bool is_float_;

    CmpCondType cmp_cond_type_;
};

class PhiInstruction: public Instruction {
public:
    using ValueBlockPair = std::pair<Value *, BasicBlock *>;

    PhiInstruction(BasicBlock *block, BasicType basic_type , const std::vector<Value *> &values,const std::vector<BasicBlock *> &bbs, const std::string &name);

    ~PhiInstruction();

    ValueBlockPair getValueBlock(int idx) const;

    Value *getValue(int idx) const {
        return getOperand(idx);
    }

    BasicBlock *getBasicBlock(int idx) const;

    size_t getSize() const {
        return getOperandNum() / 2;
    }

    OperandSet getUses() override;

private:
};


class CastInstruction: public Instruction {
public:
    CastInstruction(BasicBlock *block, bool is_i2f, Value *value, const std::string &name);

    ~CastInstruction();

    bool isI2F() const {
        return is_i2f_;
    }

    bool isF2I() const {
        return !is_i2f_;
    }

    Value *getValue() const {
        return getOperand(0);
    }

    OperandSet getUses() override;

private:
    bool is_i2f_;
};

class GEPInstruction: public Instruction {
public:

    GEPInstruction(BasicBlock *block, BasicType btype, Value *base, bool ptr_offset, const std::vector<Value *> &indexes, const std::string &name = "");

    GEPInstruction(BasicBlock *block, BasicType btype, const std::vector<Value *> &operands, const std::vector<int32_t> &array_dimensions, bool ptr_offset, const std::string &name = "");

    explicit GEPInstruction() = default;

    ~GEPInstruction();

    Value *getPtr() const {
        return getOperand(0);
    }

    Value *getOffset() const {
        return getOperand(1);
    }

    bool isPtrOffset() const {
        return is_ptr_offset_;
    }

    size_t getIndexSize() const {
        return getOperandNum() - 1;
    }

    Value *getIndexValue(int idx) const {
        return getOperand(idx + 1);
    }

    void setArrayDimension();

    const std::vector<int32_t> &getArrayDimension() const {
        return array_dimension_numbers_;
    }

    OperandSet getUses() override;

private:
    bool is_ptr_offset_;       // 如果是指向数组的指针或者普通指针

    std::vector<int32_t> array_dimension_numbers_;
};

class MemSetInstruction: public Instruction {
public:
    MemSetInstruction(BasicBlock *block, BasicType btype, Value *base, Value *size, Value *value);

    ~MemSetInstruction();

    Value *getBase() const {
        return getOperand(0);
    }

    Value *getSize() const {
        return getOperand(1);
    }

    Value *getValue() const {
        return getOperand(2);
    }

    OperandSet getUses() override;

    OperandSet getDefs() override {
        return {};
    }

private:
};

class ZextInstruction: public Instruction {
public:
    ZextInstruction(BasicBlock *block, Value *left, const std::string &name);

    ~ZextInstruction();

    Value *getValue() const {
        return getOperand(0);
    }

    OperandSet getUses() override;

private:
};

#endif //YFSCC_INSTRUCTION_H
