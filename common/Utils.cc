//
// Created by 杨丰硕 on 2023/2/9.
//
#include "Utils.h"

std::string getFileContent(const std::string& path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}





