//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Argument.h"

Argument::Argument(bool is_float, Function *func, const std::string &name):
    Value(ValueType::ArgumentValue, false, name),
    is_float_(is_float),
    parent_(func) {}

Argument::Argument(bool is_float, const std::vector<int32_t> &dimension, Function *func,
                   const std::string &name):
        Value(ValueType::ArgumentValue, true, name),
        is_float_(is_float),
        parent_(func),
        dimensions_(dimension){

}


Argument::~Argument() = default;

