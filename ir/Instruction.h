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
};

class BasicBlock;

class Instruction: public User {
public:
    Instruction(InstructionType type, BasicBlock *block, const std::string &name = "");

    virtual ~Instruction();

    BasicBlock *getParent() const {
        return parent_;
    }

    InstructionType getInstType() const {
        return inst_type_;
    }

protected:
    InstructionType inst_type_;
    BasicBlock *parent_;
};

class BinaryOpInstruction: public Instruction {
public:
    BinaryOpInstruction(InstructionType type, BasicBlock *block, Value *left, Value *right, const std::string &name = "");

    ~BinaryOpInstruction();

    Value *getLeft() const;

    Value *getRight() const;

private:
};

class UnaryOpInstruction: public Instruction {
public:
    UnaryOpInstruction(InstructionType type, BasicBlock *block, Value *value, const std::string &name = "");

    ~UnaryOpInstruction();

    Value *getValue() const;

private:
};

class StoreInstruction: public Instruction {
public:
    StoreInstruction(BasicBlock *block, Value *value, Value *ptr, const std::string &name = "");

    ~StoreInstruction();

    Value *getValue() const;

    Value *getPtr() const;
private:
};

class LoadInstruction: public Instruction {
public:
    LoadInstruction(BasicBlock *block, Value *ptr, BasicType type, const std::string &name = "");

    ~LoadInstruction();

    Value *getPtr() const;
private:
    BasicType value_type_;
};

class AllocaInstruction: public Instruction {
public:
    AllocaInstruction(BasicBlock *block, BasicType type, const std::string &name = "");

    AllocaInstruction(BasicBlock *block, BasicType type, size_t array_size, const std::string &name = "");

    ~AllocaInstruction();

    bool isArray() const {
        return is_array_;
    }

    size_t getValueSize() {
        return value_size_;
    }

    size_t getSizeof() {
        return 4 * value_size_;
    }

    BasicType getValueType() const {
        return value_type_;
    }

private:
    bool is_array_;
    size_t value_size_;
    BasicType value_type_;
};

class RetInstruction: public Instruction {
public:
    RetInstruction(BasicBlock *block, const std::string &name);

    RetInstruction(BasicBlock *block, const std::string &name, Value *value);

    ~RetInstruction();

    Value *getRetValue() const {
        return getOperand(0);
    }

    bool isRetVoid() const {
        return getOperandNum() == 0;
    }
private:
};

class BranchInstruction: public Instruction {
public:
    BranchInstruction(BasicBlock *block, Value *cond, Value *true_label, Value *false_label, const std::string &name);

    BranchInstruction(BasicBlock *block, Value *label, const std::string &name);

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

    PhiInstruction(BasicBlock *block,const std::vector<Value *> &values,const std::vector<BasicBlock *> &bbs, const std::string &name);

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

#endif //YFSCC_INSTRUCTION_H
