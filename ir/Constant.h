//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_CONSTANT_H
#define YFSCC_CONSTANT_H

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "User.h"
#include "../common/Types.h"

class Constant: public User {
public:
    Constant(const std::string &name = "");
    virtual ~Constant();
protected:
};

class ConstantVar: public Constant {
public:

    ConstantVar(float val, const std::string &name = "");

    ConstantVar(int32_t val, const std::string &name = "");

    ~ConstantVar();

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
    using ConstantVarPtr = std::unique_ptr<ConstantVar>;

    ConstantArray(bool isfloat, int dimention, const std::string &name = "");

    ~ConstantArray();

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
            init_var_list_.emplace_back(std::make_unique<ConstantVar>(initval));
        }
    }

    void addInitVar(float initval) {
        if (is_float_) {
            init_var_list_.emplace_back(std::make_unique<ConstantVar>(initval));
        }
    }

private:
    bool is_float_;
    int dimension_size_;
    std::vector<size_t> dimension_number_;
    std::vector<ConstantVarPtr> init_var_list_;
};

#endif //YFSCC_CONSTANT_H
