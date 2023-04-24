//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_VALUE_H
#define YFSCC_VALUE_H

#include <iostream>
#include <list>
#include <unordered_map>
#include "Use.h"

enum ValueType {
    InstructionValue,
    BasicBlockValue,
    FunctionValue,
    ConstantValue,
    GlobalVariableValue,
    ArgumentValue,
};

class Value {
public:
    using UserMap = std::unordered_map<User *, Use>;

    Value(ValueType vtype, bool isptr, const std::string &name = ""): name_(name), type_(vtype), is_ptr_(isptr) {}

    virtual ~Value() = default;

    Value(const Value& value) = delete;

    Value &operator = (const Value& value) = delete;

    void addUse(User *user, int idx) {
        user_map_[user] = Use(user, idx);
    }

    void delUse(User *user) {
        user_map_.erase(user);
    }

    std::string getName() const {
        return name_;
    }

    ValueType getValueType() const {
        return type_;
    }

    size_t getUseSize() const {
        return user_map_.size();
    }

    bool isPtr() const {
        return is_ptr_;
    }

protected:
    bool is_ptr_;
    UserMap user_map_;
    std::string name_;
    ValueType type_;
};


#endif //YFSCC_VALUE_H
