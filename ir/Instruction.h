//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_INSTRUCTION_H
#define YFSCC_INSTRUCTION_H

#include "User.h"
#include "../common/Types.h"

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

#endif //YFSCC_INSTRUCTION_H
