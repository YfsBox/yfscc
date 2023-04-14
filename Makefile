p:
	bison -d -o parser/Parser.cpp parser/parser.y
l:
	flex -o lexer/Lexer.cpp lexer/lexer.l
clean:
	rm parser/Parser.cpp parser/Parser.hpp lexer/Lexer.cpp
