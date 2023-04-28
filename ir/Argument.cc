//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Argument.h"

Argument::Argument(BasicType basic_type, Function *func, bool isptr, const std::string &name):
    Value(ValueType::ArgumentValue, false, false, name),
    is_ptr_(isptr),
    basic_type_(basic_type),
    parent_(func) {}

Argument::Argument(BasicType basic_type, const std::vector<int32_t> &dimension, Function *func,
                   bool isptr, const std::string &name):
        Value(ValueType::ArgumentValue, true, false, name),
        is_ptr_(isptr),
        basic_type_(basic_type),
        parent_(func),
        dimensions_(dimension){

}

Argument::~Argument() = default;

