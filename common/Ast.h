//
// Created by 杨丰硕 on 2023/2/6.
//
#ifndef YFSCC_AST_H
#define YFSCC_AST_H

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <bitset>
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
    explicit AstNode(int lineno): lineno_(lineno), is_error_(false){}
    // virtual void generateIR() = 0;
    virtual void dump(std::ostream &out, size_t n) {};
    int lineno_{-1};
    bool is_error_;
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

    DeclPtr getDecl(size_t idx) {
        return declares_[idx];
    }

    FuncDefinePtr getFuncDef(size_t idx) {
        return func_defs_[idx];
    }

private:
    std::vector<DeclPtr> declares_;         // 全局/const 声明
    std::vector<FuncDefinePtr> func_defs_;  // 函数
};

class Statement: public AstNode {
public:
    virtual StatementType getStmtType() = 0;
};

class Declare: public Statement {
public:
    explicit Declare(BasicType type): type_(type) {}
    virtual void addDef(const std::shared_ptr<Define> &def) = 0;
    StatementType getStmtType() override {
        return StatementType::DECL_STMTTYPE;
    }
    BasicType type_;
    std::vector<DefinePtr> defs_;       //
};

class Define: public AstNode {
public:
    // virtual DefType getDefType() = 0;
    explicit Define(const IdentifierPtr &ident, const ExpressionPtr &expr = nullptr): id_(ident), init_expr_(expr) {}
    virtual DefType getDefType() = 0;
    IdentifierPtr getId() const {
        return id_;
    }
    IdentifierPtr id_;
    ExpressionPtr init_expr_;
};

class ConstDefine: public Define {     // 暂时不考虑数组
public:
    ConstDefine(const IdentifierPtr &identifier, const ExpressionPtr &expr = nullptr): Define(identifier, expr) {}

    ~ConstDefine() = default;

    void dump(std::ostream &out, size_t n) override;

    DefType getDefType() override {
        return DefType::CONSTDEF;
    }

    ExpressionPtr getInitExpr() const {
        return init_expr_;
    }

private:

};

class VarDefine: public Define {
public:
    VarDefine(const IdentifierPtr &identifier, const ExpressionPtr &expr = nullptr): Define(identifier, expr) {}

    ~VarDefine() = default;

    void dump(std::ostream &out, size_t n) override;

    DefType getDefType() override {
        return DefType::VARDEF;
    }

    ExpressionPtr getInitExpr() const {
        return init_expr_;
    }

    bool hasInitExpr() const {
        return init_expr_ != nullptr;
    }
};

class FuncDefine: public Define {
public:
    using FuncFParamsPtr = std::shared_ptr<FuncFParams>;

    FuncDefine(BasicType return_type, const IdentifierPtr &identifier, const FuncFParamsPtr &formals, const StatementPtr &block):
            Define(identifier),
            return_type_(return_type),
            formals_(formals),
            block_(block){

    }

    void dump(std::ostream &out, size_t n) override;

    ~FuncDefine() = default;

    BasicType getReturnType() const {
        return return_type_;
    }

    DefType getDefType() override {
        return DefType::FUNCDEF;
    }

    FuncFParamsPtr getFormals() const {
        return formals_;
    }


    StatementPtr getBlock() const {
        return block_;
    }

private:
    BasicType return_type_;
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

    explicit Identifier(const std::string &id): id_(id) {}

    std::string getId() const {
        return id_;
    }

    void dump(std::ostream &out, size_t n) override;

    void addDimension(const ExpressionPtr &expr) {
        array_dimension_.push_back(expr);
    }
    size_t getDimensionSize() const {
        return array_dimension_.size();
    }

    ExpressionPtr getDimensionExpr(size_t idx) const {
        return array_dimension_[idx];
    }

    void setDimensionExpr(size_t idx, const ExpressionPtr &expr) {
        array_dimension_[idx] = expr;
    }

private:
    std::string id_;        // 该变量的name
    std::vector<ExpressionPtr> array_dimension_;    // 为了支持数组,所以将数组的维度信息放到identifier里面
};

class Expression:public AstNode {
public:
    BasicType expr_type_;
    virtual ExprType getExprNodeType() = 0;
};

class Number:public Expression {
public:

    explicit Number(float val): number_type_(BasicType::FLOAT_BTYPE) {
        value_.float_val = val;
    }

    explicit Number(int32_t val): number_type_(BasicType::INT_BTYPE) {
        value_.int_val = val;
    }

    BasicType getBtype() const {
        return number_type_;
    }

    ExprType getExprNodeType() override {
        return NUMBER_TYPE;
    }

    void dump(std::ostream &out, size_t n) override;

    int32_t getIntValue() const {
        return value_.int_val;
    }

    float getFloatValue() const {
        return value_.float_val;
    }

private:
    union {
        int32_t int_val;
        float float_val;
    } value_;

    BasicType number_type_;
};

class ArrayValue: public Expression {
public:
    using InitInterval = std::pair<int32_t, int32_t>;

    ArrayValue(bool is_number, const ExpressionPtr &value = nullptr)
    :is_number_(is_number), value_(value), array_idx_(-1) {}
    void addArrayValue(const ArrayValuePtr &array_value) {
        valueList_.push_back(array_value);
    }

    ExprType getExprNodeType() override {
        return ARRAYVALUE_TYPE;
    }

    void dump(std::ostream &out, size_t n) override;

    void addInitInterval(int32_t left, int32_t right) {
        if (left < right) {
            zero_init_intervals_.emplace_back(std::pair<int32_t, int32_t>(left, right));
        }
    }

    InitInterval getInterval(int idx) const {
        return zero_init_intervals_[idx];
    }

    size_t getInitIntervalSize() const {
        return zero_init_intervals_.size();
    }

    bool isNumber() const {
        return is_number_;
    }

    void setArrayIdx(int32_t idx) {
        array_idx_ = idx;
    }

    int32_t getArrayIdx() const {
        return array_idx_;
    }

public:
    bool is_number_;
    int32_t array_idx_;
    ExpressionPtr value_;
    std::vector<ArrayValuePtr> valueList_;
    std::vector<InitInterval> zero_init_intervals_;
};

class LvalExpr: public Expression {        // 包不包括数组？？
public:
    explicit LvalExpr(const IdentifierPtr &id): id_(id) {}
    // void dump() override;
    // void generateIR() override;
    IdentifierPtr getId() const {
        return id_;
    }

    ExprType getExprNodeType() override {
        return LVAL_TYPE;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    IdentifierPtr id_;      // 一个左值通常就是对一个Identifier的引用
};

class UnaryExpr : public Expression {
public:
    UnaryExpr() = default;

    UnaryExpr(UnaryOpType unaryoptype, const ExpressionPtr &expr):
    optype_(unaryoptype), expr_(expr) {}

    ExpressionPtr getExpr() const {
        return expr_;
    }

    UnaryOpType getOpType() const {
        return optype_;
    }

    ExprType getExprNodeType() override {
        return UNARY_TYPE;
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

    ExpressionPtr getLeftExpr() const {
        return left_expr_;
    }

    ExpressionPtr getRightExpr() const {
        return right_expr_;
    }

    ExprType getExprNodeType() override {
        return BINARY_TYPE;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    BinaryOpType optype_;
    ExpressionPtr left_expr_;
    ExpressionPtr right_expr_;
};

class FuncFParam: public AstNode {      // formal
public:

    FuncFParam(BasicType type, const IdentifierPtr &id):
    type_(type), id_(id) {}

    ~FuncFParam() = default;

    IdentifierPtr getFormalId() const {
        return id_;
    }

    BasicType getBtype() const {
        return type_;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    BasicType type_;    // 类型
    IdentifierPtr id_;      // id，因此判断一个id是否是数组类型的应该使用通过查看id中的dimension
};

class FuncRParams: public AstNode { // 表示一个由函数的实参序列
public:
    using FuncRParamPtr = std::shared_ptr<FuncFParam>;

    FuncRParams() = default;

    ~FuncRParams() = default;

    void addExpr(const ExpressionPtr &expr) {
        exprs_.push_back(expr);
    }

    ExpressionPtr getExprByIdx(int idx) {
        return exprs_[idx];
    }

    size_t getSize() const {
        return exprs_.size();
    }

    void dump(std::ostream &out, size_t n) override;

private:
    std::vector<ExpressionPtr> exprs_;  // 实参是Expression
};

class CallFuncExpr: public Expression {
public:
    using FuncRParamsPtr = std::shared_ptr<FuncRParams>;

    CallFuncExpr(const IdentifierPtr &func_id, const FuncRParamsPtr &actuals = nullptr):
    func_id_(func_id), actuals_(actuals) {}

    ~CallFuncExpr() = default;

    IdentifierPtr getFuncId() const {
        return func_id_;
    }

    size_t getActualSize() const {
        if (!actuals_) {
            return 0;
        }
        return actuals_->getSize();
    }

    ExprType getExprNodeType() override {
        return CALLFUNC_TYPE;
    }

    ExpressionPtr getActual(size_t idx) const {
        return actuals_->getExprByIdx(idx);
    }

    void dump(std::ostream &out, size_t n) override;

private:
    IdentifierPtr func_id_;     // 一个func id
    FuncRParamsPtr actuals_;        //
};

class AssignStatement: public Statement {
public:
    AssignStatement(const ExpressionPtr &left, const ExpressionPtr &right):left_(left), right_(right) {}

    ~AssignStatement() = default;

    StatementType getStmtType() override {
        return StatementType::ASSIGN_STMTTYPE;
    }

    ExpressionPtr getLeftExpr() const {
        return left_;
    }

    ExpressionPtr getRightExpr() const {
        return right_;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    ExpressionPtr left_;
    ExpressionPtr right_;
};

class BlockItem: public Statement {
public:
    BlockItem(const StatementPtr &stmt): stmt_(stmt) {}

    BlockItem(const DeclarePtr &decl): stmt_(decl) {}

    ~BlockItem() = default;

    StatementType getStmtType() override {
        return StatementType::BLOCKITEM_STMTTYPE;
    }

    StatementPtr getStmt() const {
        return stmt_;
    }

    void dump(std::ostream &out, size_t n) override;

private:
    StatementPtr stmt_;
};

class BlockItems: public Statement {
public:
    using BlockItemPtr = std::shared_ptr<BlockItem>;

    BlockItems() = default;

    void addItem(const BlockItemPtr &iterm) {
        items_.push_back(iterm);
    }

    StatementType getStmtType() override {
        return StatementType::BLOCKITEMS_STMTTYPE;
    }

    size_t getItemSize() const {
        return items_.size();
    }

    BlockItemPtr getBlockItem(size_t idx) const {
        return items_[idx];
    }

    void dump(std::ostream &out, size_t n) override;

private:
    std::vector<BlockItemPtr> items_;
};
/*
class BlockStatement: public Statement {
public:
    using BlockItemsPtr = std::shared_ptr<BlockItems>;

    BlockStatement(const BlockItemsPtr &iterms): iterms_(iterms) {}

    ~BlockStatement() = default;

private:
    BlockItemsPtr iterms_;
};*/

class EvalStatement: public Statement {
public:
    EvalStatement(const ExpressionPtr &expr): expr_(expr) {}
    ~EvalStatement() = default;

    void dump(std::ostream &out, size_t n) override;

    StatementType getStmtType() override {
        return StatementType::EVAL_STMTTYPE;
    }

    ExpressionPtr getExpr() const {
        return expr_;
    }

private:
    ExpressionPtr expr_;
};

class IfElseStatement: public Statement {
public:
    IfElseStatement(const ExpressionPtr &cond, const StatementPtr &ifstmt, const StatementPtr &elsestmt = nullptr):
            cond_(cond),ifstmt_(ifstmt), elsestmt_(elsestmt) {}

    ~IfElseStatement() = default;

    StatementType getStmtType() override {
        return StatementType::IFELSE_STMTTYPE;
    }

    bool hasElse() const {
        return elsestmt_ != nullptr;
    }

    ExpressionPtr getCond() const {
        return cond_;
    }

    StatementPtr getIfStmt() const {
        return ifstmt_;
    }

    StatementPtr getElseStmt() const {
        return elsestmt_;
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

    StatementType getStmtType() override {
        return StatementType::WHILE_STMTTYPE;
    }

    ExpressionPtr getCond() const {
        return cond_;
    }

    StatementPtr getStatement() const {
        return statement_;
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

    StatementType getStmtType() override {
        return StatementType::BREAK_STMTTYPE;
    }

    void dump(std::ostream &out, size_t n) override;
};

class ContinueStatement: public Statement {
public:
    ContinueStatement() = default;

    ~ContinueStatement() = default;

    StatementType getStmtType() override {
        return StatementType::CONTINUE_STMTTYPE;
    }

    void dump(std::ostream &out, size_t n) override;
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const ExpressionPtr &expr = nullptr): expr_(expr) {}

    ~ReturnStatement() = default;

    ExpressionPtr getExpr() const {
        return expr_;
    }

    void dump(std::ostream &out, size_t n) override;

    StatementType getStmtType() override {
        return StatementType::RETURN_STMTTYPE;
    }

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

    FuncFParamPtr getFuncFormal(size_t idx) const {
        return formals_[idx];
    }

    size_t getFormalSize() const {
        return formals_.size();
    }

    void dump(std::ostream &out, size_t n) override;
private:
    std::vector<FuncFParamPtr> formals_;
};

// 之所以使用FuncFParams




#endif //YFSCC_AST_H
