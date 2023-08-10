//
// Created by 杨丰硕 on 2023/6/15.
//

#ifndef YFSCC_SVN_H
#define YFSCC_SVN_H

#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"
#include "CallGraphAnalysis.h"

class Instruction;
class BasicBlock;
class Value;

class Svn: public Pass {        //  只保留对call指令的处理
public:

    using ValueName = std::string;

    using ValueNameTable = std::unordered_map<ValueName, Instruction *>;

    explicit Svn(Module *module): Pass(module), callgraph_analysis_(std::make_unique<CallGraphAnalysis>(module_)) {
        pass_name_ = "Svn";
    }

    ~Svn() = default;

protected:

    void runOnFunction() override;

private:

    ValueName getValueName(Instruction *inst);

    static ValueName getOperandName(Value *value);

    void replaceValues();

    void svn(BasicBlock *basicblock);

    void lvn(BasicBlock *basicblock);

    Instruction* lookupOrAdd(const ValueName &value_name, Instruction *inst);

    void allocateTable() {
        if (binary_value_name_table_.empty()) {
            binary_value_name_table_.emplace_back();
        } else {
            binary_value_name_table_.push_back(binary_value_name_table_.back());
        }
    }

    void deallocateTable() {
        binary_value_name_table_.pop_back();
    }

    std::unique_ptr<CallGraphAnalysis> callgraph_analysis_;

    std::unordered_map<Instruction *, Instruction *> replaced_map_;

    std::unordered_set<BasicBlock *> visited_bbs_;

    std::list<BasicBlock *> work_list_;

    std::vector<ValueNameTable> binary_value_name_table_;

};


#endif //YFSCC_SVN_H
