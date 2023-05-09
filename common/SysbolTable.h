//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SYSBOLTABLE_H
#define YFSCC_SYSBOLTABLE_H

#include <iostream>
#include <list>
#include "Types.h"
#include "Ast.h"
#include "Utils.h"

template<class DATA>
class SymbolTable {
public:
    using SymbolScope = std::list<DATA>;

    SymbolTable() = default;

    ~SymbolTable() = default;

    void addIdent(const DATA &entry) {      // 在当前作用域加入新的identifier
        scope_lists_.front().push_back(entry);
    }

    DATA *lookupFromCurrScope(const std::string &name) {
        if (scope_lists_.empty()) {
            return nullptr;
        }
        const auto& curr_scope = scope_lists_.front();

        for (auto &entry : curr_scope) {
            if (entry.getName() == name) {
                return const_cast<DATA*>(&entry);
            }
        }

        return nullptr;
    }

    DATA *lookupFromAll(const std::string &name) {
        for (auto &scope : scope_lists_) {
            for (auto &entry : scope) {
                if (entry.getName() == name) {
                    return &entry;
                }
            }
        }
        return nullptr;
    }

    void enterScope() {
        scope_lists_.push_front({});
    }

    void exitScope() {
        scope_lists_.pop_front();
    }

    bool isInGlobalScope() const {
        return scope_lists_.size() == 1;
    }

    void dump(std::ostream &out) const {
        out << "#DUMP CURR SYMBOL TABLE:\n";
        size_t curr_depth = 1;
        // 倒序遍历
        for (auto rit = scope_lists_.rbegin(); rit != scope_lists_.rend(); ++rit) {
            dumpPrefix(out, curr_depth);
            out << "{\n";
            for (auto &data : *rit) {
                data.dump(out, curr_depth + 1);
                out << '\n';
            }
            dumpPrefix(out, curr_depth);
            out << "}\n";
            curr_depth++;
        }
    }

private:
    std::list<SymbolScope> scope_lists_;
};

#endif //YFSCC_SYSBOLTABLE_H
