//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_VALUE_H
#define YFSCC_VALUE_H

#include <iostream>
#include <cassert>
#include <list>
#include <unordered_map>
#include <set>
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
    using UserMap = std::set<User *>;

    Value(ValueType vtype, bool isptr, bool isbool, const std::string &name = ""): name_(name), type_(vtype), is_bool_type_(isbool), is_ptr_(isptr) {}

    virtual ~Value() = default;

    Value(const Value& value) = delete;

    Value &operator = (const Value& value) = delete;

    void addUse(User *user, int idx) {
        assert(user);
        user_map_.insert(user);
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

    bool isBool() const {
        return is_bool_type_;
    }

    UserMap &getUserMap() {
        return user_map_;
    }

protected:
    bool is_bool_type_;
    bool is_ptr_;
    UserMap user_map_;
    std::string name_;
    ValueType type_;
};


#endif //YFSCC_VALUE_H
