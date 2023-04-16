//
// Created by 杨丰硕 on 2023/4/15.
//
#include "GlobalVariable.h"

GlobalVariable::GlobalVariable(bool is_const, const std::string &name):
Value(ValueType::GlobalVariableValue, name), is_const_(is_const) {}

GlobalVariable::GlobalVariable(bool is_const, float initval, const std::string &name):
Value(ValueType::GlobalVariableValue, name),
is_const_(is_const),
const_init_(std::make_unique<ConstantVar>(initval)) {}

GlobalVariable::GlobalVariable(bool is_const, int32_t initval, const std::string &name):
Value(ValueType::GlobalVariableValue, name),
is_const_(is_const),
const_init_(std::make_unique<ConstantVar>(initval)) {}

GlobalVariable::GlobalVariable(bool is_const, bool is_float, int dimension, const std::string &name):
Value(ValueType::GlobalVariableValue, name),
is_const_(is_const),
const_init_(std::make_unique<ConstantArray>(is_float, dimension)) {}

GlobalVariable::~GlobalVariable() = default;


