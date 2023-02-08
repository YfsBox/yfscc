%{
#include <cstdlib>
#include <cstdio>
#include "../common/Ast.h"
#include "../common/Types.h"

std::shared_ptr<CompUnit> root;

extern int yylex(void);

extern "C" void yyerror(const char *s);

%}

%locations

%union {
      BasicType type;
      std::string *string;
      int32_t int_val;
      float float_val;
      Number *number;
      CompUnit* root;
      FuncDefine* funcdef;
      Declare* declare;
      Identifier* ident;
      Expression* expr;
      BlockIterms* block;
      Statement* stmt;
      BlockIterm *block_stmt;
      FuncFParams* formals;
      FuncFParam* formal;
      FuncRParams* actuals;
}

%token <token> ADD SUB MUL DIV MOD
%token <token> ASSIGN EQ NEQ LT LTE GT GTE
%token <token> AND OR NOT
%token <token> IF ELSE WHILE BREAK CONTINUE RETURN
%token <token> CONST INT VOID FLOAT
%token <token> LEFT_PARENTHESES RIGHT_PARENTHESES
%token <token> LEFT_BRACKETS RIGHT_BRACKETS
%token <token> LEFT_BRACES RIGHT_BRACES
%token <token> COMMA SEMICOLON
%token <string> IDENTIFIER
%token <token> INTEGER
%token <float_val> FLOATPOINT

%type <root> compunit
%type <type> basic_type
%type <declare> decl const_decl var_decl
%type <funcdef> func_def
%type <ident> identifier
%type <expr> lval number lorexp landexp eqexp addexp mulexp primaryexp relexp unaryexp
%type <formals> func_formals
%type <formal> func_formal
%type <actuals> func_actuals
%type <block> block block_iterms
%type <block_stmt> block_iterm
%type <stmt> statement

%start compunit

%%

compunit: compunit decl {
            $$ = $1;
            $$->addDeclares(std::shared_ptr<Declare>($2));
      }
      | compunit func_def {
            $$ = $1;
            $$->addFuncDefine(std::shared_ptr<FuncDefine>($2));
      }
      | decl {
            root = std::make_shared<CompUnit>();
            $$ = root.get();
            $$->addDeclares(std::shared_ptr<Declare>($1));
      }
      | func_def {
            root = std::make_shared<CompUnit>();
            $$ = root.get();
            $$->addFuncDefine(std::shared_ptr<FuncDefine>($1));
      }
      ;

decl: const_decl SEMICOLON { $$ = $1;}
    | var_decl SEMICOLON { $$ = $1; }
    ;

basic_type: INT { $$ = BasicType::INT; }
    | FLOAT { $$ = BasicType::FLOAT; }
    ;

const_decl: CONST basic_type identifier ASSIGN addexp {
            $$ = new ConstDeclare($2);
            $$->addConstDef(std::shared_ptr<ConstDef>(
                std::shared_ptr<Identifier>($3),
                std::shared_ptr<Expression>($5)
            ));
      }
      | const_decl COMMA identifier ASSIGN addexp {
            $$ = $1;
            $$->addConstDef(std::shared_ptr<ConstDef>(
                            std::shared_ptr<Identifier>($3),
                            std::shared_ptr<Expression>($5)
                        ));
      }
      ;

var_decl: basic_type identifier {
            $$ = new VarDeclare($1);
            $$->addVarDef(std::shared_ptr<VarDefine>(std::shared_ptr<Identifier>($2)));
      }
      | basic_type identifier ASSIGN addexp {
             $$ = new VarDeclare($1);
             $$->addVarDef(std::shared_ptr<VarDefine>(
                std::shared_ptr<Identifier>($2),
                std::shared_ptr<Expression>($4)
             ));
      }
      | var_decl COMMA identifier {
            $$ = $1;
            $$->addVarDef(std::shared_ptr<VarDefine>(std::shared_ptr<Identifier>($2)));
      }
      | var_decl COMMA identifier ASSIGN addexp {
            $$ = $1;
            $$->addVarDef(std::shared_ptr<VarDefine>(
                            std::shared_ptr<Identifier>($3),
                            std::shared_ptr<Expression>($5)
            ));
      }
      ;

number: INTEGER { $$ = new Number($1); }
      | FLOATPOINT { $$ = new Number($1); }
      ;

identifier: IDENTIFIER {
        $$ = new Identifier($1);
    }
    ;

lval: identifier { $$ = new LvalExpr(std::shared_ptr<Identifier>($1)); }
      ;

primaryexp: LEFT_PARENTHESES addexp RIGHT_PARENTHESES { $$ = $2; }
      | lval { $$ = $1; }
      | number { $$ = $1; }
      ;

lorexp: landexp { $$ = $1; }
      | lorexp OR landexp { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::OR, std::shared_ptr<Expression>($3)); }
      ;

landexp: eqexp { $$ = $1; }
      | landexp AND eqexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::AND, std::shared_ptr<Expression>($3)); }
      ;

eqexp: relexp { $$ = $1; }
     | eqexp EQ relexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::EQ, std::shared_ptr<Expression>($3)); }
     | eqexp NEQ relexp { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::NEQ, std::shared_ptr<Expression>($3)); }
     ;

relexp: addexp { $$ = $1; }
      | relexp GT addexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::GT, std::shared_ptr<Expression>($3)); }
      | relexp GTE addexp { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::GTE, std::shared_ptr<Expression>($3)); }
      | relexp LT addexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::LT, std::shared_ptr<Expression>($3)); }
      | relexp LTE addexp { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::LTE, std::shared_ptr<Expression>($3)); }
      ;

addexp: mulexp { $$ = $1; }
      | addexp ADD mulexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::ADD, std::shared_ptr<Expression>($3)); }
      | addexp SUB mulexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::SUB, std::shared_ptr<Expression>($3)); }
      ;

mulexp: unaryexp { $$ = $1; }
      | mulexp MUL unaryexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::MUL, std::shared_ptr<Expression>($3)); }
      | mulexp DIV unaryexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::DIV, std::shared_ptr<Expression>($3)); }
      | mulexp MOD unaryexp  { $$ = new BinaryExpr(std::shared_ptr<Expression>($1), BinaryOpType::MOD, std::shared_ptr<Expression>($3)); }
      ;

unaryexp: primaryexp { $$ = $1; }
      | identifier LEFT_PARENTHESES RIGHT_PARENTHESES {
            $$ = new CallFuncExpr(std::shared_ptr<Identifier>($1));
      }
      | identifier LEFT_PARENTHESES func_actuals RIGHT_PARENTHESES {
            $$ = new CallFuncExpr(
                std::shared_ptr<Identifier>($1),
                std::shared_ptr<FuncRParams>($3)
            );
      }
      | ADD unaryexp  { $$ = new UnaryExpr(UnaryOp::POSITIVE, std::shared_ptr<Expression>($2)); }
      | SUB unaryexp  { $$ = new UnaryExpr(UnaryOp::NEGATIVE, std::shared_ptr<Expression>($2)); }
      | NOT unaryexp  { $$ = new UnaryExpr(UnaryOp::NOT, std::shared_ptr<Expression>($2)); }
      ;

func_def: basic_type identifier LEFT_PARENTHESES RIGHT_PARENTHESES block {
            $$ = new FuncDefine($1,
                std::shared_ptr<Identifier>($2),
                std::shared_ptr<FuncFParams>(),
                std::shared_ptr<Statement>($5));
      }
      | basic_type identifier LEFT_PARENTHESES func_formals RIGHT_PARENTHESES block {
            $$ = new FuncDefine($1,
                     std::shared_ptr<Identifier>($2),
                     std::shared_ptr<FuncFParams>($4),
                     std::shared_ptr<Statement>($6));
      }
      ;

func_formals: func_formal {
            $$ = new FuncRParams();
            $$->addFuncFormal(std::shared_ptr<FuncRParam>($1));
      }
      | func_formals COMMA func_formal {
            $$ = $1;
            $$->addFuncFormal(std::shared_ptr<FuncRParam>($3));
      }
      ;

func_formal: basic_type identifier { $$ = new FuncFParam($1, std::shared_ptr<Identifier>($2)); }
      ;


func_actuals: addexp { $$ = new FuncRParams(); $$->addExpr(std::shared_ptr<Expression>($1)); }
      | func_actuals COMMA addexp { $$ = $1; $$->addExpr(std::shared_ptr<Expression>($3)); }
      ;

block: LEFT_BRACES RIGHT_BRACES {
            $$ = new BlockIterms();
      }
     | LEFT_BRACES block_iterms RIGHT_BRACES {
            $$ = $2;
      }
     ;

block_iterms: block_iterm {
            $$ = new BlockIterms();
            if ($1 != nullptr) $$->addIterm(std::shared_ptr<BlockIterm>($1));
      }
      | block_iterms block_iterm {
            $$ = $1;
            if ($1 != nullptr) $$->addIterm(std::shared_ptr<BlockIterm>($2));
      }
      ;

block_iterm: decl { $$ = new BlockIterm(std::shared_ptr<Declare>($1)); }
      | statement { $$ = new BlockIterm(std::shared_ptr<Statement>($1)); }
      ;

statement: lval ASSIGN addexp SEMICOLON { $$ = new AssignStatement(std::shared_ptr<Expression>($1), std::shared_ptr<Expression>($3)); }
      | SEMICOLON { $$ = nullptr; }
      | addexp SEMICOLON { $$ = new EvalStatement(std::shared_ptr<Expression>($1)); }
      | block { $$ = $1; }
      | IF LEFT_PARENTHESES lorexp RIGHT_PARENTHESES statement {
            $$ = new IfElseStatement(std::shared_ptr<Expression>($3), std::shared_ptr<Statement>($5));
      }
      | IF LEFT_PARENTHESES lorexp RIGHT_PARENTHESES statement ELSE statement {
            $$ = new IfElseStatement(std::shared_ptr<Expression>($3), std::shared_ptr<Statement>($5), std::shared_ptr<Statement>($7));
      }
      | WHILE LEFT_PARENTHESES lorexp RIGHT_PARENTHESES statement {
            $$ = new WhileStatement(std::shared_ptr<Expression>($3), std::shared_ptr<Statement>($5));
      }
      | BREAK SEMICOLON { $$ = new BreakStatement(); }
      | CONTINUE SEMICOLON { $$ = new ContinueStatement(); }
      | RETURN SEMICOLON { $$ = new ReturnStatement(); }
      | RETURN addexp SEMICOLON { $$ = new ReturnStatement(std::shared_ptr<Expression>($2)); }
      ;

%%

extern "C" void yyerror(const char *s) {
      fprintf(stderr, "%d:%d: error: %s\n", yylloc.first_line,
              yylloc.first_column, s);
}