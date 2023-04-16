//
// Created by 杨丰硕 on 2023/4/15.
//
#include "Argument.h"

Argument::Argument(int idx, bool is_float, Function *func, const std::string &name):
    Value(ValueType::ArgumentValue, name),
    idx_(idx),
    is_float_(is_float),
    parent_(func) {}

Argument::~Argument() = default;

