//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_IRBUILDER_H
#define YFSCC_IRBUILDER_H

#include <deque>
#include "Value.h"
#include "../common/SysbolTable.h"
#include "../common/AstVisitor.h"
#include "../semantic/SemanticCheck.h"

class Module;
class Function;
class BasicBlock;
class ConstantArray;
class Expression;
class Value;
class IrDumper;
class Instruction;

struct IrContext {

    IrContext(Module *module): curr_module_(module),
        ssa_no_(-1),
        curr_function_(nullptr),
        curr_bb_(nullptr),
        curr_bb_has_ret_(false){}
        
    int ssa_no_{-1};

    int block_no_{-1};

    bool curr_bb_has_ret_;

    Module *curr_module_;

    Function *curr_function_;

    BasicBlock *curr_bb_;

    void ResetSSA() {
        ssa_no_ = -1;
    }

    void ResetBlockNo() {
        block_no_ = -1;
    }

};

class IrSymbolEntry {
public:
    IrSymbolEntry(bool is_const, BasicType basic_type, Value *value, const std::string &name):
            is_const_(is_const),
            basic_type_(basic_type),
            value_(value),
            name_(name){
    }

    IrSymbolEntry(bool is_const, BasicType basic_type, std::vector<int32_t> &array_size, Value *value, const std::string &name):
            is_const_(is_const),
            basic_type_(basic_type),
            value_(value),
            array_dimension_(std::move(array_size)),
            name_(name) {

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
        return name_;
    }

    Value *getValue() const {
        return value_;
    }

private:
    bool is_const_;
    std::string name_;
    BasicType basic_type_;
    std::vector<int32_t> array_dimension_;
    Value *value_;
};


class IrBuilder: public AstVisitor {
public:
    using CondJumpMap = std::unordered_map<std::shared_ptr<Expression>, std::vector<Instruction *>>;

    using WhileBasicBlockPair = std::pair<BasicBlock *, BasicBlock *>;

    using LibFunctionMap = std::unordered_map<std::string, std::unique_ptr<Function>>;

    explicit IrBuilder(std::ostream &out, const SemanticCheck::FuncDefineMap &libfunc_map);

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

    void setCurrBasicBlock(Value *bb);

    void addInstruction(Value *inst);

    void addBasicBlock(Value *bb);

    void getDimensionNumber(const std::shared_ptr<Define> &def, std::vector<int32_t> &dimension_number);

    void setGlobalArrayInitValue(const std::shared_ptr<ArrayValue> &array_init_value, ConstantArray *const_array);

    void dealExprAsCond(const std::shared_ptr<Expression> &expr);

    void enableDealCond() {
        is_deal_cond_ = true;
    }

    void disableDealCond() {
        is_deal_cond_ = false;
    }

    void addLibFunctions(const SemanticCheck::FuncDefineMap &libfunc_map);

    std::vector<Value *> arrayIndex2IndexVec(int32_t index) const;

    void initJumpMap();

    bool isSecondaryPtr(Value *ptr) const;

    bool is_deal_cond_;

    Expression *curr_top_expr_;

    std::unique_ptr<Module> module_;

    std::unique_ptr<IrContext> context_;

    std::unique_ptr<IrDumper> dumper_;

    std::unordered_map<std::string, Function *> function_map_;

    SymbolTable<IrSymbolEntry> var_symbol_table_;

    Declare *curr_decl_;

    Value *curr_value_;

    Value *curr_local_array_;

    Function *memset_function_;

    CondJumpMap true_jump_map_;     // key表示的是一个条件表达式，value是一个由instruction组成的集合,该条件表达式中包含的指令

    CondJumpMap false_jump_map_;

    std::deque<WhileBasicBlockPair> while_stack_;

    LibFunctionMap libfunction_map_;
};


#endif //YFSCC_IRBUILDER_H
