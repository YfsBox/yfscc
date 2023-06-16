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


#endif //YFSCC_GLOBALANALYSIS_H
