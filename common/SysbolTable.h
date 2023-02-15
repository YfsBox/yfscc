//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SYSBOLTABLE_H
#define YFSCC_SYSBOLTABLE_H

#include <iostream>
#include <list>

template<class DATA>
class SymbolEntry {
public:
    SymbolEntry(const std::string &name, DATA *data): name_(name), data_(data) {}
    ~SymbolEntry() = default;
    std::string getName() const {
        return name_;
    }
    DATA *getData() const {
        return data_;
    }
private:
    std::string name_;
    DATA *data_;
};


template<class DATA>
class SymbolTable {
public:
    using Entry = SymbolEntry<DATA>;
    using SymbolScope = std::list<Entry>;

    SymbolTable() = default;

    ~SymbolTable() = default;

    DATA *lookupFromCurrScope(const std::string &name) {
        if (scope_lists_.empty()) {
            return nullptr;
        }
        const auto& curr_scope = scope_lists_.front();

        for (auto &entry : curr_scope) {
            if (entry.getName() == name) {
                return entry.getData();
            }
        }

        return nullptr;
    }

    DATA *lookupFromAll(const std::string &name) {
        for (auto &scope : scope_lists_) {
            for (auto &entry : scope) {
                if (entry.getName() == name) {
                    return entry.getData();
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

private:
    std::list<SymbolScope> scope_lists_;
};

#endif //YFSCC_SYSBOLTABLE_H
