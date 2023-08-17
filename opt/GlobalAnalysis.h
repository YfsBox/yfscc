//
// Created by 杨丰硕 on 2023/6/15.
//

#ifndef YFSCC_GLOBALANALYSIS_H
#define YFSCC_GLOBALANALYSIS_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include "PassManager.h"
#include "../ir/Instruction.h"


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
// 基于ssa以及gvn，但是必须用于split gep之前
class ArrayAnalysis {
public:

    struct ArrayInfo {

        Value *array_base_;

        std::set<CallInstruction *> memset_call_insts_;

        std::set<GEPInstruction *> const_index_gep_insts_;

        std::set<StoreInstruction *>  store_insts_;     // 与上面的GEP相关联的store指令的集合, 不仅仅包含了const的GEP

        std::set<LoadInstruction *> load_insts_;

    };

    using ArrayInfoUptr = std::unique_ptr<ArrayInfo>;

    using ArrayInfoMap = std::unordered_map<Value *, ArrayInfoUptr>;

    explicit ArrayAnalysis();

    ~ArrayAnalysis() = default;

    void analysis(Function *function);

    ArrayInfoMap &getArrayInfoMap() {
        return array_info_map_;
    }

    ArrayInfo *getArrayInfo(Value *value) {
        return array_info_map_[value].get();
    }

private:

    Function *curr_func_;

    ArrayInfoMap array_info_map_;         // key有可能是一个alloca数组的指令，也有可能是一个Global

    std::unordered_map<Value *, Value *> gep_owner_array_map_;

};


#endif //YFSCC_GLOBALANALYSIS_H
