//
// Created by 杨丰硕 on 2023/2/6.
//
#include "Ast.h"
#include "Utils.h"

void CompUnit::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "CompUnit:\n";
    for (auto &decl : declares_) {
        decl->dump(out, n + 1);
    }
    for (auto &funcdef : func_defs_) {
        funcdef->dump(out, n + 1);
    }
}

void ConstDeclare::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "ConstDeclare:\n";


}

void VarDeclare::dump(std::ostream &out, size_t n) {


}


