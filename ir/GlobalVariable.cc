//
// Created by 杨丰硕 on 2023/4/15.
//
#include "GlobalVariable.h"

GlobalVariable::GlobalVariable(bool is_const, const std::string &name):
        Value(ValueType::GlobalVariableValue, true, name), is_const_(is_const) {

}

GlobalVariable::GlobalVariable(bool is_const, float initval, const std::string &name):
        Value(ValueType::GlobalVariableValue, true, name),
        is_const_(is_const),
        const_init_(std::make_unique<ConstantVar>(initval)) {

}

GlobalVariable::GlobalVariable(bool is_const, int32_t initval, const std::string &name):
        Value(ValueType::GlobalVariableValue, true, name),
        is_const_(is_const),
        const_init_(std::make_unique<ConstantVar>(initval)) {

}

GlobalVariable::GlobalVariable(bool is_const, bool is_float, Value *const_array, const std::string &name):
        Value(ValueType::GlobalVariableValue, true, name),
        is_const_(is_float){
    auto constarray = dynamic_cast<ConstantArray *>(const_array);
    assert(constarray);
    const_init_ = std::make_unique<Constant>(constarray);
}
GlobalVariable::~GlobalVariable() = default;
