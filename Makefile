p:
	bison -d -o parser/parser.cpp parser/parser.y
l:
	flex -o lexer/lexer.cpp lexer/lexer.l
clean:
	rm parser/parser.cpp parser/parser.hpp lexer/lexer.cpp
