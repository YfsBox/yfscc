cmake_minimum_required(VERSION 3.16)
project(yfscc)

set(CMAKE_CXX_STANDARD 17)

set(PARSER_INSTALL ./parser/install.sh)
set(LEXER_INSTALL ./lexer/install.sh)

execute_process(COMMAND chmod +x ${PARSER_INSTALL} && ./${PARSER_INSTALL})
execute_process(COMMAND chmod +x ${LEXER_INSTALL} && ./${LEXER_INSTALL})

add_executable(yfscc main.cpp parser/parser.cpp lexer/lexer.cpp common/Utils.cc)
target_link_libraries(yfscc fl)