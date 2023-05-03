//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_INSTRUCTION_H
#define YFSCC_INSTRUCTION_H

#include "User.h"
#include "../common/Types.h"

class Function;

enum InstructionType {
    RetType,
    BrType,
    AddType,
    SubType,
    MulType,
    ModType,
    DivType,
    AndType,
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
    Instruction(InstructionType type, BasicType basic_type, bool isptr, bool isbool, BasicBlock *block, const std::string &name = "");

    virtual ~Instruction();

    BasicBlock *getParent() const {
        return parent_;
    }

    InstructionType getInstType() const {
        return inst_type_;
    }

    BasicType getBasicType() const {
        return basic_type_;
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

private:
};

class UnaryOpInstruction: public Instruction {
public:
    UnaryOpInstruction(InstructionType type, BasicType basic_type, BasicBlock *block, Value *value, const std::string &name = "");

    ~UnaryOpInstruction();

    Value *getValue() const;

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
private:
};

class LoadInstruction: public Instruction {
public:
    LoadInstruction(BasicBlock *block, BasicType basic_type, Value *ptr, BasicType type, const std::string &name = "");

    ~LoadInstruction();

    Value *getPtr() const;

    const std::vector<int32_t> &getArrayDimensionSize() const;

    void setArrayDimension();

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

private:
    bool is_i2f_;
};

class GEPInstruction: public Instruction {
public:

    GEPInstruction(BasicBlock *block, BasicType btype, Value *base, bool ptr_offset, const std::vector<Value *> &indexes, const std::string &name = "");

    GEPInstruction() = default;

    ~GEPInstruction();

    Value *getPtr() const {
        return getOperand(0);
    }

    Value *getOffset() const {
        // if (is_ptr_offset_) {
            return getOperand(1);
        //}
        //return nullptr;
    }

    bool isPtrOffset() const {
        return is_ptr_offset_;
    }

    size_t getIndexSize() const {
        // if (!is_ptr_offset_) {
           return getOperandNum() - 1;
        //}
        // return 0;
    }

    Value *getIndexValue(int idx) const {
        // if (!is_ptr_offset_) {
            return getOperand(idx + 1);
        //}
        // return nullptr;
    }

    void setArrayDimension();

    const std::vector<int32_t> &getArrayDimension() const {
        return array_dimension_numbers_;
    }

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

private:
};

class ZextInstruction: public Instruction {
public:
    ZextInstruction(BasicBlock *block, Value *left, const std::string &name);

    ~ZextInstruction();

    Value *getValue() const {
        return getOperand(0);
    }
private:
};

#endif //YFSCC_INSTRUCTION_H
