#include <iostream>
#include <fstream>
#include <cstring>
#include "common/Ast.h"
#include "common/Utils.h"
#include "ir/IrBuilder.h"
#include "ir/IrDumper.h"
#include "ir/Module.h"
#include "opt/DeadCodeElim.h"
#include "opt/ConstantPropagation.h"
#include "opt/InstCombine.h"
#include "opt/Mem2Reg.h"
#include "opt/DeadBlockElim.h"
#include "opt/FunctionInline.h"
#include "opt/GlobalAnalysis.h"
#include "opt/Svn.h"
#include "opt/LoopUnrolling.h"
#include "opt/BranchOptimizer.h"
#include "opt/AlgebraicSimplify.h"
#include "semantic/SemanticCheck.h"
#include "codegen/CodeGen.h"
#include "codegen/MachineDumper.h"
#include "codegen/RegsAllocator.h"


extern int yyparse();
extern std::shared_ptr<CompUnit> root;
extern void scan_string(const char *str);


int main(int argc, char **argv) {
    // 第一个参数为file name
    bool enable_opt = false;
    std::string source_file;
    std::string target_file;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-O2") == 0 || strcmp(argv[i], "-O1") == 0) { //判断参数是否为 "-S"
            enable_opt = true; // 如果是，则将flag设置为true
            continue;
        }
        if (strcmp(argv[i], "-o") == 0) {
            target_file = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-S") != 0) {
            source_file = argv[i];
        }
    }

    auto content = getFileContent(source_file);
    scan_string(content.c_str());

    yyparse();
    auto checker = std::make_unique<SemanticCheck>(std::cout);
    checker->visit(root);

    // checker->dumpErrorMsg();

    IrBuilder irbuilder(std::cout, checker->getLibFunctionsMap());
    irbuilder.visit(root);

    auto ir_module = irbuilder.getIrModule();

    PassManager pass_manager(ir_module);

    GlobalAnalysis global_analysis(ir_module);
    pass_manager.addPass(&global_analysis);

    ConstantPropagation const_propagation(ir_module);
    pass_manager.addPass(&const_propagation);

    BranchOptimizer branch_opt(ir_module);

    DeadBlockElim dead_bb_elim(ir_module);
    pass_manager.addPass(&dead_bb_elim);

    DeadCodeElim dead_code_elim(ir_module);
    Mem2Reg mem2reg(ir_module);
    InstCombine inst_combine(ir_module);
    FunctionInline function_inline(ir_module);
    Svn svn1(ir_module);
    Svn svn2(ir_module);
    LoopUnrolling loopunrolling(ir_module);
    AlgebraicSimplify algebric_simplify(ir_module);

    if (enable_opt) {
        mem2reg.ir_dumper_ = new IrDumper(std::cout);
        inst_combine.ir_dumper_ = new IrDumper(std::cout);
        dead_code_elim.ir_dumper_ = new IrDumper(std::cout);
        svn1.ir_dumper_ = new IrDumper(std::cout);
        function_inline.ir_dumper_ = new IrDumper(std::cout);
        loopunrolling.ir_dumper_ = new IrDumper(std::cout);
        algebric_simplify.ir_dumper_ = new IrDumper(std::cout);

        pass_manager.addPass(&dead_code_elim);
        pass_manager.addPass(&mem2reg);

        pass_manager.addPass(&dead_code_elim);
        pass_manager.addPass(&function_inline);
        pass_manager.addPass(&dead_code_elim);
        pass_manager.addPass(&svn1);

        pass_manager.addPass(&dead_code_elim);

        pass_manager.addPass(&const_propagation);
        pass_manager.addPass(&inst_combine);
        pass_manager.addPass(&algebric_simplify);
        pass_manager.addPass(&dead_code_elim);

        pass_manager.addPass(&loopunrolling);
        pass_manager.addPass(&dead_code_elim);
        pass_manager.addPass(&const_propagation);
        pass_manager.addPass(&inst_combine);
        pass_manager.addPass(&svn2);
        pass_manager.addPass(&dead_code_elim);

    }
    pass_manager.run();
    irbuilder.dump();

    CodeGen codegen(irbuilder.getIrModule());
    codegen.codeGenerate();

    /*MachineDumper vmcdumper(codegen.getMCModule(), target_file + ".v");
    vmcdumper.dump();*/

    RegsAllocator::regsAllocate(codegen.getMCModule(), &codegen);

    MachineDumper mcdumper(codegen.getMCModule(), target_file);
    mcdumper.dump();

    return 0;
}
