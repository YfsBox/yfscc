//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_GLOBALVARIABLE_H
#define YFSCC_GLOBALVARIABLE_H

#include <memory>
#include "Constant.h"
#include "Value.h"

class Constant;

class GlobalVariable: public Value {
public:
    using ConstantPtr = std::unique_ptr<Constant>;

    GlobalVariable(bool is_const, const std::string &name):Value(ValueType::GlobalVariableValue, name), is_const_(is_const) {}

    GlobalVariable(bool is_const, int32_t initval, const std::string &name):
            Value(ValueType::GlobalVariableValue, name),
            is_const_(is_const),
            const_init_(std::make_unique<ConstantVar>(initval)) {}

    GlobalVariable(bool is_const, float initval, const std::string &name):
            Value(ValueType::GlobalVariableValue, name),
            is_const_(is_const),
            const_init_(std::make_unique<ConstantVar>(initval)) {}

    GlobalVariable(bool is_const, bool is_float, int dimension, const std::string &name):
            Value(ValueType::GlobalVariableValue, name),
            is_const_(is_const),
            const_init_(std::make_unique<ConstantArray>(is_float, dimension)) {}

    ~GlobalVariable() = default;

    Constant *getConstInit() const {
        return const_init_.get();
    }

    bool isConst() const {
        return is_const_;
    }

private:
    bool is_const_;
    ConstantPtr const_init_;
};


#endif //YFSCC_GLOBALVARIABLE_H
