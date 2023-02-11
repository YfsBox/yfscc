//
// Created by 杨丰硕 on 2023/2/9.
//
#ifndef YFSCC_UTILS_H
#define YFSCC_UTILS_H

#include <iostream>
#include <fstream>
#include "Types.h"

std::string getFileContent(const std::string& path);

void dumpPrefix(std::ostream &out, size_t n);

std::string basicType2Str(BasicType type);

std::string declType2Str(DeclType type);

std::string binaryType2Str(BinaryOpType type);

std::string unaryType2Str(UnaryOpType type);

std::string stmtType2Str(StatementType type);

#endif //YFSCC_UTILS_H
