#include <iostream>
#include <fstream>
#include "common/Ast.h"
#include "common/Utils.h"
#include "ir/IrBuilder.h"
#include "ir/IrDumper.h"
#include "ir/Module.h"
#include "opt/CollectUsedGlobals.h"
#include "opt/DeadCodeElim.h"
#include "opt/ConstantPropagation.h"
#include "semantic/SemanticCheck.h"
#include "codegen/CodeGen.h"
#include "codegen/MachineDumper.h"
#include "codegen/RegsAllocator.h"


extern int yyparse();
extern std::shared_ptr<CompUnit> root;
extern void scan_string(const char *str);

int main(int argc, char **argv) {
    // 第一个参数为file name
    std::string source_file;
    std::string target_file;
    if (argc > 1) {
        source_file = argv[1];
        // printf("the src file is: %s\n", source_file.c_str());
    }
    if (argc > 2) {
        target_file.assign(argv[2]);
    } else {
        target_file = "yfscc.s";
    }

    auto content = getFileContent(source_file);
    scan_string(content.c_str());

    yyparse();
    auto checker = std::make_unique<SemanticCheck>(std::cout);
    checker->visit(root);

    // root->dump(std::cout, 0);
    checker->dumpErrorMsg();

    IrBuilder irbuilder(std::cout, checker->getLibFunctionsMap());
    irbuilder.visit(root);
    // irbuilder.dump();

    auto ir_module = irbuilder.getIrModule();

    PassManager pass_manager(ir_module);

    CollectUsedGlobals globals_collector(ir_module);
    pass_manager.addPass(&globals_collector);

    ConstantPropagation const_propagation(ir_module);
    pass_manager.addPass(&const_propagation);

    DeadCodeElim dead_code_elim(ir_module);
    dead_code_elim.irdumper_ = new IrDumper(std::cout);
    pass_manager.addPass(&dead_code_elim);

    pass_manager.run();
    irbuilder.dump();

    CodeGen codegen(irbuilder.getIrModule());
    codegen.codeGenerate();

    MachineDumper vmcdumper(codegen.getMCModule(), "yfscc.v.s");
    vmcdumper.dump();

    RegsAllocator::regsAllocate(codegen.getMCModule(), &codegen);

    MachineDumper mcdumper(codegen.getMCModule(), "yfscc.s");
    mcdumper.dump();
    // yylex();
    return 0;
}
