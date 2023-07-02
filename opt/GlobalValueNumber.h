//
// Created by 杨丰硕 on 2023/6/28.
//

#ifndef YFSCC_GLOBALVALUENUMBER_H
#define YFSCC_GLOBALVALUENUMBER_H

#include "PassManager.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>

class Value;
class BasicBlock;
class UserAnalysis;

class GlobalValueNumber: public Pass {
public:

    explicit GlobalValueNumber(Module *module);

    ~GlobalValueNumber() = default;

protected:

    void runOnFunction() override;

private:

    Value *lookupOrAdd(const std::string &value_number, Value *value);

    std::string getValueNumber(const Value *value);

    std::string getOperandNumber(const Value *operand);

    void replaceValues();

    void numberValue(Value *value);

    std::unique_ptr<UserAnalysis> user_analysis_;

    std::unordered_map<std::string, Value *> value_number_map_;

    std::unordered_map<Value *, Value *> replace_values_map_;

    std::unordered_set<Value *> visited_set_;

};


#endif //YFSCC_GLOBALVALUENUMBER_H
