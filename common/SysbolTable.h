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

class SymbolEntry {
public:
    SymbolEntry(BasicType type, bool cancal, double calval, Identifier *id, bool is_const): is_const_(is_const),
    can_calculated_(cancal), cal_value_(calval), type_(type), id_(id) {}

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

    bool canCalculated() const {
        return can_calculated_;
    }

    Identifier *getId() const {
        return id_;
    }

    double getCalValue() const {
        return cal_value_;
    }

    void dump(std::ostream &out, size_t n) const {
        id_->dump(out, n);
        dumpPrefix(out, n);
        out << "IS_CONST:";
        if (is_const_) {
            out << "TRUE\n";
        } else {
            out << "FALSE\n";
        }
        dumpPrefix(out, n);
        out << "CAN_CALCULATED:";
        if (can_calculated_) {
            out << "TRUE\n";
        } else {
            out << "FALSE\n";
        }
        dumpPrefix(out, n);
        out << "CAL_INIT_VALUE:" << cal_value_ << '\n';
        dumpPrefix(out, n);
        out << "BASIC_TYPE:" << basicType2Str(type_) << '\n';
    }

private:
    bool is_const_;
    bool can_calculated_;
    double cal_value_;      // 只有当can_calculated有效时，cal_value才是有效的
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
