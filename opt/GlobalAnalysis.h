//
// Created by 杨丰硕 on 2023/6/15.
//

#ifndef YFSCC_GLOBALANALYSIS_H
#define YFSCC_GLOBALANALYSIS_H

#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"


class GlobalVariable;
class Function;
class Module;
class Instruction;
class Value;

class GlobalAnalysis: public Pass {
public:
    using GlobalVarSet = std::unordered_set<GlobalVariable *>;

    explicit GlobalAnalysis(Module *module);

    ~GlobalAnalysis() = default;

protected:

    void runOnFunction() override;

private:

    void initWorkList();

    void findGlobalOwnerByOneFunc();

    bool has_init_;

    GlobalVarSet work_global_list_;

    std::unordered_map<Function *, GlobalVarSet> function_global_map_;
};

class UserAnalysis {
public:
    using UserInsts = std::unordered_set<Instruction *>;

    explicit UserAnalysis(): curr_func_(nullptr) {}

    ~UserAnalysis() = default;

    UserInsts &getUserInsts(Value *value);

    void analysis(Function *function);

    void replaceAllUseWith(Value *value, Value *replaced_value);

private:

    Function *curr_func_;

    std::unordered_map<Value *, UserInsts> user_map_;

};


#endif //YFSCC_GLOBALANALYSIS_H
