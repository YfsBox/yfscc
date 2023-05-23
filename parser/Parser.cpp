/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ADD = 258,
     SUB = 259,
     MUL = 260,
     DIV = 261,
     MOD = 262,
     ASSIGN = 263,
     EQ = 264,
     NEQ = 265,
     LT = 266,
     LTE = 267,
     GT = 268,
     GTE = 269,
     AND = 270,
     OR = 271,
     NOT = 272,
     IF = 273,
     ELSE = 274,
     WHILE = 275,
     BREAK = 276,
     CONTINUE = 277,
     RETURN = 278,
     CONST = 279,
     INT = 280,
     VOID = 281,
     FLOAT = 282,
     LEFT_PARENTHESES = 283,
     RIGHT_PARENTHESES = 284,
     LEFT_BRACKETS = 285,
     RIGHT_BRACKETS = 286,
     LEFT_BRACES = 287,
     RIGHT_BRACES = 288,
     COMMA = 289,
     SEMICOLON = 290,
     IDENTIFIER = 291,
     INTEGER = 292,
     FLOATPOINT = 293
   };
#endif
/* Tokens.  */
#define ADD 258
#define SUB 259
#define MUL 260
#define DIV 261
#define MOD 262
#define ASSIGN 263
#define EQ 264
#define NEQ 265
#define LT 266
#define LTE 267
#define GT 268
#define GTE 269
#define AND 270
#define OR 271
#define NOT 272
#define IF 273
#define ELSE 274
#define WHILE 275
#define BREAK 276
#define CONTINUE 277
#define RETURN 278
#define CONST 279
#define INT 280
#define VOID 281
#define FLOAT 282
#define LEFT_PARENTHESES 283
#define RIGHT_PARENTHESES 284
#define LEFT_BRACKETS 285
#define RIGHT_BRACKETS 286
#define LEFT_BRACES 287
#define RIGHT_BRACES 288
#define COMMA 289
#define SEMICOLON 290
#define IDENTIFIER 291
#define INTEGER 292
#define FLOATPOINT 293




/* Copy the first part of user declarations.  */
#line 1 "parser/parser.y"

#include <cstdlib>
#include <cstdio>
#include "../common/Types.h"
#include "../common/Ast.h"

std::shared_ptr<CompUnit> root;

extern int yylex(void);
extern void yyerror(const char *s);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 16 "parser/parser.y"
{
      BasicType type;
      std::string *string;
      int32_t int_val;
      float float_val;
      Number* number;
      CompUnit* root;
      FuncDefine* funcdef;
      Declare* declare;
      Identifier* ident;
      Expression* expr;
      BlockItems* block;
      Statement* stmt;
      ArrayValue* array_value;
      BlockItem *block_stmt;
      FuncFParams* formals;
      FuncFParam* formal;
      FuncRParams* actuals;
}
/* Line 193 of yacc.c.  */
#line 205 "parser/Parser.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 230 "parser/Parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   273

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  29
/* YYNRULES -- Number of rules.  */
#define YYNRULES  91
/* YYNRULES -- Number of states.  */
#define YYNSTATES  174

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    11,    13,    16,    19,    21,
      23,    25,    31,    37,    43,    49,    52,    57,    61,    67,
      70,    75,    79,    85,    87,    89,    91,    96,   101,   104,
     108,   110,   112,   116,   120,   122,   127,   132,   136,   138,
     140,   142,   146,   148,   152,   154,   158,   162,   164,   168,
     172,   176,   180,   182,   186,   190,   192,   196,   200,   204,
     206,   210,   215,   218,   221,   224,   230,   237,   239,   243,
     246,   248,   253,   258,   260,   264,   267,   271,   273,   276,
     278,   280,   285,   287,   290,   292,   298,   306,   312,   315,
     318,   321
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      40,     0,    -1,    40,    41,    -1,    40,    59,    -1,    41,
      -1,    59,    -1,    43,    35,    -1,    44,    35,    -1,    25,
      -1,    27,    -1,    26,    -1,    24,    42,    46,     8,    56,
      -1,    43,    34,    46,     8,    56,    -1,    24,    42,    47,
       8,    48,    -1,    43,    34,    47,     8,    48,    -1,    42,
      46,    -1,    42,    46,     8,    56,    -1,    44,    34,    46,
      -1,    44,    34,    46,     8,    56,    -1,    42,    47,    -1,
      42,    47,     8,    48,    -1,    44,    34,    47,    -1,    44,
      34,    47,     8,    48,    -1,    37,    -1,    38,    -1,    36,
      -1,    46,    30,    56,    31,    -1,    47,    30,    56,    31,
      -1,    32,    33,    -1,    32,    49,    33,    -1,    56,    -1,
      48,    -1,    49,    34,    56,    -1,    49,    34,    48,    -1,
      46,    -1,    46,    30,    56,    31,    -1,    50,    30,    56,
      31,    -1,    28,    56,    29,    -1,    50,    -1,    45,    -1,
      53,    -1,    52,    16,    53,    -1,    54,    -1,    53,    15,
      54,    -1,    55,    -1,    54,     9,    55,    -1,    54,    10,
      55,    -1,    56,    -1,    55,    13,    56,    -1,    55,    14,
      56,    -1,    55,    11,    56,    -1,    55,    12,    56,    -1,
      57,    -1,    56,     3,    57,    -1,    56,     4,    57,    -1,
      58,    -1,    57,     5,    58,    -1,    57,     6,    58,    -1,
      57,     7,    58,    -1,    51,    -1,    46,    28,    29,    -1,
      46,    28,    63,    29,    -1,     3,    58,    -1,     4,    58,
      -1,    17,    58,    -1,    42,    46,    28,    29,    64,    -1,
      42,    46,    28,    60,    29,    64,    -1,    61,    -1,    60,
      34,    61,    -1,    42,    46,    -1,    62,    -1,    42,    46,
      30,    31,    -1,    62,    30,    56,    31,    -1,    56,    -1,
      63,    34,    56,    -1,    32,    33,    -1,    32,    65,    33,
      -1,    66,    -1,    65,    66,    -1,    41,    -1,    67,    -1,
      50,     8,    56,    35,    -1,    35,    -1,    56,    35,    -1,
      64,    -1,    18,    28,    52,    29,    67,    -1,    18,    28,
      52,    29,    67,    19,    67,    -1,    20,    28,    52,    29,
      67,    -1,    21,    35,    -1,    22,    35,    -1,    23,    35,
      -1,    23,    56,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    68,    68,    72,    76,    81,    88,    89,    92,    93,
      94,    97,   104,   111,   118,   127,   131,   138,   142,   149,
     153,   160,   164,   173,   174,   177,   182,   186,   192,   193,
     196,   200,   204,   208,   214,   215,   220,   227,   228,   229,
     232,   233,   236,   237,   240,   241,   242,   245,   246,   247,
     248,   249,   252,   253,   254,   257,   258,   259,   260,   263,
     264,   267,   273,   274,   275,   278,   284,   292,   296,   302,
     303,   308,   312,   318,   319,   322,   325,   330,   334,   340,
     341,   344,   345,   346,   347,   348,   354,   363,   369,   370,
     371,   372
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ADD", "SUB", "MUL", "DIV", "MOD",
  "ASSIGN", "EQ", "NEQ", "LT", "LTE", "GT", "GTE", "AND", "OR", "NOT",
  "IF", "ELSE", "WHILE", "BREAK", "CONTINUE", "RETURN", "CONST", "INT",
  "VOID", "FLOAT", "LEFT_PARENTHESES", "RIGHT_PARENTHESES",
  "LEFT_BRACKETS", "RIGHT_BRACKETS", "LEFT_BRACES", "RIGHT_BRACES",
  "COMMA", "SEMICOLON", "IDENTIFIER", "INTEGER", "FLOATPOINT", "$accept",
  "compunit", "decl", "basic_type", "const_decl", "var_decl", "number",
  "identifier", "array_id", "array_val", "array_vals", "lval",
  "primaryexp", "lorexp", "landexp", "eqexp", "relexp", "addexp", "mulexp",
  "unaryexp", "func_def", "func_formals", "func_formal",
  "func_array_formal", "func_actuals", "block", "block_items",
  "block_item", "statement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    39,    40,    40,    40,    40,    41,    41,    42,    42,
      42,    43,    43,    43,    43,    44,    44,    44,    44,    44,
      44,    44,    44,    45,    45,    46,    47,    47,    48,    48,
      49,    49,    49,    49,    50,    50,    50,    51,    51,    51,
      52,    52,    53,    53,    54,    54,    54,    55,    55,    55,
      55,    55,    56,    56,    56,    57,    57,    57,    57,    58,
      58,    58,    58,    58,    58,    59,    59,    60,    60,    61,
      61,    62,    62,    63,    63,    64,    64,    65,    65,    66,
      66,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     1,     1,     2,     2,     1,     1,
       1,     5,     5,     5,     5,     2,     4,     3,     5,     2,
       4,     3,     5,     1,     1,     1,     4,     4,     2,     3,
       1,     1,     3,     3,     1,     4,     4,     3,     1,     1,
       1,     3,     1,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       3,     4,     2,     2,     2,     5,     6,     1,     3,     2,
       1,     4,     4,     1,     3,     2,     3,     1,     2,     1,
       1,     4,     1,     2,     1,     5,     7,     5,     2,     2,
       2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     8,    10,     9,     0,     4,     0,     0,     0,
       5,     0,     1,     2,     3,    25,    15,    19,     0,     6,
       0,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,    21,     0,     0,     0,     0,     0,     0,    23,
      24,    39,    34,    38,    59,    16,    52,    55,     0,     0,
       0,    67,    70,     0,     0,    20,     0,     0,     0,     0,
       0,    11,    13,    62,    63,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    65,    69,     0,     0,
       0,    26,    28,    31,     0,    30,    27,    12,    14,    18,
      22,    37,    60,    73,     0,     0,     0,    53,    54,    56,
      57,    58,     0,     0,     0,     0,     0,    75,    82,    79,
       0,    38,     0,    84,     0,    77,    80,     0,    66,    68,
       0,    29,     0,    61,     0,    35,    36,     0,     0,    88,
      89,    90,     0,    15,     0,    83,    76,    78,    71,    72,
      33,    32,    74,     0,    40,    42,    44,    47,     0,    91,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    81,    41,    85,    43,    45,    46,    50,    51,    48,
      49,    87,     0,    86
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,   109,     7,     8,     9,    41,    42,    17,    55,
      84,    43,    44,   143,   144,   145,   146,   147,    46,    47,
      10,    50,    51,    52,    94,   113,   114,   115,   116
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -69
static const yytype_int16 yypact[] =
{
     223,   231,   -69,   -69,   -69,   194,   -69,   -23,    96,   139,
     -69,   -23,   -69,   -69,   -69,   -69,     4,    69,   -23,   -69,
     -23,   -69,   119,   197,   187,   216,   187,    -4,   187,   198,
     203,   204,   205,   187,    -4,   187,   187,   187,   187,   -69,
     -69,   -69,   114,    19,   -69,   259,   254,   -69,    20,   -23,
      -3,   -69,    30,   112,    33,   -69,   115,   187,    -4,   187,
      -4,   259,   -69,   -69,   -69,   -69,    38,   172,   187,   187,
     187,   187,   187,   187,   187,   134,   -69,    55,    20,   231,
     187,   -69,   -69,   -69,   232,   259,   -69,   259,   -69,   259,
     -69,   -69,   -69,   259,    49,   122,   137,   254,   254,   -69,
     -69,   -69,    65,    73,    76,   104,    59,   -69,   -69,   -69,
     -23,   206,    12,   -69,   160,   -69,   -69,   148,   -69,   -69,
     199,   -69,    36,   -69,   187,   -69,   -69,   187,   187,   -69,
     -69,   -69,    51,   208,   187,   -69,   -69,   -69,   -69,   -69,
     -69,   259,   259,   -11,   130,   258,   241,   259,    -5,   -69,
      77,   187,    85,   187,   187,   187,   187,   187,   187,   187,
      85,   -69,   130,   128,   258,   241,   241,   259,   259,   259,
     259,   -69,    85,   -69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -69,   -69,   124,     0,   -69,   -69,   -69,    -1,   226,   -31,
     -69,   -68,   -69,    22,    14,    46,   116,   -24,   202,   -15,
     212,   -69,   143,   -69,   -69,   -40,   -69,    93,    79
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      45,    11,    53,    62,    56,   151,    16,   111,    76,    61,
      22,   151,    24,    15,    66,    70,    71,    29,   152,    31,
      63,    64,    65,    83,   160,    49,    78,    88,    54,    90,
      85,    79,    25,    87,    26,    89,    35,    36,   118,    35,
      36,    70,    71,    93,    95,    96,   111,   135,    77,    69,
      37,   112,    75,    37,    70,    71,   120,    99,   100,   101,
      80,    38,    35,    36,    38,    54,    82,    91,    54,    15,
      39,    40,    15,    39,    40,   110,    37,    27,   123,    49,
      70,    71,   132,   124,   111,   117,   149,    38,    35,    36,
     112,   140,   111,   127,   131,    15,    39,    40,   141,    28,
     142,   128,    37,   102,   111,   103,   104,   105,   106,   133,
     150,   129,   161,    38,   110,    70,    71,    75,    70,    71,
     108,    15,    39,    40,     6,    70,    71,    33,   112,    13,
      18,    19,   167,   168,   169,   170,   112,    35,    36,   130,
      70,    71,    67,    81,    68,   153,    86,   172,   112,    26,
     148,    37,   102,   125,   103,   104,   105,   106,     1,     2,
       3,     4,    38,    35,    36,   162,    75,   107,   126,   108,
      15,    39,    40,    20,    21,    35,    36,    37,   102,   138,
     103,   104,   105,   106,     1,     2,     3,     4,    38,    37,
      35,    36,    75,   136,    12,   108,    15,    39,    40,   164,
      38,    92,    70,    71,    37,    34,    57,   137,    15,    39,
      40,    58,    59,    60,   134,    38,    24,    14,     1,     2,
       3,     4,   119,    15,    39,    40,     0,    28,    26,     0,
     139,   163,     0,    28,    26,    28,    69,    23,    26,   171,
       0,     2,     3,     4,    30,    48,    32,     1,     2,     3,
       4,   173,   156,   157,   158,   159,     2,     3,     4,    72,
      73,    74,    70,    71,     0,   121,   122,   154,   155,     0,
     165,   166,    97,    98
};

static const yytype_int16 yycheck[] =
{
      24,     1,    26,    34,    28,    16,     7,    75,    48,    33,
      11,    16,     8,    36,    38,     3,     4,    18,    29,    20,
      35,    36,    37,    54,    29,    25,    29,    58,    32,    60,
      54,    34,    28,    57,    30,    59,     3,     4,    78,     3,
       4,     3,     4,    67,    68,    69,   114,    35,    49,    30,
      17,    75,    32,    17,     3,     4,    80,    72,    73,    74,
      30,    28,     3,     4,    28,    32,    33,    29,    32,    36,
      37,    38,    36,    37,    38,    75,    17,     8,    29,    79,
       3,     4,   106,    34,   152,    30,    35,    28,     3,     4,
     114,   122,   160,    28,    35,    36,    37,    38,   122,    30,
     124,    28,    17,    18,   172,    20,    21,    22,    23,   110,
     134,    35,    35,    28,   114,     3,     4,    32,     3,     4,
      35,    36,    37,    38,     0,     3,     4,     8,   152,     5,
      34,    35,   156,   157,   158,   159,   160,     3,     4,    35,
       3,     4,    28,    31,    30,    15,    31,    19,   172,    30,
     128,    17,    18,    31,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     3,     4,   151,    32,    33,    31,    35,
      36,    37,    38,    34,    35,     3,     4,    17,    18,    31,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    17,
       3,     4,    32,    33,     0,    35,    36,    37,    38,   153,
      28,    29,     3,     4,    17,     8,     8,   114,    36,    37,
      38,     8,     8,     8,     8,    28,     8,     5,    24,    25,
      26,    27,    79,    36,    37,    38,    -1,    30,    30,    -1,
      31,   152,    -1,    30,    30,    30,    30,    11,    30,   160,
      -1,    25,    26,    27,    18,    29,    20,    24,    25,    26,
      27,   172,    11,    12,    13,    14,    25,    26,    27,     5,
       6,     7,     3,     4,    -1,    33,    34,     9,    10,    -1,
     154,   155,    70,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    24,    25,    26,    27,    40,    41,    42,    43,    44,
      59,    42,     0,    41,    59,    36,    46,    47,    34,    35,
      34,    35,    46,    47,     8,    28,    30,     8,    30,    46,
      47,    46,    47,     8,     8,     3,     4,    17,    28,    37,
      38,    45,    46,    50,    51,    56,    57,    58,    29,    42,
      60,    61,    62,    56,    32,    48,    56,     8,     8,     8,
       8,    56,    48,    58,    58,    58,    56,    28,    30,    30,
       3,     4,     5,     6,     7,    32,    64,    46,    29,    34,
      30,    31,    33,    48,    49,    56,    31,    56,    48,    56,
      48,    29,    29,    56,    63,    56,    56,    57,    57,    58,
      58,    58,    18,    20,    21,    22,    23,    33,    35,    41,
      42,    50,    56,    64,    65,    66,    67,    30,    64,    61,
      56,    33,    34,    29,    34,    31,    31,    28,    28,    35,
      35,    35,    56,    46,     8,    35,    33,    66,    31,    31,
      48,    56,    56,    52,    53,    54,    55,    56,    52,    35,
      56,    16,    29,    15,     9,    10,    11,    12,    13,    14,
      29,    35,    53,    67,    54,    55,    55,    56,    56,    56,
      56,    67,    19,    67
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 68 "parser/parser.y"
    {
            (yyval.root) = (yyvsp[(1) - (2)].root);
            (yyval.root)->addDeclares(std::shared_ptr<Declare>((yyvsp[(2) - (2)].declare)));
      ;}
    break;

  case 3:
#line 72 "parser/parser.y"
    {
            (yyval.root) = (yyvsp[(1) - (2)].root);
            (yyval.root)->addFuncDefine(std::shared_ptr<FuncDefine>((yyvsp[(2) - (2)].funcdef)));
      ;}
    break;

  case 4:
#line 76 "parser/parser.y"
    {
            root = std::make_shared<CompUnit>();
            (yyval.root) = root.get();
            (yyval.root)->addDeclares(std::shared_ptr<Declare>((yyvsp[(1) - (1)].declare)));
      ;}
    break;

  case 5:
#line 81 "parser/parser.y"
    {
            root = std::make_shared<CompUnit>();
            (yyval.root) = root.get();
            (yyval.root)->addFuncDefine(std::shared_ptr<FuncDefine>((yyvsp[(1) - (1)].funcdef)));
      ;}
    break;

  case 6:
#line 88 "parser/parser.y"
    { (yyval.declare) = (yyvsp[(1) - (2)].declare);;}
    break;

  case 7:
#line 89 "parser/parser.y"
    { (yyval.declare) = (yyvsp[(1) - (2)].declare); ;}
    break;

  case 8:
#line 92 "parser/parser.y"
    { (yyval.type) = BasicType::INT_BTYPE; ;}
    break;

  case 9:
#line 93 "parser/parser.y"
    { (yyval.type) = BasicType::FLOAT_BTYPE; ;}
    break;

  case 10:
#line 94 "parser/parser.y"
    { (yyval.type) = BasicType::VOID_BTYPE; ;}
    break;

  case 11:
#line 97 "parser/parser.y"
    {
            (yyval.declare) = new ConstDeclare((yyvsp[(2) - (5)].type));
            (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                std::shared_ptr<Expression>((yyvsp[(5) - (5)].expr))
            ));
      ;}
    break;

  case 12:
#line 104 "parser/parser.y"
    {
            (yyval.declare) = (yyvsp[(1) - (5)].declare);
            (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                            std::shared_ptr<Expression>((yyvsp[(5) - (5)].expr))
                        ));
      ;}
    break;

  case 13:
#line 111 "parser/parser.y"
    {
             (yyval.declare) = new ConstDeclare((yyvsp[(2) - (5)].type));
             (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                            std::shared_ptr<Expression>((yyvsp[(5) - (5)].array_value))
             ));
      ;}
    break;

  case 14:
#line 118 "parser/parser.y"
    {
             (yyval.declare) = (yyvsp[(1) - (5)].declare);
             (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                            std::shared_ptr<Expression>((yyvsp[(5) - (5)].array_value))
             ));
      ;}
    break;

  case 15:
#line 127 "parser/parser.y"
    {
            (yyval.declare) = new VarDeclare((yyvsp[(1) - (2)].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[(2) - (2)].ident))));
      ;}
    break;

  case 16:
#line 131 "parser/parser.y"
    {
             (yyval.declare) = new VarDeclare((yyvsp[(1) - (4)].type));
             (yyval.declare)->addDef(std::make_shared<VarDefine>(
                std::shared_ptr<Identifier>((yyvsp[(2) - (4)].ident)),
                std::shared_ptr<Expression>((yyvsp[(4) - (4)].expr))
             ));
      ;}
    break;

  case 17:
#line 138 "parser/parser.y"
    {
            (yyval.declare) = (yyvsp[(1) - (3)].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[(3) - (3)].ident))));
      ;}
    break;

  case 18:
#line 142 "parser/parser.y"
    {
            (yyval.declare) = (yyvsp[(1) - (5)].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                            std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                            std::shared_ptr<Expression>((yyvsp[(5) - (5)].expr))
            ));
      ;}
    break;

  case 19:
#line 149 "parser/parser.y"
    {
            (yyval.declare) = new VarDeclare((yyvsp[(1) - (2)].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[(2) - (2)].ident))));
      ;}
    break;

  case 20:
#line 153 "parser/parser.y"
    {
            (yyval.declare) = new VarDeclare((yyvsp[(1) - (4)].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                       std::shared_ptr<Identifier>((yyvsp[(2) - (4)].ident)),
                       std::shared_ptr<Expression>((yyvsp[(4) - (4)].array_value))
            ));
      ;}
    break;

  case 21:
#line 160 "parser/parser.y"
    {
            (yyval.declare) = (yyvsp[(1) - (3)].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[(3) - (3)].ident))));
      ;}
    break;

  case 22:
#line 164 "parser/parser.y"
    {
            (yyval.declare) = (yyvsp[(1) - (5)].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                       std::shared_ptr<Identifier>((yyvsp[(3) - (5)].ident)),
                       std::shared_ptr<Expression>((yyvsp[(5) - (5)].array_value)))
            );
      ;}
    break;

  case 23:
#line 173 "parser/parser.y"
    { (yyval.expr) = new Number((yyvsp[(1) - (1)].int_val)); ;}
    break;

  case 24:
#line 174 "parser/parser.y"
    { (yyval.expr) = new Number((yyvsp[(1) - (1)].float_val)); ;}
    break;

  case 25:
#line 177 "parser/parser.y"
    {
        (yyval.ident) = new Identifier((yyvsp[(1) - (1)].string));
    ;}
    break;

  case 26:
#line 182 "parser/parser.y"
    {
                (yyval.ident) = (yyvsp[(1) - (4)].ident);
                (yyval.ident)->addDimension(std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr)));
        ;}
    break;

  case 27:
#line 186 "parser/parser.y"
    {
                (yyval.ident) = (yyvsp[(1) - (4)].ident);
                (yyval.ident)->addDimension(std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr)));
        ;}
    break;

  case 28:
#line 192 "parser/parser.y"
    { (yyval.array_value) = new ArrayValue(false); ;}
    break;

  case 29:
#line 193 "parser/parser.y"
    { (yyval.array_value) = (yyvsp[(2) - (3)].array_value); ;}
    break;

  case 30:
#line 196 "parser/parser.y"
    {
            (yyval.array_value) = new ArrayValue(false);
            (yyval.array_value)->addArrayValue(std::make_shared<ArrayValue>(true, std::shared_ptr<Expression>((yyvsp[(1) - (1)].expr))));
      ;}
    break;

  case 31:
#line 200 "parser/parser.y"
    {
            (yyval.array_value) = new ArrayValue(false);
            (yyval.array_value)->addArrayValue(std::shared_ptr<ArrayValue>((yyvsp[(1) - (1)].array_value)));
      ;}
    break;

  case 32:
#line 204 "parser/parser.y"
    {
            (yyval.array_value) = (yyvsp[(1) - (3)].array_value);
            (yyval.array_value)->addArrayValue(std::make_shared<ArrayValue>(true, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))));
      ;}
    break;

  case 33:
#line 208 "parser/parser.y"
    {
            (yyval.array_value) = (yyvsp[(1) - (3)].array_value);
            (yyval.array_value)->addArrayValue(std::shared_ptr<ArrayValue>((yyvsp[(3) - (3)].array_value)));
      ;}
    break;

  case 34:
#line 214 "parser/parser.y"
    { (yyval.expr) = new LvalExpr(std::shared_ptr<Identifier>((yyvsp[(1) - (1)].ident))); ;}
    break;

  case 35:
#line 215 "parser/parser.y"
    {
            auto lval = new LvalExpr(std::shared_ptr<Identifier>((yyvsp[(1) - (4)].ident)));
            lval->getId()->addDimension(std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr)));
            (yyval.expr) = lval;
      ;}
    break;

  case 36:
#line 220 "parser/parser.y"
    {
            auto lval = dynamic_cast<LvalExpr*>((yyvsp[(1) - (4)].expr));
            lval->getId()->addDimension(std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr)));
            (yyval.expr) = (yyvsp[(1) - (4)].expr);
      ;}
    break;

  case 37:
#line 227 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 38:
#line 228 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 39:
#line 229 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 40:
#line 232 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 41:
#line 233 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::OR_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 42:
#line 236 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 43:
#line 237 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::AND_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 44:
#line 240 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 45:
#line 241 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::EQ_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 46:
#line 242 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::NEQ_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 47:
#line 245 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 48:
#line 246 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::GT_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 49:
#line 247 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::GTE_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 50:
#line 248 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::LT_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 51:
#line 249 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::LTE_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 52:
#line 252 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 53:
#line 253 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::ADD_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 54:
#line 254 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::SUB_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 55:
#line 257 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 56:
#line 258 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::MUL_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 57:
#line 259 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::DIV_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 58:
#line 260 "parser/parser.y"
    { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[(1) - (3)].expr)), BinaryOpType::MOD_OPTYPE, std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 59:
#line 263 "parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 60:
#line 264 "parser/parser.y"
    {
            (yyval.expr) = new CallFuncExpr(std::shared_ptr<Identifier>((yyvsp[(1) - (3)].ident)));
      ;}
    break;

  case 61:
#line 267 "parser/parser.y"
    {
            (yyval.expr) = new CallFuncExpr(
                std::shared_ptr<Identifier>((yyvsp[(1) - (4)].ident)),
                std::shared_ptr<FuncRParams>((yyvsp[(3) - (4)].actuals))
            );
      ;}
    break;

  case 62:
#line 273 "parser/parser.y"
    { (yyval.expr) = new UnaryExpr(UnaryOpType::POSITIVE_OPTYPE, std::shared_ptr<Expression>((yyvsp[(2) - (2)].expr))); ;}
    break;

  case 63:
#line 274 "parser/parser.y"
    { (yyval.expr) = new UnaryExpr(UnaryOpType::NEGATIVE_OPTYPE, std::shared_ptr<Expression>((yyvsp[(2) - (2)].expr))); ;}
    break;

  case 64:
#line 275 "parser/parser.y"
    { (yyval.expr) = new UnaryExpr(UnaryOpType::NOTOP_OPTYPE, std::shared_ptr<Expression>((yyvsp[(2) - (2)].expr))); ;}
    break;

  case 65:
#line 278 "parser/parser.y"
    {
            (yyval.funcdef) = new FuncDefine((yyvsp[(1) - (5)].type),
                std::shared_ptr<Identifier>((yyvsp[(2) - (5)].ident)),
                std::shared_ptr<FuncFParams>(),
                std::shared_ptr<Statement>((yyvsp[(5) - (5)].block)));
      ;}
    break;

  case 66:
#line 284 "parser/parser.y"
    {
            (yyval.funcdef) = new FuncDefine((yyvsp[(1) - (6)].type),
                     std::shared_ptr<Identifier>((yyvsp[(2) - (6)].ident)),
                     std::shared_ptr<FuncFParams>((yyvsp[(4) - (6)].formals)),
                     std::shared_ptr<Statement>((yyvsp[(6) - (6)].block)));
      ;}
    break;

  case 67:
#line 292 "parser/parser.y"
    {
            (yyval.formals) = new FuncFParams();
            (yyval.formals)->addFuncFormal(std::shared_ptr<FuncFParam>((yyvsp[(1) - (1)].formal)));
      ;}
    break;

  case 68:
#line 296 "parser/parser.y"
    {
            (yyval.formals) = (yyvsp[(1) - (3)].formals);
            (yyval.formals)->addFuncFormal(std::shared_ptr<FuncFParam>((yyvsp[(3) - (3)].formal)));
      ;}
    break;

  case 69:
#line 302 "parser/parser.y"
    { (yyval.formal) = new FuncFParam((yyvsp[(1) - (2)].type), std::shared_ptr<Identifier>((yyvsp[(2) - (2)].ident))); ;}
    break;

  case 70:
#line 303 "parser/parser.y"
    {
            (yyval.formal) = (yyvsp[(1) - (1)].formal);
      ;}
    break;

  case 71:
#line 308 "parser/parser.y"
    {
            (yyval.formal) = new FuncFParam((yyvsp[(1) - (4)].type), std::shared_ptr<Identifier>((yyvsp[(2) - (4)].ident)));
            (yyval.formal)->getFormalId()->addDimension(std::shared_ptr<Expression>(nullptr));
      ;}
    break;

  case 72:
#line 312 "parser/parser.y"
    {
            (yyval.formal) = (yyvsp[(1) - (4)].formal);
            (yyval.formal)->getFormalId()->addDimension(std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr)));
      ;}
    break;

  case 73:
#line 318 "parser/parser.y"
    { (yyval.actuals) = new FuncRParams(); (yyval.actuals)->addExpr(std::shared_ptr<Expression>((yyvsp[(1) - (1)].expr))); ;}
    break;

  case 74:
#line 319 "parser/parser.y"
    { (yyval.actuals) = (yyvsp[(1) - (3)].actuals); (yyval.actuals)->addExpr(std::shared_ptr<Expression>((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 75:
#line 322 "parser/parser.y"
    {
            (yyval.block) = new BlockItems();
      ;}
    break;

  case 76:
#line 325 "parser/parser.y"
    {
            (yyval.block) = (yyvsp[(2) - (3)].block);
      ;}
    break;

  case 77:
#line 330 "parser/parser.y"
    {
            (yyval.block) = new BlockItems();
            if ((yyvsp[(1) - (1)].block_stmt) != nullptr) (yyval.block)->addItem(std::shared_ptr<BlockItem>((yyvsp[(1) - (1)].block_stmt)));
      ;}
    break;

  case 78:
#line 334 "parser/parser.y"
    {
            (yyval.block) = (yyvsp[(1) - (2)].block);
            if ((yyvsp[(1) - (2)].block) != nullptr) (yyval.block)->addItem(std::shared_ptr<BlockItem>((yyvsp[(2) - (2)].block_stmt)));
      ;}
    break;

  case 79:
#line 340 "parser/parser.y"
    { (yyval.block_stmt) = new BlockItem(std::shared_ptr<Declare>((yyvsp[(1) - (1)].declare))); ;}
    break;

  case 80:
#line 341 "parser/parser.y"
    { (yyval.block_stmt) = new BlockItem(std::shared_ptr<Statement>((yyvsp[(1) - (1)].stmt))); ;}
    break;

  case 81:
#line 344 "parser/parser.y"
    { (yyval.stmt) = new AssignStatement(std::shared_ptr<Expression>((yyvsp[(1) - (4)].expr)), std::shared_ptr<Expression>((yyvsp[(3) - (4)].expr))); ;}
    break;

  case 82:
#line 345 "parser/parser.y"
    { (yyval.stmt) = nullptr; ;}
    break;

  case 83:
#line 346 "parser/parser.y"
    { (yyval.stmt) = new EvalStatement(std::shared_ptr<Expression>((yyvsp[(1) - (2)].expr))); ;}
    break;

  case 84:
#line 347 "parser/parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].block); ;}
    break;

  case 85:
#line 348 "parser/parser.y"
    {
            auto then_stmt_block = std::make_shared<BlockItems>();
            auto then_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[(5) - (5)].stmt)));
            then_stmt_block->addItem(then_stmt_block_item);
            (yyval.stmt) = new IfElseStatement(std::shared_ptr<Expression>((yyvsp[(3) - (5)].expr)), then_stmt_block);
      ;}
    break;

  case 86:
#line 354 "parser/parser.y"
    {
            auto then_stmt_block = std::make_shared<BlockItems>();
            auto then_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[(5) - (7)].stmt)));
            then_stmt_block->addItem(then_stmt_block_item);
            auto else_stmt_block = std::make_shared<BlockItems>();
            auto else_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[(7) - (7)].stmt)));
            else_stmt_block->addItem(else_stmt_block_item);
            (yyval.stmt) = new IfElseStatement(std::shared_ptr<Expression>((yyvsp[(3) - (7)].expr)), then_stmt_block, else_stmt_block);
      ;}
    break;

  case 87:
#line 363 "parser/parser.y"
    {
            auto block_items = new BlockItems();
            auto block_item = new BlockItem(std::shared_ptr<Statement>((yyvsp[(5) - (5)].stmt)));
            block_items->addItem(std::shared_ptr<BlockItem>(block_item));
            (yyval.stmt) = new WhileStatement(std::shared_ptr<Expression>((yyvsp[(3) - (5)].expr)), std::shared_ptr<Statement>(block_items));
      ;}
    break;

  case 88:
#line 369 "parser/parser.y"
    { (yyval.stmt) = new BreakStatement(); ;}
    break;

  case 89:
#line 370 "parser/parser.y"
    { (yyval.stmt) = new ContinueStatement(); ;}
    break;

  case 90:
#line 371 "parser/parser.y"
    { (yyval.stmt) = new ReturnStatement(); ;}
    break;

  case 91:
#line 372 "parser/parser.y"
    { (yyval.stmt) = new ReturnStatement(std::shared_ptr<Expression>((yyvsp[(2) - (3)].expr))); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2238 "parser/Parser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 375 "parser/parser.y"


void yyerror(const char* s) {
    printf("line:%d\tcolumn:%d\n", yylloc.first_line, yylloc.first_column);
    printf("ERROR: %s\n", s);
}

