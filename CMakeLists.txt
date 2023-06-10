cmake_minimum_required(VERSION 3.16)
project(yfscc)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

set(CXX_FLAGS
        -g
        -Wall
        -std=c++17
        -D_PTHREADS
        -Wno-unused-parameter
        -Wno-unreachable
        -O2
        )

set(FRONTEND_SRC
        parser/Parser.cpp
        lexer/Lexer.cpp
        semantic/SemanticCheck.cc
        common/Utils.cc
        common/Ast.cc

        ir/Module.cc
        ir/Argument.cc
        ir/BasicBlock.cc
        ir/Constant.cc
        ir/Function.cc
        ir/GlobalVariable.cc
        ir/IrFactory.cc
        ir/Instruction.cc
        ir/IrBuilder.cc
        ir/IrDumper.cc
)

set(IROPT_SRC
        opt/PassManager.cc
        opt/CollectUsedGlobals.cc
        opt/DataflowAnalysis.cc
        opt/DeadCodeElim.cc
        opt/ConstantPropagation.cc
        opt/InstCombine.cc
)

set(BACKEND_SRC
        codegen/CodeGen.cc
        codegen/Machine.cc
        codegen/MachineOperand.cc
        codegen/RegsAllocator.cc
        codegen/MachineDumper.cc
        codegen/MachineInst.cc
)

add_library(frontend ${FRONTEND_SRC})
add_library(iropt ${IROPT_SRC})
target_link_libraries(iropt frontend)
add_library(backend ${BACKEND_SRC})
target_link_libraries(backend frontend)



add_executable(yfscc main.cpp)
target_link_libraries(yfscc iropt backend)