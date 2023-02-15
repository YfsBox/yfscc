//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SEMANTICCHECK_H
#define YFSCC_SEMANTICCHECK_H

#include "../common/AstVisitor.h"
#include "../common/SysbolTable.h"

class SemanticCheck: public AstVisitor {
public:
    SemanticCheck(): error_cnt(0) {}
    ~SemanticCheck() = default;

    void visit(const std::shared_ptr<CompUnit> &compunit) override;
    void visit(const std::shared_ptr<Declare> &decl) override;
    void visit(const std::shared_ptr<ConstDefine> &def) override;
    void visit(const std::shared_ptr<VarDefine> &def) override;
    void visit(const std::shared_ptr<FuncDefine> &def) override;
    void visit(const std::shared_ptr<UnaryExpr> &expr) override;
    void visit(const std::shared_ptr<BinaryExpr> &expr) override;
    void visit(const std::shared_ptr<AssignStatement> &stmt) override;
    void visit(const std::shared_ptr<IfElseStatement> &stmt) override;
    void visit(const std::shared_ptr<WhileStatement> &stmt) override;
    void visit(const std::shared_ptr<BreakStatement> &stmt) override;
    void visit(const std::shared_ptr<ContinueStatement> &stmt) override;
    void visit(const std::shared_ptr<ReturnStatement> *stmt) override;

private:
    bool checkIsValidMain(const FuncDefine *funcdef);

    size_t error_cnt;
    std::vector<std::string> error_msgs_;
    SymbolTable<FuncDefine> func_systable_;
};

#endif //YFSCC_SEMANTICCHECK_H
