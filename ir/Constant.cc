//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Constant.h"

Constant::Constant(bool isfloat, const std::string &name):
    User(ConstantValue, false, name),
    is_float_(isfloat){

}

Constant::~Constant() = default;

BasicType Constant::getBasicType() const {
    return is_float_ ? BasicType::FLOAT_BTYPE : BasicType::INT_BTYPE;
}

ConstantVar::ConstantVar(float val, const std::string &name):
        Constant(true, name), fval_(val) {}


ConstantVar::ConstantVar(int32_t val, const std::string &name):
        Constant(false, name), ival_(val) {}

ConstantVar::~ConstantVar() = default;

ConstantArray::ConstantArray(bool isfloat, int dimention, const std::string &name):
        Constant(isfloat, name),
        dimension_size_(dimention) {
    dimension_number_.resize(dimension_size_);
    init_var_list_.resize(dimension_size_);
}

ConstantArray::~ConstantArray() = default;

