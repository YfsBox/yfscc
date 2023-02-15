//
// Created by 杨丰硕 on 2023/2/15.
//
#include <unordered_set>
#include "SemanticCheck.h"

bool SemanticCheck::checkIsValidMain(const FuncDefine *funcdef) {
    return funcdef->id_->getId() == "main" && funcdef->getFormals()->getFormalSize() == 0
        && funcdef->getReturnType() == BasicType::INT_BTYPE;
}

void SemanticCheck::visit(const std::shared_ptr<CompUnit> &compunit) {
    // 首先检查main函数是否存在
    std::unordered_set<std::string> name_set;

    auto func_number = compunit->getFuncDefNumber();
    auto decl_number = compunit->getDeclNumber();

    bool have_main = false;

    for (size_t i = 0; i < func_number; ++i) {
        auto func_def = compunit->getFuncDef(i);
        std::string func_name = func_def->id_->getId();
        if (name_set.find(func_name) != name_set.end()) {       // 如果之前已经存在了
            appendError("#Function Identifier Redination,the name is " + func_name + "\n");
        }
        name_set.insert(func_name);
        if (checkIsValidMain(func_def)) {
            have_main = true;
        }
    }

    if (!have_main) {
        appendError("#Not have a valid main function\n");
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        for (auto &def : decl->defs_) {
            std::string name = def->id_->getId();
            if (name_set.find(name) != name_set.end()) {        // 重复过的名字
                appendError("#Identifier Redination,the name is " + name + "\n");
            }
            name_set.insert(name);
        }
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        visit(DeclarePtr(decl));
    }

}

void SemanticCheck::visit(const std::shared_ptr<Declare> &decl) {
    for (auto &def : decl->defs_) {
        if (dynamic_cast<ConstDefine*>(def.get())) {

        } else {

        }
    }
}

void SemanticCheck::visit(const std::shared_ptr<ConstDefine> &def) {}

void SemanticCheck::visit(const std::shared_ptr<VarDefine> &def) {}

void SemanticCheck::visit(const std::shared_ptr<FuncDefine> &def) {
    // 首先创建新的作用域
    ident_systable_.enterScope();
    std::unordered_set<std::string> formal_name_set;
    for (size_t i = 0; i < def->getFormals()->getFormalSize(); ++i) {
        auto formal = def->getFormals()->getFuncFormal(i);
        // 检查是否有重名的
        std::string formal_name = formal->getFormalId()->getId();
        if (formal_name_set.find(formal_name) != formal_name_set.end()) {
            appendError("#the formal name invalid in function " + def->id_->getId() + ".\n");
        }
        formal_name_set.insert(formal_name);

    }


    ident_systable_.exitScope();
}

void SemanticCheck::visit(const std::shared_ptr<Expression> &expr) {
    ExprType type = expr->getExprNodeType();
    switch (type) {
        case ExprType::UNARY_TYPE:
            visit(std::dynamic_pointer_cast<UnaryExpr>(expr));
            return;
        case ExprType::BINARY_TYPE:
            visit(std::dynamic_pointer_cast<BinaryExpr>(expr));
            return;
        case ExprType::NUMBER_TYPE:
            visit(std::dynamic_pointer_cast<Number>(expr));
            return;
        case ExprType::ARRAYVALUE_TYPE:
            visit(std::dynamic_pointer_cast<ArrayValue>(expr));
            return;
        case ExprType::LVAL_TYPE:
            visit(std::dynamic_pointer_cast<LvalExpr>(expr));
            return;
        case ExprType::CALLFUNC_TYPE:
            visit(std::dynamic_pointer_cast<CallFuncExpr>(expr));
            return;
    }
}

void SemanticCheck::visit(const std::shared_ptr<UnaryExpr> &expr) {
    visit(ExpressionPtr(expr->getExpr()));
    expr->expr_type_ = expr->getExpr()->expr_type_;
}

void SemanticCheck::visit(const std::shared_ptr<BinaryExpr> &expr) {
    auto left = expr->getLeftExpr();
    auto right = expr->getRightExpr();

    visit(ExpressionPtr(left));
    visit(ExpressionPtr(right));

    if (left->expr_type_ == right->expr_type_) {
        expr->expr_type_ = left->expr_type_;
        if (expr->expr_type_ == BasicType::VOID_BTYPE) {
            appendError("#The left and right can't be void.\n");
        }
    } else {
        expr->expr_type_ = BasicType::INT_BTYPE;
    }
}

void SemanticCheck::visit(const std::shared_ptr<AssignStatement> &stmt) {
    // 首先求出左半部分
    auto left = stmt->getLeftExpr();
    visit(ExpressionPtr(left));
    LvalExpr *lval = dynamic_cast<LvalExpr*>(left);
    std::string name = lval->getId()->getId();
    // 查找符号表,判断是否是const
    Define *find_lval = ident_systable_.lookupFromAll(name);
    if (find_lval && find_lval->getDefType() == DefType::CONSTDEF) {  // 如果是存在的
        appendError("#lval in assignment statement can't be const type\n");
    }
    // 然后求出右半部分的
    auto right = stmt->getRightExpr();
    visit(ExpressionPtr(right));
}

void SemanticCheck::visit(const std::shared_ptr<IfElseStatement> &stmt) {}

void SemanticCheck::visit(const std::shared_ptr<WhileStatement> &stmt) {}

void SemanticCheck::visit(const std::shared_ptr<BreakStatement> &stmt) {
    if (!isInWhile()) {
        appendError("#Break statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<ContinueStatement> &stmt) {
    if (!isInWhile()) {
        appendError("#Continue statement not in while.\n");
    }
}

void SemanticCheck::visit(const std::shared_ptr<ReturnStatement> &stmt) {
    if (stmt->getExpr()) {

    }
}

void SemanticCheck::visit(const std::shared_ptr<Number> &number) {
    number->expr_type_ = number->getBtype();
}
