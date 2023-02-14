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
class ArrayValue;

// using AstNodeShptr = std::shared_ptr<AstNode>;
using AstNodePtr = std::shared_ptr<AstNode>;
using ExpressionPtr = std::shared_ptr<Expression>;
using StatementPtr = std::shared_ptr<Statement>;
using IdentifierPtr = std::shared_ptr<Identifier>;
using FuncFParamPtr = std::shared_ptr<FuncFParam>;
using DeclarePtr = std::shared_ptr<Declare>;
using DefinePtr = std::shared_ptr<Define>;
using ArrayValuePtr = std::shared_ptr<ArrayValue>;

class AstNode: std::enable_shared_from_this<AstNode> {
public:
    AstNode() = default;
    virtual ~AstNode() = default;
    explicit AstNode(int lineno): lineno_(lineno){}
    // virtual void generateIR() = 0;
    virtual void dump(std::ostream &out, size_t n) {};
    int lineno_{-1};
};

class FuncDefine;
class CompUnit: public AstNode {
public:
    using DeclPtr = std::shared_ptr<Declare>;
    using FuncDefinePtr = std::shared_ptr<FuncDefine>;

    CompUnit() = default;

    ~CompUnit() = default;

    void dump(std::ostream &out, size_t n) override;

    void addDeclares(const DeclPtr &decl) {
        declares_.push_back(decl);
    }

    void addFuncDefine(const FuncDefinePtr &funcdef) {
        func_defs_.push_back(funcdef);
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

class Statement: public AstNode {
public:
    // virtual StatementType getStatementType() = 0;
};

class Declare: public Statement {
public:
    // virtual DeclType getDeclType() = 0;
    explicit Declare(BasicType type): type_(type) {}
    virtual void addDef(const std::shared_ptr<Define> &def) = 0;
    BasicType type_;
    std::vector<DefinePtr> defs_;
};

class Define: public AstNode {
public:
    // virtual DefType getDefType() = 0;
};

class ConstDefine: public Define {     // 暂时不考虑数组
public:
    ConstDefine(const IdentifierPtr &identifier, const ExpressionPtr &expr = nullptr): id_(identifier), init_expr_(expr) {}

    ~ConstDefine() = default;

    void dump(std::ostream &out, size_t n) override;

    DefType getDefType() const {
        return DefType::CONSTDEF;
    }

    Identifier *getIdentifier() const {
        return id_.get();
    }

    Expression *getInitExpr() const {
        return init_expr_.get();
    }

private:
    IdentifierPtr id_;
    ExpressionPtr init_expr_;
};

class VarDefine: public Define {
public:
    VarDefine(const IdentifierPtr &identifier, const ExpressionPtr &expr = nullptr): id_(identifier), init_expr_(expr) {}

    ~VarDefine() = default;

    void dump(std::ostream &out, size_t n) override;

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

class FuncDefine: public Define {
public:
    using FuncFParamsPtr = std::shared_ptr<FuncFParams>;

    FuncDefine(BasicType return_type, const IdentifierPtr &identifier, const FuncFParamsPtr &formals, const StatementPtr &block):
            return_type_(return_type),
            func_id_(identifier),
            formals_(formals),
            block_(block){

    }

    void dump(std::ostream &out, size_t n) override;

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

class ConstDeclare :public Declare {
public:
    using ConstDefPtr = std::shared_ptr<ConstDefine>;

    ConstDeclare(BasicType const_type): Declare(const_type){}

    ~ConstDeclare() = default;

    void addDef(const DefinePtr &def) {
        defs_.push_back(def);
    }

    void dump(std::ostream &out, size_t n) override;
};

class VarDeclare : public Declare {
public:
    using VarDeclarePtr = std::shared_ptr<VarDefine>;

    VarDeclare(BasicType var_type): Declare(var_type) {}

    ~VarDeclare() = default;

    void addDef(const DefinePtr &def) {
        defs_.push_back(def);
    }

    void dump(std::ostream &out, size_t n) override;
};

class Identifier : public AstNode {
public:

    explicit Identifier(std::string *id): id_(*id) {}

    Identifier(const std::string &id): id_(id) {}

    std::string getId() const {
        return id_;
    }

    void dump(std::ostream &out, size_t n) override;

    void addDimension(const ExpressionPtr &expr) {
        array_dimension_.push_back(expr);
    }

private:
    std::string id_;
    std::vector<ExpressionPtr> array_dimension_;
};

class Expression:public AstNode {
public:
    BasicType type_;
};

class Number:public Expression {
public:

    explicit Number(float val): number_type_(BasicType::FLOAT_BTYPE) {
        value_.float_val = val;
    }

    explicit Number(int32_t val): number_type_(BasicType::INT_BTYPE) {
        value_.int_val = val;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    union {
        int32_t int_val;
        float float_val;
    } value_;

    BasicType number_type_;
};

class ArrayValue: public Expression {
public:
    ArrayValue(bool is_number, const ExpressionPtr &value = nullptr)
    :is_number_(is_number), value_(value) {}
    void addArrayValue(const ArrayValuePtr &array_value) {
        valueList_.push_back(array_value);
    }
    void dump(std::ostream &out, size_t n) override;
public:
    bool is_number_;
    ExpressionPtr value_;
    std::vector<ArrayValuePtr> valueList_;
};

/*class ArrayDeclare: public Declare {
public:
    ArrayDeclare(BasicType type, const IdentifierPtr &identifier, const ArrayValuePtr &array_value,
                 bool is_const)
            :Declare(type), is_const_(is_const), id_(identifier), value_(array_value) {}
public:
    bool is_const_;
    IdentifierPtr id_;
    ArrayValuePtr value_;
};*/

class LvalExpr: public Expression {        // 包不包括数组？？
public:
    explicit LvalExpr(const IdentifierPtr &id): id_(id) {}
    // void dump() override;
    // void generateIR() override;
    Identifier *getId() const {
        return id_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    IdentifierPtr id_;
};

class UnaryExpr : public Expression {
public:
    UnaryExpr() = default;

    UnaryExpr(UnaryOpType unaryoptype, const ExpressionPtr &expr):
    optype_(unaryoptype), expr_(expr) {}

    Expression *getExpr() const {
        return expr_.get();
    }

    UnaryOpType getOpType() const {
        return optype_;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    UnaryOpType optype_;
    ExpressionPtr expr_;
};

class BinaryExpr : public Expression {
public:

    BinaryExpr() = default;

    BinaryExpr(const ExpressionPtr &left, BinaryOpType op, const ExpressionPtr &right):
        left_expr_(left), optype_(op), right_expr_(right){}

    BinaryOpType getOpType() const {
        return optype_;
    }

    Expression *getLeftExpr() const {
        return left_expr_.get();
    }

    Expression *getRightExpr() const {
        return right_expr_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    BinaryOpType optype_;
    ExpressionPtr left_expr_;
    ExpressionPtr right_expr_;
};

class FuncFParam: public AstNode {
public:

    FuncFParam(BasicType type, const IdentifierPtr &id):
    type_(type), id_(id) {}

    ~FuncFParam() = default;

    Identifier *getFormalId() const {
        return id_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    BasicType type_;
    IdentifierPtr id_;
};

class FuncRParams: public AstNode {
public:
    using FuncRParamPtr = std::shared_ptr<FuncFParam>;

    FuncRParams() = default;

    ~FuncRParams() = default;

    void addExpr(const ExpressionPtr &expr) {
        exprs_.push_back(expr);
    }

    Expression *getExprByIdx(int idx) {
        return exprs_[idx].get();
    }

    size_t getSize() const {
        return exprs_.size();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    std::vector<ExpressionPtr> exprs_;
};

class CallFuncExpr: public Expression {
public:
    using FuncRParamsPtr = std::shared_ptr<FuncRParams>;

    CallFuncExpr(const IdentifierPtr &func_id, const FuncRParamsPtr &actuals = nullptr):
    func_id_(func_id), actuals_(actuals) {}

    ~CallFuncExpr() = default;

    Identifier *getFuncId() const {
        return func_id_.get();
    }

    size_t getActualSize() const {
        if (!actuals_) {
            return 0;
        }
        return actuals_->getSize();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    IdentifierPtr func_id_;
    FuncRParamsPtr actuals_;
};

/*
class ExprStatement: public Statement {
public:
    explicit ExprStatement(const ExpressionPtr &expr): expr_(expr) {}

    ~ExprStatement() = default;

    StatementType getStatementType() const {
        return StatementType::EXPE_STMTTYPE;
    }

    Expression *getExpr() const {
        return expr_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    ExpressionPtr expr_;
};*/

class AssignStatement: public Statement {
public:
    AssignStatement(const ExpressionPtr &left, const ExpressionPtr &right):left_(left), right_(right) {}

    ~AssignStatement() = default;

    StatementType getStatementType() const {
        return StatementType::ASSIGN_STMTTYPE;
    }

    Expression *getLeftExpr() const {
        return left_.get();
    }

    Expression *getRightExpr() const {
        return right_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    ExpressionPtr left_;
    ExpressionPtr right_;
};

class BlockIterm: public Statement {
public:
    BlockIterm(const StatementPtr &stmt): stmt_(stmt) {}

    BlockIterm(const DeclarePtr &decl): stmt_(decl) {}

    ~BlockIterm() = default;

    void dump(std::ostream &out, size_t n) override;

private:
    StatementPtr stmt_;
};

class BlockIterms: public Statement {
public:
    using BlockItermPtr = std::shared_ptr<BlockIterm>;

    BlockIterms() = default;

    void addIterm(const BlockItermPtr &iterm) {
        iterms_.push_back(iterm);
    }

    void dump(std::ostream &out, size_t n) override;

private:
    std::vector<BlockItermPtr> iterms_;
};
/*
class BlockStatement: public Statement {
public:
    using BlockItermsPtr = std::shared_ptr<BlockIterms>;

    BlockStatement(const BlockItermsPtr &iterms): iterms_(iterms) {}

    ~BlockStatement() = default;

private:
    BlockItermsPtr iterms_;
};*/

class EvalStatement: public Statement {
public:
    EvalStatement(const ExpressionPtr &expr): expr_(expr) {}
    ~EvalStatement() = default;
    void dump(std::ostream &out, size_t n) override;
private:
    ExpressionPtr expr_;
};

class IfElseStatement: public Statement {
public:
    IfElseStatement(const ExpressionPtr &cond, const StatementPtr &ifstmt, const StatementPtr &elsestmt = nullptr):
            cond_(cond),ifstmt_(ifstmt), elsestmt_(elsestmt) {}

    ~IfElseStatement() = default;

    StatementType getStatementType() const {
        return StatementType::IFELSE_STMTTYPE;
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

    void dump(std::ostream &out, size_t n) override;

private:
    ExpressionPtr cond_;
    StatementPtr ifstmt_;
    StatementPtr elsestmt_;
};

class WhileStatement: public Statement {
public:
    WhileStatement(const ExpressionPtr &cond, const StatementPtr &statement): cond_(cond), statement_(statement) {}

    ~WhileStatement() = default;

    StatementType getStatementType() const {
        return StatementType::WHILE_STMTTYPE;
    }

    Expression *getCond() const {
        return cond_.get();
    }

    Statement *getStatement() const {
        return statement_.get();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    ExpressionPtr cond_;
    StatementPtr statement_;
};

class BreakStatement: public Statement {
public:
    BreakStatement() = default;

    ~BreakStatement() = default;

    StatementType getStatementType() const {
        return StatementType::BREAK_STMTTYPE;
    }

    void dump(std::ostream &out, size_t n) override;
};

class ContinueStatement: public Statement {
public:
    ContinueStatement() = default;

    ~ContinueStatement() = default;

    StatementType getStatementType() const {
        return StatementType::CONTINUE_STMTTYPE;
    }

    void dump(std::ostream &out, size_t n) override;
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const ExpressionPtr &expr = nullptr): expr_(expr) {}

    ~ReturnStatement() = default;

    StatementType getStatementType() const {
        return StatementType::RETURN_STMTTYPE;
    }

    Expression *getExpr() const {
        return expr_.get();
    }

    void dump(std::ostream &out, size_t n) override;
private:
    ExpressionPtr expr_;
};

class FuncFParams:public AstNode { // formal
public:
    FuncFParams() = default;

    ~FuncFParams() = default;

    void addFuncFormal(const FuncFParamPtr &formal) {
        formals_.push_back(formal);
    }

    FuncFParam *getFuncFormal(int idx) const {
        return formals_[idx].get();
    }

    size_t getFormalSize() const {
        return formals_.size();
    }

    void dump(std::ostream &out, size_t n) override;
private:
    std::vector<FuncFParamPtr> formals_;
};





#endif //YFSCC_AST_H
