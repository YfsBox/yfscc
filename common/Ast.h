//
// Created by 杨丰硕 on 2023/2/6.
//
#ifndef YFSCC_AST_H
#define YFSCC_AST_H

#include <vector>
#include <memory>
#include "Types.h"

class AstNode;
class Expression;
class Statement;
class Identifier;
class FuncFParam;
class Declare;
class Define;
class FuncFParams;

// using AstNodeShptr = std::shared_ptr<AstNode>;
using AstNodePtr = std::unique_ptr<AstNodeType>;
using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;
using IdentifierPtr = std::unique_ptr<Identifier>;
using FuncFParamPtr = std::unique_ptr<FuncFParam>;

class AstNode {
public:
    AstNode() = default;
    virtual ~AstNode() {};
    explicit AstNode(int lineno): lineno_(lineno){}
    virtual void generateIR() = 0;
    virtual void dump() = 0;
    int lineno_;
};

class FuncDefine;
class CompUnit: AstNode {
public:
    using DeclPtr = std::unique_ptr<Declare>;
    using FuncDefinePtr = std::unique_ptr<FuncDefine>;

    CompUnit() = default;

    ~CompUnit() = default;

    void addDeclares(DeclPtr decl) {
        declares_.push_back(std::move(decl));
    }

    void addFuncDefine(FuncDefinePtr funcdef) {
        func_defs_.push_back(std::move(funcdef));
    }

    size_t getDeclNumber() const {
        return declares_.size();
    }

    size_t getFuncDefNumber() const {
        return func_defs_.size();
    }

    Declare *getDecl(int idx) {
        return declares_[idx].get();
    }

    FuncDefine *getFuncDef(int idx) {
        return func_defs_[idx].get();
    }

private:
    std::vector<DeclPtr> declares_;
    std::vector<FuncDefinePtr> func_defs_;
};

class Declare: AstNode {
public:
    virtual DeclType getDeclType() = 0;
};

class Define: AstNode {
public:
    virtual DefType getDefType() = 0;
};

class ConstDefine: Define {     // 暂时不考虑数组
public:
    ConstDefine(IdentifierPtr identifier, ExpressionPtr expr = nullptr): id_(std::move(identifier)), init_expr_(std::move(expr)) {}

    ~ConstDefine() = default;

    DefType getDefType() const {
        return DefType::CONSTDEF;
    }

    Identifier *getIdentifier() const {
        return id_.get();
    }

    Expression *getInitExpr() const {
        return init_expr_.get();
    }

    bool hasInitExpr() const {
        return init_expr_ != nullptr;
    }

private:
    IdentifierPtr id_;
    ExpressionPtr init_expr_;
};

class VarDefine: Define {
public:
    VarDefine(IdentifierPtr identifier, ExpressionPtr expr = nullptr): id_(std::move(identifier)), init_expr_(std::move(expr)) {}

    ~VarDefine() = default;

    DefType getDefType() {
        return DefType::VARDEF;
    }

    Identifier *getIdentifier() const {
        return id_.get();
    }

    Expression *getInitExpr() const {
        return init_expr_.get();
    }

    bool hasInitExpr() const {
        return init_expr_ != nullptr;
    }

private:
    IdentifierPtr id_;
    ExpressionPtr init_expr_;
};

class FuncDefine: Define {
public:
    using FuncFParamsPtr = std::unique_ptr<FuncFParams>;

    FuncDefine(BasicType return_type, IdentifierPtr identifier, FuncFParamsPtr formals, StatementPtr block):
        return_type_(return_type),
        func_id_(std::move(identifier)),
        formals_(std::move(formals)),
        block_(std::move(block)){

    }

    ~FuncDefine() = default;

    BasicType getReturnType() const {
        return return_type_;
    }

    Identifier *getId() const {
        return func_id_.get();
    }

    FuncFParams *getFormals() const {
        return formals_.get();
    }

    Statement *getBlock() const {
        return block_.get();
    }

private:
    BasicType return_type_;
    IdentifierPtr func_id_;
    FuncFParamsPtr formals_;
    StatementPtr block_;

};

class ConstDeclare : Declare {
public:

private:
    BasicType const_type_;

};

class VarDeclare : Declare {
public:

};

class Identifier : AstNode {
public:

    explicit Identifier(const std::string &id): id_(id) {}

    Identifier(int lineno, const std::string &id):AstNode(lineno), id_(id) {}

    std::string getId() const {
        return id_;
    }

    void generateIR() override;

private:
    std::string id_;
};

class Number : AstNode {
public:

    explicit Number(float val): AstNode(), number_type_(BasicType::FLOAT) {
        value_.float_val = val;
    }
    explicit Number(int32_t val): AstNode(), number_type_(BasicType::INT) {
        value_.int_val = val;
    }

    void generateIR() override;

private:
    union {
        int32_t int_val;
        float float_val;
    } value_;

    BasicType number_type_;
};


class Expression : AstNode {
public:
    BasicType type_;
};

class ConditionExpr : Expression {


};

class LvalExpr: Expression {        // 包不包括数组？？
public:
    explicit LvalExpr(IdentifierPtr id): id_(std::move(id)) {}
    void dump() override;
    void generateIR() override;
    Identifier *getId() const {
        return id_.get();
    }
private:
    IdentifierPtr id_;
};

class UnaryExpr : Expression {
public:
    UnaryExpr() = default;
    Expression *getExpr() const {
        return expr_.get();
    }

    UnaryOpType getOpType() const {
        return optype_;
    }

private:
    UnaryOpType optype_;
    ExpressionPtr expr_;
};

class BinaryExpr : Expression {
public:

    BinaryExpr() = default;

    BinaryOpType getOpType() const {
        return optype_;
    }

    Expression *getLeftExpr() const {
        return left_expr_.get();
    }

    Expression *getRightExpr() const {
        return right_expr_.get();
    }

private:
    BinaryOpType optype_;
    ExpressionPtr left_expr_;
    ExpressionPtr right_expr_;
};

class FuncRParams: AstNode {
public:

    FuncRParams() = default;

    ~FuncRParams() = default;

    void addExpr(ExpressionPtr expr) {
        exprs_.push_back(std::move(expr));
    }

    Expression *getExprByIdx(int idx) {
        return exprs_[idx].get();
    }

    size_t getSize() const {
        return exprs_.size();
    }

private:
    std::vector<ExpressionPtr> exprs_;
};

class Statement: AstNode {
public:
    virtual StatementType getStatementType() = 0;
};

class ExprStatement: Statement {
public:
    explicit ExprStatement(ExpressionPtr expr): expr_(std::move(expr)) {}

    ~ExprStatement() = default;

    StatementType getStatementType() const {
        return StatementType::EXPE;
    }

    Expression *getExpr() const {
        return expr_.get();
    }
private:
    ExpressionPtr expr_;
};

class AssignStatement: Statement {
public:
    AssignStatement(ExpressionPtr left, ExpressionPtr right):left_(std::move(left)), right_(std::move(right)) {}

    ~AssignStatement() = default;

    StatementType getStatementType() const {
        return StatementType::ASSIGN;
    }

    Expression *getLeftExpr() const {
        return left_.get();
    }

    Expression *getRightExpr() const {
        return right_.get();
    }

private:
    ExpressionPtr left_;
    ExpressionPtr right_;
};

class BlockStatement: Statement {
public:
    BlockStatement() = default;
    ~BlockStatement() = default;

    size_t getStatementSize() const {
        return statements_.size();
    }

    Statement *getStatement(int idx) const {
        return statements_[idx].get();
    }

private:
    std::vector<StatementPtr> statements_;
};

class IfElseStatement: Statement {
public:
    IfElseStatement(ExpressionPtr cond, StatementPtr ifstmt, StatementPtr elsestmt = nullptr):
    cond_(std::move(cond)),ifstmt_(std::move(ifstmt)), elsestmt_(std::move(elsestmt)) {}

    ~IfElseStatement() = default;

    StatementType getStatementType() const {
        return StatementType::IFELSE;
    }

    bool hasElse() const {
        return elsestmt_ != nullptr;
    }

    Expression *getCond() const {
        return cond_.get();
    }

    Statement *getIfStmt() const {
        return ifstmt_.get();
    }

    Statement *getElseStmt() const {
        return elsestmt_.get();
    }

private:
    ExpressionPtr cond_;
    StatementPtr ifstmt_;
    StatementPtr elsestmt_;
};

class WhileStatement: Statement {
public:
    WhileStatement(ExpressionPtr cond, StatementPtr statement): cond_(std::move(cond)), statement_(std::move(statement)) {}

    ~WhileStatement() = default;

    StatementType getStatementType() const {
        return StatementType::WHILE;
    }

    Expression *getCond() const {
        return cond_.get();
    }

    Statement *getStatement() const {
        return statement_.get();
    }

private:
    ExpressionPtr cond_;
    StatementPtr statement_;
};

class BreakStatement: Statement {
public:
    BreakStatement() = default;

    ~BreakStatement() = default;

    StatementType getStatementType() const {
        return StatementType::BREAK;
    }
};

class ContinueStatement: Statement {
public:
    ContinueStatement() = default;

    ~ContinueStatement() = default;

    StatementType getStatementType() const {
        return StatementType::CONTINUE;
    }
};

class ReturnStatement: Statement {
public:
    ReturnStatement(ExpressionPtr expr): expr_(std::move(expr)) {}

    ~ReturnStatement() = default;

    StatementType getStatementType() const {
        return StatementType::RETURN;
    }

    Expression *getExpr() const {
        return expr_.get();
    }

private:
    ExpressionPtr expr_;
};

class FuncFParam: AstNode {
public:

    FuncFParam(BasicType btype, IdentifierPtr identifier): formal_type_(btype), identifier_(std::move(identifier)) {}

    ~FuncFParam() = default;

    BasicType getFormalType() const {
        return formal_type_;
    }

    Identifier *getIdentifier() const {
        return identifier_.get();
    }

private:
    BasicType formal_type_;
    IdentifierPtr identifier_;
};

class FuncFParams: AstNode { // formal
public:
    FuncFParams() = default;

    ~FuncFParams() = default;

    void addFuncFormal(FuncFParamPtr formal) {
        formals_.push_back(std::move(formal));
    }

    FuncFParam *getFuncFormal(int idx) const {
        return formals_[idx].get();
    }

    size_t getFormalSize() const {
        return formals_.size();
    }

private:
    std::vector<FuncFParamPtr> formals_;
};





#endif //YFSCC_AST_H
