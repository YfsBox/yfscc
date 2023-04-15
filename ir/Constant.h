//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_CONSTANT_H
#define YFSCC_CONSTANT_H

#include <iostream>
#include <cassert>
#include <vector>
#include "User.h"
#include "../common/Types.h"

class Constant: public User {
public:
    Constant(const std::string &name = ""): User(ConstantValue, name) {}
    virtual ~Constant() = default;
protected:
};

class ConstantVar: public Constant {
public:

    static ConstantVar *CreateZeroIntConst();

    static ConstantVar *CreateZeroFloatConst();

    ConstantVar(float val, const std::string &name = ""): Constant(name), is_float_(true), fval_(val) {}

    ConstantVar(int32_t val, const std::string &name = ""): Constant(name), is_float_(false), ival_(val) {}

    ~ConstantVar() = default;

    bool isFloat() const {
        return is_float_;
    }

    int32_t getIValue() const {
        return ival_;
    }

    float getFValue() const {
        return fval_;
    }

private:
    bool is_float_;
    union {
        int32_t ival_;
        float fval_;
    };
};

class ConstantArray: public Constant {
public:
    explicit ConstantArray(bool isfloat, int dimention, const std::string &name):
            Constant(name),
            is_float_(isfloat),
            dimension_size_(dimention) {

    }

    ~ConstantArray() = default;

    size_t getDimensionSize() const {
        return dimension_size_;
    }

    void setDimensionNumber(int idx, size_t size) {
        if (idx < dimension_size_) {
            dimension_number_[idx] = size;
        }
    }

    void addInitVar(int32_t initval) {
        if (!is_float_) {
            init_var_list_.emplace_back(initval);
        }
    }

    void addInitVar(float initval) {
        if (is_float_) {
            init_var_list_.emplace_back(initval);
        }
    }

private:
    bool is_float_;
    int dimension_size_;
    std::vector<size_t> dimension_number_;
    std::vector<ConstantVar> init_var_list_;
};

#endif //YFSCC_CONSTANT_H
