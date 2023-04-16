//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Constant.h"

Constant::Constant(const std::string &name):
User(ConstantValue, name) {}

Constant::~Constant() = default;

ConstantVar::ConstantVar(float val, const std::string &name):
        Constant(name), is_float_(true), fval_(val) {}


ConstantVar::ConstantVar(int32_t val, const std::string &name):
        Constant(name), is_float_(false), ival_(val) {}

ConstantVar::~ConstantVar() = default;

ConstantArray::ConstantArray(bool isfloat, int dimention, const std::string &name):
        Constant(name),
        is_float_(isfloat),
        dimension_size_(dimention) {
    dimension_number_.resize(dimension_size_);
    init_var_list_.resize(dimension_size_);
}

ConstantArray::~ConstantArray() = default;

