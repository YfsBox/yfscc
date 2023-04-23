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

ConstantArray::ConstantArray(bool isfloat, const std::vector<int32_t> &dimensions, const std::string &name):
        Constant(isfloat, name),
        dimension_number_(dimensions) {
    auto dimension_size = dimensions.size();
    dimension_size_number_.resize(dimension_size);
    size_t total_size = 1;

    for (auto dimension: dimension_number_) {
        total_size *= dimension;
    }

    for (int i = 0; i < dimension_size; ++i) {
        dimension_size_number_[i] = total_size / dimension_number_[i];
    }
}

ConstantArray::~ConstantArray() = default;

void ConstantArray::setInitValue(int32_t idx, float value) {
    init_value_map_.insert(std::make_pair(idx, std::make_unique<ConstantVar>(value)));
}

void ConstantArray::setInitValue(int32_t idx, int32_t value) {
    init_value_map_.insert(std::make_pair(idx, std::make_unique<ConstantVar>(value)));
}
