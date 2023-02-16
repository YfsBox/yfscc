//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SEMANTICCHECK_H
#define YFSCC_SEMANTICCHECK_H

#include "../common/AstVisitor.h"
#include "../common/SysbolTable.h"

class SemanticCheck: public AstVisitor {
public:
    SemanticCheck(): curr_while_depth_(0), error_cnt(0), curr_func_scope_(nullptr) {}
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
    void visit(const std::shared_ptr<ReturnStatement> &stmt) override;
    void visit(const std::shared_ptr<Number> &number) override;
    void visit(const std::shared_ptr<Expression> &expr) override;
    void visit(const std::shared_ptr<LvalExpr> &expr) override;

private:

    bool checkIsValidMain(const FuncDefine *funcdef);

    void appendError(const std::string &msg) {
        error_msgs_.push_back(msg);
    }

    void addWhileDepth() {
        curr_while_depth_++;
    }

    void descWhileDepth() {
        curr_while_depth_--;
    }

    bool isInWhile() const {
        return curr_while_depth_ != 0;
    }

    size_t curr_while_depth_;        // 用来记录当前是否处于while之中
    size_t error_cnt;
    FuncDefine *curr_func_scope_;
    std::vector<std::string> error_msgs_;
    // SymbolTable<FuncDefine> func_systable_;
    SymbolTable<SymbolEntry> ident_systable_;
};

#endif //YFSCC_SEMANTICCHECK_H
