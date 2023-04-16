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
        -O3
        )

set(FRONTEND_SRC
        parser/Parser.cpp
        lexer/Lexer.cpp
        semantic/SemanticCheck.cc
        common/Utils.cc
        common/Ast.cc

        ir/Argument.cc
        ir/BasicBlock.cc
        ir/Constant.cc
        ir/Function.cc
        ir/GlobalVariable.cc
        ir/Instruction.cc
)

add_library(frontend ${FRONTEND_SRC})

add_subdirectory(test)

add_executable(yfscc main.cpp)
target_link_libraries(yfscc fl frontend)