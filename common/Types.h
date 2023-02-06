//
// Created by 杨丰硕 on 2023/2/6.
//

#ifndef YFSCC_TYPES_H
#define YFSCC_TYPES_H

enum BasicType {
    INT,
    FLOAT,
    VOID,
    UNVALID,
};

enum DeclType {
    CONSTDECL,
    VARDECL,
};

enum DefType {
    CONSTDEF,
    VARDEF,
    FUNCDEF,
};

enum ExprType {     // 一个表达式求值之后的类型，在语义分析中会使用到

};

enum AstNodeType {

};

enum BinaryOpType {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LT,
    LTE,
    GT,
    GTE,
    EQ,
    NEQ,
    AND,
    OR
};

enum UnaryOpType {
    POSITIVE, NEGATIVE, NOT
};

enum StatementType {
    EXPE,
    ASSIGN,
    BLOCK,
    IFELSE,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN,
};


#endif //YFSCC_TYPES_H
