#include <iostream>
#include "common/Ast.h"
#include "common/Utils.h"
#include "semantic/SemanticCheck.h"

extern int yyparse();
extern std::shared_ptr<CompUnit> root;
extern void scan_string(const char *str);

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
    auto checker = std::make_unique<SemanticCheck>(std::cout);
    checker->visit(root);

    root->dump(std::cout, 0);

    checker->dumpErrorMsg();
    // yylex();
    return 0;
}
