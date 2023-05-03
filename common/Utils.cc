//
// Created by 杨丰硕 on 2023/2/9.
//
#include "Utils.h"

static char dump_prefix[] = "---------------------------------------------------------------------------------------------------------------------------------------------------------------";

std::string getFileContent(const std::string& path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

void dumpPrefix(std::ostream &out, size_t n) {
    out << "|";
    if (n == 0) {
        return;
    }
    out << dump_prefix + 160 - n;
}

std::string basicType2Str(BasicType type) {
    switch (type) {
        case INT_BTYPE:
            return "INT";
        case FLOAT_BTYPE:
            return "FLOAT";
        case VOID_BTYPE:
            return "VOID";
        default:
            return "UNVALID";
    }
}

std::string declType2Str(DeclType type) {
    switch (type) {
        case CONSTDECL:
            return "CONST";
        case VARDECL:
            return "VARDECL";
        default:
            return "UNVALID";
    }
}

std::string binaryType2Str(BinaryOpType type) {
    switch (type) {
        case ADD_OPTYPE:
            return "ADD";
        case SUB_OPTYPE:
            return "SUB";
        case MUL_OPTYPE:
            return "MUL";
        case DIV_OPTYPE:
            return "DIV";
        case MOD_OPTYPE:
            return "MOD";
        case LT_OPTYPE:
            return "LT";
        case LTE_OPTYPE:
            return "LTE";
        case GT_OPTYPE:
            return "GT";
        case GTE_OPTYPE:
            return "GTE";
        case EQ_OPTYPE:
            return "EQ";
        case NEQ_OPTYPE:
            return "NEQ";
        case AND_OPTYPE:
            return "AND";
        case OR_OPTYPE:
            return "OR";
        default:
            return "UNVALID";
    }
}

std::string unaryType2Str(UnaryOpType type) {
    switch (type) {
        case POSITIVE_OPTYPE:
            return "POSITIVE";
        case NEGATIVE_OPTYPE:
            return "NEGATIVE";
        default:
            return "NOTOP";
    }
}

std::string stmtType2Str(StatementType type) {
    return "";
}

inline int32_t getFloat2IntForm(float value) {
    return *(int32_t *)(&value);
}





