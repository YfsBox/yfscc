/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser/parser.y"

#include <cstdlib>
#include <cstdio>
#include "../common/Types.h"
#include "../common/Ast.h"

std::shared_ptr<CompUnit> root;

extern int yylex(void);
extern void yyerror(const char *s);


#line 83 "parser/Parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PARSER_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "parser/parser.y"

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

#line 194 "parser/Parser.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_PARSER_PARSER_HPP_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

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
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  174

#define YYUNDEFTOK  2
#define YYMAXUTOK   293


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
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

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "block_item", "statement", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

#define YYPACT_NINF (-69)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
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

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
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

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -69,   -69,   124,     0,   -69,   -69,   -69,    -1,   226,   -31,
     -69,   -68,   -69,    22,    14,    46,   116,   -24,   202,   -15,
     212,   -69,   143,   -69,   -69,   -40,   -69,    93,    79
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,   109,     7,     8,     9,    41,    42,    17,    55,
      84,    43,    44,   143,   144,   145,   146,   147,    46,    47,
      10,    50,    51,    52,    94,   113,   114,   115,   116
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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
static const yytype_int8 yystos[] =
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

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
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


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 68 "parser/parser.y"
                        {
            (yyval.root) = (yyvsp[-1].root);
            (yyval.root)->addDeclares(std::shared_ptr<Declare>((yyvsp[0].declare)));
      }
#line 1624 "parser/Parser.cpp"
    break;

  case 3:
#line 72 "parser/parser.y"
                          {
            (yyval.root) = (yyvsp[-1].root);
            (yyval.root)->addFuncDefine(std::shared_ptr<FuncDefine>((yyvsp[0].funcdef)));
      }
#line 1633 "parser/Parser.cpp"
    break;

  case 4:
#line 76 "parser/parser.y"
             {
            root = std::make_shared<CompUnit>();
            (yyval.root) = root.get();
            (yyval.root)->addDeclares(std::shared_ptr<Declare>((yyvsp[0].declare)));
      }
#line 1643 "parser/Parser.cpp"
    break;

  case 5:
#line 81 "parser/parser.y"
                 {
            root = std::make_shared<CompUnit>();
            (yyval.root) = root.get();
            (yyval.root)->addFuncDefine(std::shared_ptr<FuncDefine>((yyvsp[0].funcdef)));
      }
#line 1653 "parser/Parser.cpp"
    break;

  case 6:
#line 88 "parser/parser.y"
                           { (yyval.declare) = (yyvsp[-1].declare);}
#line 1659 "parser/Parser.cpp"
    break;

  case 7:
#line 89 "parser/parser.y"
                         { (yyval.declare) = (yyvsp[-1].declare); }
#line 1665 "parser/Parser.cpp"
    break;

  case 8:
#line 92 "parser/parser.y"
                { (yyval.type) = BasicType::INT_BTYPE; }
#line 1671 "parser/Parser.cpp"
    break;

  case 9:
#line 93 "parser/parser.y"
            { (yyval.type) = BasicType::FLOAT_BTYPE; }
#line 1677 "parser/Parser.cpp"
    break;

  case 10:
#line 94 "parser/parser.y"
           { (yyval.type) = BasicType::VOID_BTYPE; }
#line 1683 "parser/Parser.cpp"
    break;

  case 11:
#line 97 "parser/parser.y"
                                                      {
            (yyval.declare) = new ConstDeclare((yyvsp[-3].type));
            (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                std::shared_ptr<Expression>((yyvsp[0].expr))
            ));
      }
#line 1695 "parser/Parser.cpp"
    break;

  case 12:
#line 104 "parser/parser.y"
                                                  {
            (yyval.declare) = (yyvsp[-4].declare);
            (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                            std::shared_ptr<Expression>((yyvsp[0].expr))
                        ));
      }
#line 1707 "parser/Parser.cpp"
    break;

  case 13:
#line 111 "parser/parser.y"
                                                   {
             (yyval.declare) = new ConstDeclare((yyvsp[-3].type));
             (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                            std::shared_ptr<Expression>((yyvsp[0].array_value))
             ));
      }
#line 1719 "parser/Parser.cpp"
    break;

  case 14:
#line 118 "parser/parser.y"
                                                   {
             (yyval.declare) = (yyvsp[-4].declare);
             (yyval.declare)->addDef(std::make_shared<ConstDefine>(
                            std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                            std::shared_ptr<Expression>((yyvsp[0].array_value))
             ));
      }
#line 1731 "parser/Parser.cpp"
    break;

  case 15:
#line 127 "parser/parser.y"
                                {
            (yyval.declare) = new VarDeclare((yyvsp[-1].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[0].ident))));
      }
#line 1740 "parser/Parser.cpp"
    break;

  case 16:
#line 131 "parser/parser.y"
                                            {
             (yyval.declare) = new VarDeclare((yyvsp[-3].type));
             (yyval.declare)->addDef(std::make_shared<VarDefine>(
                std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                std::shared_ptr<Expression>((yyvsp[0].expr))
             ));
      }
#line 1752 "parser/Parser.cpp"
    break;

  case 17:
#line 138 "parser/parser.y"
                                  {
            (yyval.declare) = (yyvsp[-2].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[0].ident))));
      }
#line 1761 "parser/Parser.cpp"
    break;

  case 18:
#line 142 "parser/parser.y"
                                                {
            (yyval.declare) = (yyvsp[-4].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                            std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                            std::shared_ptr<Expression>((yyvsp[0].expr))
            ));
      }
#line 1773 "parser/Parser.cpp"
    break;

  case 19:
#line 149 "parser/parser.y"
                            {
            (yyval.declare) = new VarDeclare((yyvsp[-1].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[0].ident))));
      }
#line 1782 "parser/Parser.cpp"
    break;

  case 20:
#line 153 "parser/parser.y"
                                             {
            (yyval.declare) = new VarDeclare((yyvsp[-3].type));
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                       std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                       std::shared_ptr<Expression>((yyvsp[0].array_value))
            ));
      }
#line 1794 "parser/Parser.cpp"
    break;

  case 21:
#line 160 "parser/parser.y"
                                {
            (yyval.declare) = (yyvsp[-2].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(std::shared_ptr<Identifier>((yyvsp[0].ident))));
      }
#line 1803 "parser/Parser.cpp"
    break;

  case 22:
#line 164 "parser/parser.y"
                                                 {
            (yyval.declare) = (yyvsp[-4].declare);
            (yyval.declare)->addDef(std::make_shared<VarDefine>(
                       std::shared_ptr<Identifier>((yyvsp[-2].ident)),
                       std::shared_ptr<Expression>((yyvsp[0].array_value)))
            );
      }
#line 1815 "parser/Parser.cpp"
    break;

  case 23:
#line 173 "parser/parser.y"
                { (yyval.expr) = new Number((yyvsp[0].int_val)); }
#line 1821 "parser/Parser.cpp"
    break;

  case 24:
#line 174 "parser/parser.y"
                   { (yyval.expr) = new Number((yyvsp[0].float_val)); }
#line 1827 "parser/Parser.cpp"
    break;

  case 25:
#line 177 "parser/parser.y"
                       {
        (yyval.ident) = new Identifier((yyvsp[0].string));
    }
#line 1835 "parser/Parser.cpp"
    break;

  case 26:
#line 182 "parser/parser.y"
                                                         {
                (yyval.ident) = (yyvsp[-3].ident);
                (yyval.ident)->addDimension(std::shared_ptr<Expression>((yyvsp[-1].expr)));
        }
#line 1844 "parser/Parser.cpp"
    break;

  case 27:
#line 186 "parser/parser.y"
                                                       {
                (yyval.ident) = (yyvsp[-3].ident);
                (yyval.ident)->addDimension(std::shared_ptr<Expression>((yyvsp[-1].expr)));
        }
#line 1853 "parser/Parser.cpp"
    break;

  case 28:
#line 192 "parser/parser.y"
                                    { (yyval.array_value) = new ArrayValue(false); }
#line 1859 "parser/Parser.cpp"
    break;

  case 29:
#line 193 "parser/parser.y"
                                            { (yyval.array_value) = (yyvsp[-1].array_value); }
#line 1865 "parser/Parser.cpp"
    break;

  case 30:
#line 196 "parser/parser.y"
                    {
            (yyval.array_value) = new ArrayValue(false);
            (yyval.array_value)->addArrayValue(std::make_shared<ArrayValue>(true, std::shared_ptr<Expression>((yyvsp[0].expr))));
      }
#line 1874 "parser/Parser.cpp"
    break;

  case 31:
#line 200 "parser/parser.y"
                  {
            (yyval.array_value) = new ArrayValue(false);
            (yyval.array_value)->addArrayValue(std::shared_ptr<ArrayValue>((yyvsp[0].array_value)));
      }
#line 1883 "parser/Parser.cpp"
    break;

  case 32:
#line 204 "parser/parser.y"
                                {
            (yyval.array_value) = (yyvsp[-2].array_value);
            (yyval.array_value)->addArrayValue(std::make_shared<ArrayValue>(true, std::shared_ptr<Expression>((yyvsp[0].expr))));
      }
#line 1892 "parser/Parser.cpp"
    break;

  case 33:
#line 208 "parser/parser.y"
                                   {
            (yyval.array_value) = (yyvsp[-2].array_value);
            (yyval.array_value)->addArrayValue(std::shared_ptr<ArrayValue>((yyvsp[0].array_value)));
      }
#line 1901 "parser/Parser.cpp"
    break;

  case 34:
#line 214 "parser/parser.y"
                 { (yyval.expr) = new LvalExpr(std::shared_ptr<Identifier>((yyvsp[0].ident))); }
#line 1907 "parser/Parser.cpp"
    break;

  case 35:
#line 215 "parser/parser.y"
                                                       {
            auto lval = new LvalExpr(std::shared_ptr<Identifier>((yyvsp[-3].ident)));
            lval->getId()->addDimension(std::shared_ptr<Expression>((yyvsp[-1].expr)));
            (yyval.expr) = lval;
      }
#line 1917 "parser/Parser.cpp"
    break;

  case 36:
#line 220 "parser/parser.y"
                                                 {
            auto lval = dynamic_cast<LvalExpr*>((yyvsp[-3].expr));
            lval->getId()->addDimension(std::shared_ptr<Expression>((yyvsp[-1].expr)));
            (yyval.expr) = (yyvsp[-3].expr);
      }
#line 1927 "parser/Parser.cpp"
    break;

  case 37:
#line 227 "parser/parser.y"
                                                      { (yyval.expr) = (yyvsp[-1].expr); }
#line 1933 "parser/Parser.cpp"
    break;

  case 38:
#line 228 "parser/parser.y"
             { (yyval.expr) = (yyvsp[0].expr); }
#line 1939 "parser/Parser.cpp"
    break;

  case 39:
#line 229 "parser/parser.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 1945 "parser/Parser.cpp"
    break;

  case 40:
#line 232 "parser/parser.y"
                { (yyval.expr) = (yyvsp[0].expr); }
#line 1951 "parser/Parser.cpp"
    break;

  case 41:
#line 233 "parser/parser.y"
                          { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::OR_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 1957 "parser/Parser.cpp"
    break;

  case 42:
#line 236 "parser/parser.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 1963 "parser/Parser.cpp"
    break;

  case 43:
#line 237 "parser/parser.y"
                           { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::AND_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 1969 "parser/Parser.cpp"
    break;

  case 44:
#line 240 "parser/parser.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1975 "parser/Parser.cpp"
    break;

  case 45:
#line 241 "parser/parser.y"
                        { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::EQ_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 1981 "parser/Parser.cpp"
    break;

  case 46:
#line 242 "parser/parser.y"
                        { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::NEQ_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 1987 "parser/Parser.cpp"
    break;

  case 47:
#line 245 "parser/parser.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 1993 "parser/Parser.cpp"
    break;

  case 48:
#line 246 "parser/parser.y"
                          { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::GT_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 1999 "parser/Parser.cpp"
    break;

  case 49:
#line 247 "parser/parser.y"
                          { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::GTE_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2005 "parser/Parser.cpp"
    break;

  case 50:
#line 248 "parser/parser.y"
                          { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::LT_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2011 "parser/Parser.cpp"
    break;

  case 51:
#line 249 "parser/parser.y"
                          { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::LTE_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2017 "parser/Parser.cpp"
    break;

  case 52:
#line 252 "parser/parser.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 2023 "parser/Parser.cpp"
    break;

  case 53:
#line 253 "parser/parser.y"
                           { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::ADD_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2029 "parser/Parser.cpp"
    break;

  case 54:
#line 254 "parser/parser.y"
                           { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::SUB_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2035 "parser/Parser.cpp"
    break;

  case 55:
#line 257 "parser/parser.y"
                 { (yyval.expr) = (yyvsp[0].expr); }
#line 2041 "parser/Parser.cpp"
    break;

  case 56:
#line 258 "parser/parser.y"
                             { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::MUL_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2047 "parser/Parser.cpp"
    break;

  case 57:
#line 259 "parser/parser.y"
                             { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::DIV_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2053 "parser/Parser.cpp"
    break;

  case 58:
#line 260 "parser/parser.y"
                             { (yyval.expr) = new BinaryExpr(std::shared_ptr<Expression>((yyvsp[-2].expr)), BinaryOpType::MOD_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2059 "parser/Parser.cpp"
    break;

  case 59:
#line 263 "parser/parser.y"
                     { (yyval.expr) = (yyvsp[0].expr); }
#line 2065 "parser/Parser.cpp"
    break;

  case 60:
#line 264 "parser/parser.y"
                                                      {
            (yyval.expr) = new CallFuncExpr(std::shared_ptr<Identifier>((yyvsp[-2].ident)));
      }
#line 2073 "parser/Parser.cpp"
    break;

  case 61:
#line 267 "parser/parser.y"
                                                                   {
            (yyval.expr) = new CallFuncExpr(
                std::shared_ptr<Identifier>((yyvsp[-3].ident)),
                std::shared_ptr<FuncRParams>((yyvsp[-1].actuals))
            );
      }
#line 2084 "parser/Parser.cpp"
    break;

  case 62:
#line 273 "parser/parser.y"
                      { (yyval.expr) = new UnaryExpr(UnaryOpType::POSITIVE_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2090 "parser/Parser.cpp"
    break;

  case 63:
#line 274 "parser/parser.y"
                      { (yyval.expr) = new UnaryExpr(UnaryOpType::NEGATIVE_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2096 "parser/Parser.cpp"
    break;

  case 64:
#line 275 "parser/parser.y"
                      { (yyval.expr) = new UnaryExpr(UnaryOpType::NOTOP_OPTYPE, std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2102 "parser/Parser.cpp"
    break;

  case 65:
#line 278 "parser/parser.y"
                                                                         {
            (yyval.funcdef) = new FuncDefine((yyvsp[-4].type),
                std::shared_ptr<Identifier>((yyvsp[-3].ident)),
                std::shared_ptr<FuncFParams>(),
                std::shared_ptr<Statement>((yyvsp[0].block)));
      }
#line 2113 "parser/Parser.cpp"
    break;

  case 66:
#line 284 "parser/parser.y"
                                                                                    {
            (yyval.funcdef) = new FuncDefine((yyvsp[-5].type),
                     std::shared_ptr<Identifier>((yyvsp[-4].ident)),
                     std::shared_ptr<FuncFParams>((yyvsp[-2].formals)),
                     std::shared_ptr<Statement>((yyvsp[0].block)));
      }
#line 2124 "parser/Parser.cpp"
    break;

  case 67:
#line 292 "parser/parser.y"
                          {
            (yyval.formals) = new FuncFParams();
            (yyval.formals)->addFuncFormal(std::shared_ptr<FuncFParam>((yyvsp[0].formal)));
      }
#line 2133 "parser/Parser.cpp"
    break;

  case 68:
#line 296 "parser/parser.y"
                                       {
            (yyval.formals) = (yyvsp[-2].formals);
            (yyval.formals)->addFuncFormal(std::shared_ptr<FuncFParam>((yyvsp[0].formal)));
      }
#line 2142 "parser/Parser.cpp"
    break;

  case 69:
#line 302 "parser/parser.y"
                                   { (yyval.formal) = new FuncFParam((yyvsp[-1].type), std::shared_ptr<Identifier>((yyvsp[0].ident))); }
#line 2148 "parser/Parser.cpp"
    break;

  case 70:
#line 303 "parser/parser.y"
                          {
            (yyval.formal) = (yyvsp[0].formal);
      }
#line 2156 "parser/Parser.cpp"
    break;

  case 71:
#line 308 "parser/parser.y"
                                                                      {
            (yyval.formal) = new FuncFParam((yyvsp[-3].type), std::shared_ptr<Identifier>((yyvsp[-2].ident)));
            (yyval.formal)->getFormalId()->addDimension(std::shared_ptr<Expression>(nullptr));
      }
#line 2165 "parser/Parser.cpp"
    break;

  case 72:
#line 312 "parser/parser.y"
                                                              {
            (yyval.formal) = (yyvsp[-3].formal);
            (yyval.formal)->getFormalId()->addDimension(std::shared_ptr<Expression>((yyvsp[-1].expr)));
      }
#line 2174 "parser/Parser.cpp"
    break;

  case 73:
#line 318 "parser/parser.y"
                     { (yyval.actuals) = new FuncRParams(); (yyval.actuals)->addExpr(std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2180 "parser/Parser.cpp"
    break;

  case 74:
#line 319 "parser/parser.y"
                                  { (yyval.actuals) = (yyvsp[-2].actuals); (yyval.actuals)->addExpr(std::shared_ptr<Expression>((yyvsp[0].expr))); }
#line 2186 "parser/Parser.cpp"
    break;

  case 75:
#line 322 "parser/parser.y"
                                {
            (yyval.block) = new BlockItems();
      }
#line 2194 "parser/Parser.cpp"
    break;

  case 76:
#line 325 "parser/parser.y"
                                            {
            (yyval.block) = (yyvsp[-1].block);
      }
#line 2202 "parser/Parser.cpp"
    break;

  case 77:
#line 330 "parser/parser.y"
                        {
            (yyval.block) = new BlockItems();
            if ((yyvsp[0].block_stmt) != nullptr) (yyval.block)->addItem(std::shared_ptr<BlockItem>((yyvsp[0].block_stmt)));
      }
#line 2211 "parser/Parser.cpp"
    break;

  case 78:
#line 334 "parser/parser.y"
                               {
            (yyval.block) = (yyvsp[-1].block);
            if ((yyvsp[-1].block) != nullptr) (yyval.block)->addItem(std::shared_ptr<BlockItem>((yyvsp[0].block_stmt)));
      }
#line 2220 "parser/Parser.cpp"
    break;

  case 79:
#line 340 "parser/parser.y"
                 { (yyval.block_stmt) = new BlockItem(std::shared_ptr<Declare>((yyvsp[0].declare))); }
#line 2226 "parser/Parser.cpp"
    break;

  case 80:
#line 341 "parser/parser.y"
                  { (yyval.block_stmt) = new BlockItem(std::shared_ptr<Statement>((yyvsp[0].stmt))); }
#line 2232 "parser/Parser.cpp"
    break;

  case 81:
#line 344 "parser/parser.y"
                                        { (yyval.stmt) = new AssignStatement(std::shared_ptr<Expression>((yyvsp[-3].expr)), std::shared_ptr<Expression>((yyvsp[-1].expr))); }
#line 2238 "parser/Parser.cpp"
    break;

  case 82:
#line 345 "parser/parser.y"
                  { (yyval.stmt) = nullptr; }
#line 2244 "parser/Parser.cpp"
    break;

  case 83:
#line 346 "parser/parser.y"
                         { (yyval.stmt) = new EvalStatement(std::shared_ptr<Expression>((yyvsp[-1].expr))); }
#line 2250 "parser/Parser.cpp"
    break;

  case 84:
#line 347 "parser/parser.y"
              { (yyval.stmt) = (yyvsp[0].block); }
#line 2256 "parser/Parser.cpp"
    break;

  case 85:
#line 348 "parser/parser.y"
                                                               {
            auto then_stmt_block = std::make_shared<BlockItems>();
            auto then_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[0].stmt)));
            then_stmt_block->addItem(then_stmt_block_item);
            (yyval.stmt) = new IfElseStatement(std::shared_ptr<Expression>((yyvsp[-2].expr)), then_stmt_block);
      }
#line 2267 "parser/Parser.cpp"
    break;

  case 86:
#line 354 "parser/parser.y"
                                                                              {
            auto then_stmt_block = std::make_shared<BlockItems>();
            auto then_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[-2].stmt)));
            then_stmt_block->addItem(then_stmt_block_item);
            auto else_stmt_block = std::make_shared<BlockItems>();
            auto else_stmt_block_item = std::make_shared<BlockItem>(std::shared_ptr<Statement>((yyvsp[0].stmt)));
            else_stmt_block->addItem(else_stmt_block_item);
            (yyval.stmt) = new IfElseStatement(std::shared_ptr<Expression>((yyvsp[-4].expr)), then_stmt_block, else_stmt_block);
      }
#line 2281 "parser/Parser.cpp"
    break;

  case 87:
#line 363 "parser/parser.y"
                                                                  {
            auto block_items = new BlockItems();
            auto block_item = new BlockItem(std::shared_ptr<Statement>((yyvsp[0].stmt)));
            block_items->addItem(std::shared_ptr<BlockItem>(block_item));
            (yyval.stmt) = new WhileStatement(std::shared_ptr<Expression>((yyvsp[-2].expr)), std::shared_ptr<Statement>(block_items));
      }
#line 2292 "parser/Parser.cpp"
    break;

  case 88:
#line 369 "parser/parser.y"
                        { (yyval.stmt) = new BreakStatement(); }
#line 2298 "parser/Parser.cpp"
    break;

  case 89:
#line 370 "parser/parser.y"
                           { (yyval.stmt) = new ContinueStatement(); }
#line 2304 "parser/Parser.cpp"
    break;

  case 90:
#line 371 "parser/parser.y"
                         { (yyval.stmt) = new ReturnStatement(); }
#line 2310 "parser/Parser.cpp"
    break;

  case 91:
#line 372 "parser/parser.y"
                                { (yyval.stmt) = new ReturnStatement(std::shared_ptr<Expression>((yyvsp[-1].expr))); }
#line 2316 "parser/Parser.cpp"
    break;


#line 2320 "parser/Parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, yylsp);
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
  return yyresult;
}
#line 375 "parser/parser.y"


void yyerror(const char* s) {
    printf("line:%d\tcolumn:%d\n", yylloc.first_line, yylloc.first_column);
    printf("ERROR: %s\n", s);
}
