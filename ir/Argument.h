//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_ARGUMENT_H
#define YFSCC_ARGUMENT_H

#include "Value.h"

class Function;

class Argument: public Value {
public:
    Argument(int idx, bool is_float, Function *func, const std::string &name):
    Value(ValueType::ArgumentValue, name), idx_(idx), is_float_(is_float), parent_(func) {}

    ~Argument() = default;

    Function *getParent() const {
        return parent_;
    }

    bool isFloat() const {
        return is_float_;
    }

    int getIdx() const {
        return idx_;
    }

private:
    int idx_;
    bool is_float_;      // 参数类型
    Function *parent_;
};

#endif //YFSCC_ARGUMENT_H
