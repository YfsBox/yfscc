//
// Created by 杨丰硕 on 2023/6/14.
//

#ifndef YFSCC_CALLGRAPHANALYSIS_H
#define YFSCC_CALLGRAPHANALYSIS_H

#include <unordered_map>
#include <unordered_set>

class Module;
class Function;

class CallGraphAnalysis {
public:

    using FunctionSet = std::unordered_set<Function *>;

    explicit CallGraphAnalysis(Module *module): module_(module), has_analysis_(false) {}

    ~CallGraphAnalysis() = default;

    bool hasSideEffect(Function *function) {
        return side_effect_functions_.count(function);
    }

    bool isRecursive(Function *function) {
        return recursive_functions_.count(function);
    }

    bool isLibFunction(Function *function) {
        return lib_function_set_.count(function);
    }

    void analysis();

private:

    void buildCallGraph();

    void initForLibFunction();

    void findRecursive();

    bool has_analysis_;

    Module *module_;

    FunctionSet side_effect_functions_;

    FunctionSet recursive_functions_;

    FunctionSet lib_function_set_;

    std::unordered_map<Function *, FunctionSet> caller_sets_;

    std::unordered_map<Function *, FunctionSet> callee_sets_;
};


#endif //YFSCC_CALLGRAPHANALYSIS_H
