//
// Created by 杨丰硕 on 2023/5/2.
//

#ifndef YFSCC_MACHINEOPERAND_H
#define YFSCC_MACHINEOPERAND_H

#include <iostream>

class MachineOperand {
public:
    enum ValueType {
        Undef,
        Float,
        Int,
    };

    enum OperandType {
        VirtualReg,
        ImmNumber,
        Label,
        MachineReg,
    };

    MachineOperand(ValueType value_type, OperandType operand_type);

    virtual ~MachineOperand();

    ValueType getValueType() const {
        return value_type_;
    }

    OperandType getOperandType() const {
        return operand_type_;
    }

protected:
    ValueType value_type_;
    OperandType operand_type_;
};

class VirtualReg: public MachineOperand {
public:
    VirtualReg(int reg_id, ValueType value_type);

    ~VirtualReg() = default;

    int getRegId() const {
        return reg_id_;
    }

private:
    int reg_id_;
};

class ImmNumber: public MachineOperand {
public:
    ImmNumber(int32_t value);

    ImmNumber(float value);

    ~ImmNumber();

    bool isFloat() const {
        return is_float_;
    }

    int32_t getIValue() const {
        return ivalue_;
    }

    float getFValue() const {
        return fvalue_;
    }

private:
    bool is_float_;
    union {
        int32_t ivalue_;
        float fvalue_;
    };
};

class Label: public MachineOperand {
public:
    Label(const std::string &name);

    ~Label();

    std::string getName() const {
        return name_;
    }

private:
    std::string name_;
};

class MachineReg: public MachineOperand {
public:
    enum Reg {
        undef, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
        s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31,
        fp = r11, ip = r12, sp = r13, lr = r14, pc = r15,
    };

    MachineReg(Reg reg);

    ~MachineReg();

    Reg getReg() const {
        return reg_;
    }

    std::string machieReg2RegName() const;

private:
    Reg reg_;
};

#endif //YFSCC_MACHINEOPERAND_H
