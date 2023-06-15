#!/bin/bash

clang++ -std=c++17 -O2 -lm -lpthread main.cpp codegen/CodeGen.cc codegen/MachineOperand.cc codegen/Machine.cc codegen/MachineInst.cc codegen/MachineDumper.cc codegen/RegsAllocator.cc common/Ast.cc common/AstVisitor.cc common/Utils.cc common/SymbolTable.cc ir/Module.cc ir/IrDumper.cc ir/GlobalVariable.cc ir/Function.cc ir/Constant.cc ir/IrBuilder.cc ir/Argument.cc ir/Instruction.cc ir/IrFactory.cc ir/BasicBlock.cc lexer/Lexer.cpp opt/Mem2Reg.cc opt/DeadCodeElim.cc opt/PassManager.cc opt/GvnGcm.cc opt/ConstantPropagation.cc opt/CopyPropagation.cc opt/ComputeDominators.cc opt/CollectUsedGlobals.cc opt/DataflowAnalysis.cc opt/InstCombine.cc parser/Parser.cpp semantic/SemanticCheck.cc -I codegen -I common -I ir -I opt -I parser -I semantic -o compiler