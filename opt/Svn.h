//
// Created by 杨丰硕 on 2023/6/15.
//

#ifndef YFSCC_SVN_H
#define YFSCC_SVN_H

#include <list>
#include <unordered_map>
#include <unordered_set>
#include "PassManager.h"

class Instruction;
class BasicBlock;
class Value;

class Svn: public Pass {
public:

    using ValueName = std::string;

    using ValueNameTable = std::unordered_map<ValueName, Instruction *>;

    explicit Svn(Module *module): Pass(module) {}

    ~Svn() = default;

protected:

    void runOnFunction() override;

private:

    static ValueName getValueName(Instruction *inst);

    static ValueName getOperandName(Value *value);

    void replaceValues();

    void svn(BasicBlock *basicblock);

    void lvn(BasicBlock *basicblock);

    Instruction* lookupOrAdd(const ValueName &value_name, Instruction *inst);

    void allocateTable() {
        binary_value_name_table_.emplace_back();
    }

    void deallocateTable() {
        binary_value_name_table_.pop_back();
    }

    std::unordered_map<Instruction *, Instruction *> replaced_map_;

    std::unordered_set<BasicBlock *> visited_bbs_;

    std::list<BasicBlock *> work_list_;

    std::vector<ValueNameTable> binary_value_name_table_;

};


#endif //YFSCC_SVN_H
