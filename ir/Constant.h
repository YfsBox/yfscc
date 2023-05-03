//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_CONSTANT_H
#define YFSCC_CONSTANT_H

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <map>
#include "User.h"
#include "../common/Types.h"

class Constant: public User {
public:
    Constant(bool isfloat, const std::string &name = "");
    virtual ~Constant();
    BasicType getBasicType() const;
protected:
    bool is_float_;
};

class ConstantVar: public Constant {
public:

    ConstantVar(float val, const std::string &name = "");

    ConstantVar(int32_t val, const std::string &name = "");

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
    union {
        int32_t ival_;
        float fval_;
    };
};

class ConstantArray: public Constant {
public:
    using ConstantVarPtr = std::unique_ptr<ConstantVar>;

    using InitValueMap = std::map<int32_t, ConstantVarPtr>;

    ConstantArray(bool isfloat, const std::vector<int32_t>& dimensions, const std::string &name = "");

    ~ConstantArray() = default;

    void setInitValue(int32_t idx, int32_t value);

    void setInitValue(int32_t idx, float value);

    const InitValueMap &getInitValueMap() const {
        return init_value_map_;
    }

    size_t getArrayLen() const {
        return dimension_size_number_[0] * dimension_number_[0];
    }

    const std::vector<int32_t> &getDimensionNumbers() const {
        return dimension_number_;
    }

    const std::vector<int32_t> &getDimensionSizeNumbers() const {
        return dimension_size_number_;
    }

private:
    std::vector<int32_t> dimension_number_;
    std::vector<int32_t> dimension_size_number_;
    InitValueMap init_value_map_;
};

#endif //YFSCC_CONSTANT_H
