#include <iostream>
#include "common/Utils.h"
#include "parser/parser_sysy.tab.h"

extern int yyparse();
extern int scan_string(const char *str);

int main(int argc, char **argv) {
    // 第一个参数为file name
    std::string source_file;
    if (argc > 1) {
        source_file = argv[1];
        printf("the src file is: %s\n", source_file.c_str());
    }

    auto content = getFileContent(source_file);
    scan_string(content.c_str());

    yyparse();
    // yylex();
    return 0;
}
