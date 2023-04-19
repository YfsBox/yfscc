//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_IRBUILDER_H
#define YFSCC_IRBUILDER_H

#include "../common/SysbolTable.h"
#include "../common/AstVisitor.h"

class Module;
class Function;
class BasicBlock;
class Value;

struct IrContext {

    IrContext(Module *module): curr_module_(module),
        ssa_no_(-1),
        curr_function_(nullptr),
        curr_bb_(nullptr) {}
        
    int ssa_no_{-1};

    Module *curr_module_;

    Function *curr_function_;

    BasicBlock *curr_bb_;

    void ResetSSA();
};

void IrContext::ResetSSA() {
    ssa_no_ = -1;
}

class IrSymbolEntry {
public:
    IrSymbolEntry(bool is_const, BasicType basic_type, const std::string &name):
            is_const_(is_const),
            basic_type_(basic_type),
            name_(name) {
    }

    IrSymbolEntry(bool is_const, BasicType basic_type, std::vector<int> &array_size, const std::string &name):
            is_const_(is_const),
            basic_type_(basic_type),
            name_(name),
            array_dimension_(std::move(array_size)) {

    }

    ~IrSymbolEntry() = default;

    bool isConst() const {
        return is_const_;
    }

    const std::vector<int> &getArraySize() const {
        return array_dimension_;
    }

    std::string getName() const {
        return name_;
    }

    BasicType getBasicType() const {
        return basic_type_;
    }

    bool isArray() const {
        return !array_dimension_.empty();
    }

private:
    bool is_const_;
    BasicType basic_type_;
    std::string name_;
    std::vector<int> array_dimension_;
};


class IrBuilder: public AstVisitor {
public:
    explicit IrBuilder();

    ~IrBuilder();

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
    std::unique_ptr<Module> module_;

    std::unique_ptr<IrContext> context_;

    SymbolTable<IrSymbolEntry> var_symbol_table_;

    Declare *curr_decl_;

    Value *curr_value_;
};


#endif //YFSCC_IRBUILDER_H
