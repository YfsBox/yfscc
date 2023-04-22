//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_IRBUILDER_H
#define YFSCC_IRBUILDER_H

#include "Value.h"
#include "../common/SysbolTable.h"
#include "../common/AstVisitor.h"

class Module;
class Function;
class BasicBlock;
class Value;
class IrDumper;

struct IrContext {

    IrContext(Module *module): curr_module_(module),
        ssa_no_(-1),
        curr_function_(nullptr),
        curr_bb_(nullptr) {}
        
    int ssa_no_{-1};

    Module *curr_module_;

    Function *curr_function_;

    BasicBlock *curr_bb_;

    void ResetSSA() {
        ssa_no_ = -1;
    }
};

class IrSymbolEntry {
public:
    IrSymbolEntry(bool is_const, BasicType basic_type, Value *value):
            is_const_(is_const),
            basic_type_(basic_type),
            value_(value) {
    }

    IrSymbolEntry(bool is_const, BasicType basic_type, std::vector<int32_t> &array_size, Value *value, const std::string &name):
            is_const_(is_const),
            basic_type_(basic_type),
            value_(value),
            array_dimension_(std::move(array_size)) {

    }

    ~IrSymbolEntry() = default;

    bool isConst() const {
        return is_const_;
    }

    const std::vector<int> &getArraySize() const {
        return array_dimension_;
    }

    BasicType getBasicType() const {
        return basic_type_;
    }

    bool isArray() const {
        return !array_dimension_.empty();
    }

    std::string getName() const {
        return value_->getName();
    }

    Value *getValue() const {
        return value_;
    }

private:
    bool is_const_;
    BasicType basic_type_;
    std::vector<int32_t> array_dimension_;
    Value *value_;
};


class IrBuilder: public AstVisitor {
public:
    explicit IrBuilder(std::ostream &out);

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

    void dump() const;

private:
    void addValueCast(Value *left, Value *right);

    void setCurrValue(Value *value);

    void addInstruction(Value *inst);

    std::unique_ptr<Module> module_;

    std::unique_ptr<IrContext> context_;

    std::unique_ptr<IrDumper> dumper_;

    std::unordered_map<std::string, Function *> function_map_;

    SymbolTable<IrSymbolEntry> var_symbol_table_;

    Declare *curr_decl_;

    Value *curr_value_;
};


#endif //YFSCC_IRBUILDER_H
