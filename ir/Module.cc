//
// Created by 杨丰硕 on 2023/4/20.
//
#include "Module.h"
#include "GlobalVariable.h"
#include "Function.h"

void Module::addGlobalVariable(GlobalVariablePtr var) {
    global_variables_.emplace_back(std::move(var));
}