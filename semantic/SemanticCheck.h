//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SEMANTICCHECK_H
#define YFSCC_SEMANTICCHECK_H

#include <unordered_map>
#include "../common/AstVisitor.h"
#include "../common/SysbolTable.h"

class SemanticCheck: public AstVisitor {
public:
    using FuncDefineMap = std::unordered_map<std::string, std::shared_ptr<FuncDefine>>;

    // SemanticCheck():AstVisitor(), curr_while_depth_(0), error_cnt(0), curr_func_scope_(nullptr) {}
    explicit SemanticCheck(std::ostream &out);

    ~SemanticCheck() = default;

    size_t getErrorSize() const {
        return error_msgs_.size();
    }

    void dumpErrorMsg() const;

    void visit(const std::shared_ptr<CompUnit> &compunit) override;
    void visit(const std::shared_ptr<Declare> &decl) override;
    void visit(const std::shared_ptr<ConstDeclare> &decl) override;
    void visit(const std::shared_ptr<VarDeclare> &decl) override;
    void visit(const std::shared_ptr<ConstDefine> &def) override;
    void visit(const std::shared_ptr<VarDefine> &def) override;
    void visit(const std::shared_ptr<FuncDefine> &def) override;
    void visit(const std::shared_ptr<UnaryExpr> &expr) override;
    void visit(const std::shared_ptr<BinaryExpr> &expr) override;
    void visit(const std::shared_ptr<BlockItems> &stmt) override;
    void visit(const std::shared_ptr<BlockItem> &stmt) override;
    void visit(const std::shared_ptr<AssignStatement> &stmt) override;
    void visit(const std::shared_ptr<IfElseStatement> &stmt) override;
    void visit(const std::shared_ptr<WhileStatement> &stmt) override;
    void visit(const std::shared_ptr<BreakStatement> &stmt) override;
    void visit(const std::shared_ptr<ContinueStatement> &stmt) override;
    void visit(const std::shared_ptr<CallFuncExpr> &expr) override;
    void visit(const std::shared_ptr<ReturnStatement> &stmt) override;
    void visit(const std::shared_ptr<Number> &number) override;
    void visit(const std::shared_ptr<Expression> &expr) override;
    void visit(const std::shared_ptr<LvalExpr> &expr) override;
    void visit(const std::shared_ptr<Statement> &stmt) override;
    void visit(const std::shared_ptr<ArrayValue> &arrayval) override;

private:

    bool checkIsValidMain(const FuncDefine *funcdef);

    bool checkArrayInitList(const std::shared_ptr<ArrayValue> &arrayval);

    void appendError(AstNode *node, const std::string &msg) {
        node->is_error_ = true;
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

    bool canCalculated(const std::shared_ptr<Expression> &init, double *value);

    void addLibFunc();

    std::shared_ptr<FuncDefine> createLibFuncDefine(BasicType btype, const std::string &funcname);

    void dumpSymbolTable() const;

    void checkVarDefine(const std::shared_ptr<VarDefine> &def, BasicType basic_type);

    void checkConstDefine(const std::shared_ptr<ConstDefine> &def, BasicType basic_type);

    bool checkArrayVarDefine(const std::shared_ptr<Define> &def, BasicType basic_type);

    bool isRedefinedCurrScope(const std::string &id);

    std::ostream &out_;

    size_t curr_while_depth_;        // 用来记录当前是否处于while之中
    size_t error_cnt;
    /*
    size_t curr_initlist_depth_;
    size_t curr_initlist_cnt_;
    size_t curr_initlist_fullcnt_;
    */
    FuncDefine *curr_func_scope_;       // 当前所处的函数
    Declare *curr_decl_;      // 用来追踪当前的array_decl,需要知道其声明类型
    std::vector<std::string> error_msgs_;
    // SymbolTable<FuncDefine> func_systable_;
    FuncDefineMap func_map_;
    FuncDefineMap lib_func_map_;
    SymbolTable<SymbolEntry> ident_systable_;
};  // 这其中的很多变量都表示在进行递归AST时，当前的一些状态

#endif //YFSCC_SEMANTICCHECK_H
