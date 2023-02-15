//
// Created by 杨丰硕 on 2023/2/6.
//
#include "Ast.h"
#include "Utils.h"

void CompUnit::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "CompUnit:\n";
    for (auto &decl : declares_) {
        decl->dump(out, n + 1);
    }
    for (auto &funcdef : func_defs_) {
        funcdef->dump(out, n + 1);
    }
}

void ConstDeclare::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "ConstDeclare(" << basicType2Str(type_) << "):\n";

    for (auto &def : defs_) {
        def->dump(out, n + 1);
    }
}

void ConstDefine::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "ConstDefine:\n";
    id_->dump(out, n + 1);
    if (init_expr_) {
        init_expr_->dump(out, n + 1);
    }
}

void VarDeclare::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "VarDeclare(" << basicType2Str(type_) << "):\n";

    for (auto &def : defs_) {
        def->dump(out, n + 1);
    }
}

void VarDefine::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "VarDefine:\n";
    id_->dump(out, n + 1);
    if (init_expr_) {
        init_expr_->dump(out, n + 1);
    }
}

void FuncDefine::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "FuncDefine(" << basicType2Str(return_type_) << "):\n";
    id_->dump(out, n + 1);
    if (formals_) {
        formals_->dump(out, n + 1);
    }
    block_->dump(out, n + 1);
}

void Identifier::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "ID(" << id_ << ")";
    if (!array_dimension_.empty()) {
        out << "'S DIMENSION NUMBER:\n";
        for (auto &dimension : array_dimension_) {
            if (dimension) {
                dimension->dump(out, n + 1);
            } else {
                dumpPrefix(out, n + 1);
                out << "null dimension\n";
            }
        }
    } else {
        out << "\n";
    }
}

void Number::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Number(" << basicType2Str(number_type_) << "):";
    if (number_type_ == BasicType::INT_BTYPE) {
        out << value_.int_val << '\n';
    } else if (number_type_ == BasicType::FLOAT_BTYPE) {
        out << value_.float_val << '\n';
    }
}

void ArrayValue::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "ArrayValue:\n";
    dumpPrefix(out, n + 1);
    out << "IS_NUMBER: ";
    if (is_number_) {
        out << "TRUE\n";
        value_->dump(out, n + 1);
    } else {
        out << "FALSE\n";
        for (auto & arrayval : valueList_) {
            arrayval->dump(out, n + 1);
        }
    }
}

void LvalExpr::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "LvalExpr:\n";
    id_->dump(out, n + 1);
}

void UnaryExpr::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "UnaryExpr(" << unaryType2Str(optype_) << "):\n";
    expr_->dump(out, n + 1);
}

void BinaryExpr::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "BinaryExpr(" << binaryType2Str(optype_) << "):\n";
    left_expr_->dump(out, n + 1);
    right_expr_->dump(out, n + 1);
}

void FuncFParam::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Formal: " << basicType2Str(type_) << " " << id_->getId() << '\n';
    // dumpPrefix(out, n + 1);
    auto dsize = id_->getDimensionSize();
    if (dsize) {
        dumpPrefix(out, n + 1);
        out << "ArrayFormal Dimension:\n";
        for (size_t i = 0; i < dsize; ++i) {
            auto expr = id_->getDimensionExpr(i);
            if (expr) {
                expr->dump(out, n + 2);
            } else {
                dumpPrefix(out, n + 2);
                out << "NULL DIMENSION\n";
            }
        }
    }
}

void FuncRParams::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "FuncActuals:\n";
    for (auto &expr : exprs_) {
        expr->dump(out, n + 1);
    }
}

void CallFuncExpr::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "CallFuncExpr:\n";
    func_id_->dump(out, n + 1);
    if (actuals_) {
        actuals_->dump(out, n + 1);
    }
}

void AssignStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "AssignStatement:\n";
    left_->dump(out, n + 1);
    right_->dump(out, n + 1);
}

void BlockIterm::dump(std::ostream &out, size_t n) {
    if (stmt_) {
        stmt_->dump(out, n);
    }
}

void BlockIterms::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Block:\n";
    for (auto &item : iterms_) {
        item->dump(out, n + 1);
    }
}

void EvalStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "EvalStatement:\n";
    expr_->dump(out, n + 1);
}

void IfElseStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "IfElseStatement:\n";
    dumpPrefix(out, n + 1);
    out << "Condition:\n";
    cond_->dump(out, n + 2);
    ifstmt_->dump(out, n + 1);
    if (elsestmt_) {
        elsestmt_->dump(out, n + 1);
    }
}

void WhileStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "While:\n";
    cond_->dump(out, n + 1);
    statement_->dump(out, n + 1);
}

void BreakStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Break\n";
}

void ContinueStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Continue\n";
}

void ReturnStatement::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Return:\n";
    if (expr_) {
        expr_->dump(out, n + 1);
    }
}

void FuncFParams::dump(std::ostream &out, size_t n) {
    dumpPrefix(out, n);
    out << "Formals:\n";
    for (auto &formal : formals_) {
        formal->dump(out, n + 1);
    }
}










