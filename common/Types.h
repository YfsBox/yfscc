//
// Created by 杨丰硕 on 2023/2/6.
//

#ifndef YFSCC_TYPES_H
#define YFSCC_TYPES_H

enum BasicType {
    INT_BTYPE,
    FLOAT_BTYPE,
    VOID_BTYPE,
    UNVALID_BTYPE,
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
    ADD_OPTYPE,
    SUB_OPTYPE,
    MUL_OPTYPE,
    DIV_OPTYPE,
    MOD_OPTYPE,
    LT_OPTYPE,
    LTE_OPTYPE,
    GT_OPTYPE,
    GTE_OPTYPE,
    EQ_OPTYPE,
    NEQ_OPTYPE,
    AND_OPTYPE,
    OR_OPTYPE
};

enum UnaryOpType {
    POSITIVE_OPTYPE, NEGATIVE_OPTYPE, NOTOP_OPTYPE
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
