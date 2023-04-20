//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_ARGUMENT_H
#define YFSCC_ARGUMENT_H

#include <vector>
#include "Value.h"

class Function;

class Argument: public Value {
public:
    Argument(bool is_float, Function *func, const std::string &name);

    Argument(bool is_float, const std::vector<int32_t> &dimension, Function *func, const std::string &name);

    ~Argument();

    Function *getParent() const {
        return parent_;
    }

    bool isFloat() const {
        return is_float_;
    }

    bool isArray() const {
        return !dimensions_.empty();
    }

    size_t getArraySize() const {
        return dimensions_.size();
    }

    int32_t getDimensionByIdx(int idx) const {
        return dimensions_[idx];
    }

private:
    bool is_float_;      // 参数类型
    Function *parent_;
    std::vector<int32_t> dimensions_;
};

#endif //YFSCC_ARGUMENT_H
