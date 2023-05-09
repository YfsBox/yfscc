//
// Created by 杨丰硕 on 2023/2/15.
//

#ifndef YFSCC_SEMANTICCHECK_H
#define YFSCC_SEMANTICCHECK_H

#include <unordered_map>
#include "../common/AstVisitor.h"
#include "../common/SysbolTable.h"

struct CurrArrayValueListInfo {
    BasicType value_type_;
    size_t dim_size_;
    int32_t max_values_{0};
    size_t curr_list_depth_{0};
    int32_t curr_depth_value_num_{0};
    int32_t total_value_num_{0};
    std::unordered_map<size_t, int32_t> depth_need_size_map_;
    ArrayValuePtr curr_top_array_initlist_;
};

class SymbolEntry {
public:
    SymbolEntry(BasicType type, bool cancal, double calval, Identifier *id, bool is_const): is_const_(is_const),
                                                                                            can_calculated_(cancal), cal_value_(calval), type_(type), id_(id) {}

    ~SymbolEntry() = default;

    std::string getName() const {
        return id_->getId();
    }

    BasicType getType() const {
        return type_;
    }

    bool isConst() const {
        return is_const_;
    }

    bool isArray() const {
        return id_->getDimensionSize() != 0;
    }

    bool canCalculated() const {
        return can_calculated_;
    }

    Identifier *getId() const {
        return id_;
    }

    double getCalValue() const {
        return cal_value_;
    }

    void dump(std::ostream &out, size_t n) const {
        id_->dump(out, n);
        dumpPrefix(out, n);
        out << "IS_CONST:";
        if (is_const_) {
            out << "TRUE\n";
        } else {
            out << "FALSE\n";
        }
        dumpPrefix(out, n);
        out << "CAN_CALCULATED:";
        if (can_calculated_) {
            out << "TRUE\n";
        } else {
            out << "FALSE\n";
        }
        dumpPrefix(out, n);
        out << "CAL_INIT_VALUE:" << cal_value_ << '\n';
        dumpPrefix(out, n);
        out << "BASIC_TYPE:" << basicType2Str(type_) << '\n';
    }

private:
    bool is_const_;
    bool can_calculated_;
    double cal_value_;      // 只有当can_calculated有效时，cal_value才是有效的
    BasicType type_;
    Identifier *id_;
};


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

    const FuncDefineMap &getLibFunctionsMap() const {
        return lib_func_map_;
    }

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

    void initCurrArrayListInfo(const std::shared_ptr<Define> &def, BasicType btype);

    inline void enterDeeperArrayList();

    inline void exitDeeperArrayList(int32_t old_depth_cnt);

    std::ostream &out_;

    size_t curr_while_depth_;        // 用来记录当前是否处于while之中

    size_t error_cnt;

    FuncDefine *curr_func_scope_;       // 当前所处的函数

    Declare *curr_decl_;      // 用来追踪当前的array_decl,需要知道其声明类型

    std::vector<std::string> error_msgs_;
    // SymbolTable<FuncDefine> func_systable_;
    FuncDefineMap func_map_;

    FuncDefineMap lib_func_map_;

    SymbolTable<SymbolEntry> ident_systable_;

    CurrArrayValueListInfo curr_array_list_info_;
};  // 这其中的很多变量都表示在进行递归AST时，当前的一些状态

#endif //YFSCC_SEMANTICCHECK_H
