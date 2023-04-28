//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_USER_H
#define YFSCC_USER_H

#include <vector>
#include "Value.h"

class User: public Value {
public:
    explicit User(ValueType type, bool isptr, bool isbool, const std::string &name = "", size_t num = 0):
        Value(type, isptr, isbool, name) , operand_num_(num) {}

    virtual ~User() = default;

    size_t getOperandNum() const {
        return operand_num_;
    }

    Value *getOperand(int idx) const {
        if (idx < operands_.size()) {
            return operands_[idx];
        }
        return nullptr;
    }

    void setOperand(Value *value, int idx) {
        if (idx < operand_num_) {
            operands_[idx] = value;
            value->addUse(this, idx);
        }
    }

    void addOperand(Value *value) {
        operands_.emplace_back(value);
        // value->addUse(this, operand_num_);
        operand_num_++;
    }

protected:
    size_t operand_num_;
    std::vector<Value *> operands_;
};

#endif //YFSCC_USER_H
