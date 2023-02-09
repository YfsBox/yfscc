cmake_minimum_required(VERSION 3.16)
project(yfscc)

set(CMAKE_CXX_STANDARD 17)

add_executable(yfscc main.cpp lexer/sysy_lexer.cc parser/parser_sysy.tab.cc common/Utils.cc )
#target_link_libraries(yfscc fl)