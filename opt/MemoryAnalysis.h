//
// Created by 杨丰硕 on 2023/7/3.
//

#ifndef YFSCC_MEMORYANALYSIS_H
#define YFSCC_MEMORYANALYSIS_H

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "PassManager.h"
#include "GlobalAnalysis.h"
#include "CallGraphAnalysis.h"

class Value;
class BasicBlock;
class Instruction;
class StoreInstruction;
class CallGraphAnalysis;

class MemoryAnalysis: public Pass {
public:

    using MemVersionTable = std::unordered_map<Value *, Instruction *>;

    explicit MemoryAnalysis(Module *module): Pass(module), user_analysis_(std::make_unique<UserAnalysis>()), call_graph_analysis_(std::make_unique<CallGraphAnalysis>(module_)) {}

    ~MemoryAnalysis() = default;

protected:

    void runOnFunction() override;

private:

    void visitBasicBlock(BasicBlock *basicblock);

    void visitExtendBasicBlock(BasicBlock *basicblock);

    void removeAndReplace();

    void simplifyStoreSecondaryPtr();

    void allocateTable() {
        if (!memversion_table_.empty()) {
            memversion_table_.emplace_back(memversion_table_.back());
        } else {
            memversion_table_.emplace_back();
        }
    }

    void deallocateTable() {
        memversion_table_.pop_back();
    }

    std::unique_ptr<UserAnalysis> user_analysis_;

    std::unique_ptr<CallGraphAnalysis> call_graph_analysis_;

    std::unordered_map<Value *, Value *> replace_load_insts_;

    std::unordered_set<Instruction *> removes_insts_;

    std::unordered_set<BasicBlock *> visited_bbs_;

    std::list<BasicBlock *> work_list_;

    std::vector<MemVersionTable> memversion_table_;
};


#endif //YFSCC_MEMORYANALYSIS_H
