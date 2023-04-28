//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_ARGUMENT_H
#define YFSCC_ARGUMENT_H

#include <vector>
#include "Value.h"
#include "../common/Types.h"

class Function;

class Argument: public Value {
public:

    static constexpr const int32_t ArrayArgumentNullIdx = -1;

    Argument(BasicType basic_type, Function *func, bool isptr, const std::string &name);

    Argument(BasicType basic_type, const std::vector<int32_t> &dimension, Function *func, bool isptr, const std::string &name);

    ~Argument();

    bool isPtrPtr() const {
        return is_ptr_;
    }

    Function *getParent() const {
        return parent_;
    }

    BasicType getBasicType() const {
        return basic_type_;
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

    const std::vector<int32_t> getDimension() const {
        return dimensions_;
    }

private:
    bool is_ptr_;
    BasicType basic_type_;      // 参数类型
    Function *parent_;
    std::vector<int32_t> dimensions_;
};

#endif //YFSCC_ARGUMENT_H
