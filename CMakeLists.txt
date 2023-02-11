cmake_minimum_required(VERSION 3.16)
project(yfscc)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(test)

add_executable(yfscc main.cpp parser/parser.cpp lexer/lexer.cpp common/Utils.cc)
target_link_libraries(yfscc fl)