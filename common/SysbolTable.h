//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SYSBOLTABLE_H
#define YFSCC_SYSBOLTABLE_H

#include <iostream>
#include <list>
#include "Types.h"
#include "Ast.h"

class SymbolEntry {
public:
    SymbolEntry(BasicType type, Identifier *id, bool is_const): is_const_(is_const),
    type_(type), id_(id) {}

    ~SymbolEntry() = default;

    std::string getName() const {
        return id_->getId();
    }

    BasicType getType() const {
        return type_;
    }

    bool isConst() const {
        return is_const_;
    }

    bool isArray() const {
        return id_->getDimensionSize() != 0;
    }

    Identifier *getId() const {
        return id_;
    }

private:
    bool is_const_;
    BasicType type_;
    Identifier *id_;
};

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
                return &entry;
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

private:
    std::list<SymbolScope> scope_lists_;
};

#endif //YFSCC_SYSBOLTABLE_H
