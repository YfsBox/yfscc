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
            error_cnt++;
            error_msgs_.push_back("#Function Identifier Redination,the name is " + func_name + "\n");
        }
        name_set.insert(func_name);
        if (checkIsValidMain(func_def)) {
            have_main = true;
        }
    }

    if (!have_main) {
        error_cnt++;
        error_msgs_.push_back("#Not have a valid main function\n");
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        for (auto &def : decl->defs_) {
            std::string name = def->id_->getId();
            if (name_set.find(name) != name_set.end()) {        // 重复过的名字
                error_cnt++;
                error_msgs_.push_back("#Identifier Redination,the name is " + name + "\n");
            }
            name_set.insert(name);
        }
    }

    for (size_t i = 0; i < decl_number; ++i) {
        auto decl = compunit->getDecl(i);
        visit(std::shared_ptr<Declare>(decl));
    }

}

void SemanticCheck::visit(const std::shared_ptr<Declare> &decl) {
    for (auto &def : decl->defs_) {
        if (dynamic_cast<ConstDefine*>(def.get())) {

        } else {

        }
    }
}

