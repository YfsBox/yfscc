%option noyywrap

%{

#include <string>
#include "../common/Ast.h"
#include "../parser/Parser.hpp"

int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylineno;            \
                       yylloc.first_column = yycolumn;          \
                       for (auto i : std::string(yytext_ptr)) { \
                           if (i == '\n') {                     \
                               yylineno++;                      \
                               yycolumn = 1;                    \
                           } else {                             \
                               yycolumn++;                      \
                           }                                    \
                       }
%}


NONZERO_DIGIT [1-9]
DIGIT [0-9]

HEX_PREFIX (0x)|(0X)
HEX_DIGIT [0-9a-fA-F]

OCT_PREFIX 0
OCT_DIGIT [0-7]

DEC_INTEGER {NONZERO_DIGIT}{DIGIT}*
OCT_INTEGER {OCT_PREFIX}{OCT_DIGIT}*
HEX_INTEGER {HEX_PREFIX}{HEX_DIGIT}*

DEC_FLOATPOINT ({DIGIT}*[.]{DIGIT}+|{DIGIT}+[.])([eE][+-]?{DIGIT}+)?|{DIGIT}+([eE][+-]?{DIGIT}+)
HEX_FLOATPOINT {HEX_PREFIX}({HEX_DIGIT}*[.]{HEX_DIGIT}+|{HEX_DIGIT}+[.])([pP][+-]?{DIGIT}+)|{HEX_PREFIX}({HEX_DIGIT}+)([pP][+-]?{DIGIT}+)

TOKEN_INTEGER {DEC_INTEGER}|{OCT_INTEGER}|{HEX_INTEGER}
TOKEN_FLOATPOINT  {DEC_FLOATPOINT}|{HEX_FLOATPOINT}

LETTER [a-zA-Z]
TOKEN_IDENTIFIER ({LETTER}|_)({DIGIT}|{LETTER}|_)*

%%

\/\/.*              ;

"/*"                {
                        int c;
                        while((c = yyinput()) != 0) {
                            if(c == '\n') {
                                yylineno++;
                                yycolumn = 1;
                            } else if(c == '*') {
                            if((c = yyinput()) == '/')
                                break;
                            else
                                unput(c);
                            }
                        }
                    }

[\n]                ;
[ ]                 ;
[\t\r]              ;

"+"                 return ADD;
"-"                 return SUB;
"*"                 return MUL;
"/"                 return DIV;
"%"                 return MOD;

"if"                return IF;
"else"              return ELSE;
"while"             return WHILE;
"break"             return BREAK;
"continue"          return CONTINUE;
"return"            return RETURN;

"const"             return CONST;
"int"               return INT;
"void"              return VOID;
"float"             return FLOAT;

{TOKEN_IDENTIFIER}  yylval.string = new std::string(yytext, yyleng); return IDENTIFIER;
{TOKEN_INTEGER}     yylval.int_val = static_cast<int32_t>(strtol(yytext, NULL, 0)); return INTEGER;
{TOKEN_FLOATPOINT}	yylval.float_val = static_cast<float>(strtod(yytext, NULL)); return FLOATPOINT;

"="                 return ASSIGN;
"=="                return EQ;
"!="                return NEQ;

">"                 return GT;
">="                return GTE;
"<"                 return LT;
"<="                return LTE;

"&&"                return AND;
"||"                return OR;
"!"                 return NOT;

"("                 return LEFT_PARENTHESES;
")"                 return RIGHT_PARENTHESES;
"["                 return LEFT_BRACKETS;
"]"                 return RIGHT_BRACKETS;
"{"                 return LEFT_BRACES;
"}"                 return RIGHT_BRACES;

","                 return COMMA;
";"                 return SEMICOLON;

.                   printf("%s\n", (std::string("Unknown token: ") + yytext).c_str()); yyterminate();

%%

void scan_string(const char* str) {
    yy_switch_to_buffer(yy_scan_string(str));
}
