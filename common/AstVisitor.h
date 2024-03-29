//
// Created by 杨丰硕 on 2023/2/6.
//

#ifndef YFSCC_ASTVISITOR_H
#define YFSCC_ASTVISITOR_H

#include "Ast.h"

class AstVisitor {
public:

    AstVisitor() = default;
    virtual ~AstVisitor() = default;

    virtual void visit(const std::shared_ptr<CompUnit> &compunit) = 0;
    virtual void visit(const std::shared_ptr<Declare> &decl) = 0;
    virtual void visit(const std::shared_ptr<ConstDeclare> &decl) = 0;
    virtual void visit(const std::shared_ptr<VarDeclare> &decl) = 0;
    virtual void visit(const std::shared_ptr<ConstDefine> &def) = 0;
    virtual void visit(const std::shared_ptr<VarDefine> &def) = 0;
    virtual void visit(const std::shared_ptr<FuncDefine> &def) = 0;
    virtual void visit(const std::shared_ptr<UnaryExpr> &expr) = 0;
    virtual void visit(const std::shared_ptr<BinaryExpr> &expr) = 0;
    virtual void visit(const std::shared_ptr<BlockItems> &stmt) = 0;
    virtual void visit(const std::shared_ptr<BlockItem> &stmt) = 0;
    virtual void visit(const std::shared_ptr<AssignStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<IfElseStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<WhileStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<BreakStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<ContinueStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<CallFuncExpr> &expr) = 0;
    virtual void visit(const std::shared_ptr<ReturnStatement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<Number> &number) = 0;
    virtual void visit(const std::shared_ptr<Expression> &expr) = 0;
    virtual void visit(const std::shared_ptr<LvalExpr> &expr) = 0;
    virtual void visit(const std::shared_ptr<Statement> &stmt) = 0;
    virtual void visit(const std::shared_ptr<ArrayValue> &arrayval) = 0;

};

#endif //YFSCC_ASTVISITOR_H
