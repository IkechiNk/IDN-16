/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/tools/assembler/parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "idn16/codegen.h"
#include "idn16/symbol_table.h"

extern int yylineno;
extern char current_token[256];
extern char current_line[1024];
extern int token_idx;

int yylex(void);
void yyerror(const char *s);

#line 88 "src/tools/assembler/parser.tab.c"

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

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_REG = 3,                        /* REG  */
  YYSYMBOL_OFFSET = 4,                     /* OFFSET  */
  YYSYMBOL_IMM5 = 5,                       /* IMM5  */
  YYSYMBOL_IMM8 = 6,                       /* IMM8  */
  YYSYMBOL_IMM16 = 7,                      /* IMM16  */
  YYSYMBOL_IDENTIFIER = 8,                 /* IDENTIFIER  */
  YYSYMBOL_ADD = 9,                        /* ADD  */
  YYSYMBOL_SUB = 10,                       /* SUB  */
  YYSYMBOL_AND = 11,                       /* AND  */
  YYSYMBOL_OR = 12,                        /* OR  */
  YYSYMBOL_XOR = 13,                       /* XOR  */
  YYSYMBOL_SHL = 14,                       /* SHL  */
  YYSYMBOL_SHR = 15,                       /* SHR  */
  YYSYMBOL_SRA = 16,                       /* SRA  */
  YYSYMBOL_MOV = 17,                       /* MOV  */
  YYSYMBOL_CMP = 18,                       /* CMP  */
  YYSYMBOL_NOT = 19,                       /* NOT  */
  YYSYMBOL_LDI = 20,                       /* LDI  */
  YYSYMBOL_LDW = 21,                       /* LDW  */
  YYSYMBOL_STW = 22,                       /* STW  */
  YYSYMBOL_ADDI = 23,                      /* ADDI  */
  YYSYMBOL_LUI = 24,                       /* LUI  */
  YYSYMBOL_ANDI = 25,                      /* ANDI  */
  YYSYMBOL_ORI = 26,                       /* ORI  */
  YYSYMBOL_XORI = 27,                      /* XORI  */
  YYSYMBOL_JMP = 28,                       /* JMP  */
  YYSYMBOL_JEQ = 29,                       /* JEQ  */
  YYSYMBOL_JNE = 30,                       /* JNE  */
  YYSYMBOL_JGT = 31,                       /* JGT  */
  YYSYMBOL_JLT = 32,                       /* JLT  */
  YYSYMBOL_JSR = 33,                       /* JSR  */
  YYSYMBOL_RET = 34,                       /* RET  */
  YYSYMBOL_HLT = 35,                       /* HLT  */
  YYSYMBOL_NOP = 36,                       /* NOP  */
  YYSYMBOL_INC = 37,                       /* INC  */
  YYSYMBOL_DEC = 38,                       /* DEC  */
  YYSYMBOL_LDB = 39,                       /* LDB  */
  YYSYMBOL_STB = 40,                       /* STB  */
  YYSYMBOL_LOAD16 = 41,                    /* LOAD16  */
  YYSYMBOL_NEWLINE = 42,                   /* NEWLINE  */
  YYSYMBOL_EMPTY = 43,                     /* EMPTY  */
  YYSYMBOL_44_ = 44,                       /* ':'  */
  YYSYMBOL_45_ = 45,                       /* '='  */
  YYSYMBOL_46_ = 46,                       /* ','  */
  YYSYMBOL_47_ = 47,                       /* '['  */
  YYSYMBOL_48_ = 48,                       /* '+'  */
  YYSYMBOL_49_ = 49,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 50,                  /* $accept  */
  YYSYMBOL_program = 51,                   /* program  */
  YYSYMBOL_lines = 52,                     /* lines  */
  YYSYMBOL_line = 53,                      /* line  */
  YYSYMBOL_label_def = 54,                 /* label_def  */
  YYSYMBOL_assignment = 55,                /* assignment  */
  YYSYMBOL_instruction = 56                /* instruction  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
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
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   231

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  7
/* YYNRULES -- Number of rules.  */
#define YYNRULES  88
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  212

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   298


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,    46,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    44,     2,
       2,    45,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    49,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    54,    54,    57,    59,    63,    64,    65,    66,    70,
      74,    75,    76,    77,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     140,   141,   142,   150,   155,   160,   166,   167,   168,   169,
     170,   171,   172,   173,   175,   176,   177,   178,   179
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "REG", "OFFSET",
  "IMM5", "IMM8", "IMM16", "IDENTIFIER", "ADD", "SUB", "AND", "OR", "XOR",
  "SHL", "SHR", "SRA", "MOV", "CMP", "NOT", "LDI", "LDW", "STW", "ADDI",
  "LUI", "ANDI", "ORI", "XORI", "JMP", "JEQ", "JNE", "JGT", "JLT", "JSR",
  "RET", "HLT", "NOP", "INC", "DEC", "LDB", "STB", "LOAD16", "NEWLINE",
  "EMPTY", "':'", "'='", "','", "'['", "'+'", "']'", "$accept", "program",
  "lines", "line", "label_def", "assignment", "instruction", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-9)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -9,    43,    -8,    -9,    59,    45,    50,    55,    65,    70,
      98,   104,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,    36,    41,    46,    51,    56,    61,
      -9,    -9,    66,   123,   124,   125,   126,   127,    -9,    -9,
      21,    39,    89,    -9,    71,    87,    91,    93,    95,    96,
      97,    99,   100,   101,   102,   103,   105,   106,   107,   108,
     109,   110,   128,   129,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,   130,   131,   132,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,   133,   135,   137,   141,   147,   154,
     155,   156,   157,   158,   159,    74,    85,   134,   160,    78,
     161,   162,   163,   136,   138,    31,   140,   142,   143,   144,
     145,   146,   148,   149,    -9,    -9,    -9,    -9,    -9,    -9,
     164,   165,   150,    -9,    -9,    -9,   151,   152,   153,   166,
     167,    -9,    -9,    -9,    -9,    -9,   168,   169,   170,   176,
     177,   179,   181,   184,    57,    60,    80,    82,    84,    86,
      62,    64,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
      88,    -9,    90,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    92,    -9,    94,    -9,   171,   172,   173,   174,
     175,   178,   180,   182,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      69,    70,    71,     0,     0,     0,     0,     0,     8,     4,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    46,    47,    45,    52,    50,
      51,    49,    56,    54,    55,    53,    60,    58,    59,    57,
      64,    62,    63,    61,    68,    66,    67,    65,    73,    75,
      74,    72,    76,    77,     0,     0,     0,     5,     6,     7,
      11,    13,    12,    10,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,    23,    24,    26,    25,    27,
       0,     0,     0,    36,    37,    38,     0,     0,     0,     0,
       0,    85,    87,    86,    84,    88,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,    15,    16,    17,    18,    19,    20,    21,
       0,    30,     0,    33,    34,    35,    39,    40,    41,    42,
      43,    44,     0,    83,     0,    80,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    31,    32,    81,    82,
      78,    79
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -9,    -9,    -9,    -9,    -9,    -9,    -9
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    39,    40,    41,    42
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,   151,   152,   153,   154,   155,
      64,    65,    66,     3,    67,    68,    69,    70,    45,    71,
      72,    73,    74,    46,    75,    76,    77,    78,    47,    79,
      80,    81,    82,    97,    83,    84,    85,    86,    48,    87,
      88,    89,    90,    49,    91,   100,   101,   102,   103,   137,
     138,    98,   139,   143,   144,   184,   145,   186,   185,   188,
     187,   190,   189,   196,   191,   198,   197,   200,   199,   202,
     201,    50,   203,    43,    44,   180,   181,    51,   182,   183,
     192,   193,   194,   195,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    92,    93,    94,    95,
      96,    99,   140,   104,     0,     0,   126,   105,   127,   106,
     128,   107,   108,   109,   129,   110,   111,   112,   113,   114,
     130,   115,   116,   117,   118,   119,   120,   131,   132,   133,
     134,   135,   136,   142,   146,   147,   148,   164,   165,   170,
     171,   172,   173,   174,   121,   122,   123,   124,   125,   175,
     176,   141,   177,   149,   178,   150,   156,   179,   157,   158,
     159,   160,   161,     0,   162,   163,   166,   167,   168,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     204,   205,   206,   207,   208,     0,     0,   209,     0,   210,
       0,   211
};

static const yytype_int8 yycheck[] =
{
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     4,     5,     6,     7,     8,
       4,     5,     6,     0,     8,     4,     5,     6,     3,     8,
       4,     5,     6,     3,     8,     4,     5,     6,     3,     8,
       4,     5,     6,    42,     8,     4,     5,     6,     3,     8,
       4,     5,     6,     3,     8,     4,     5,     6,     7,     5,
       6,    42,     8,     5,     6,     5,     8,     5,     8,     5,
       8,     5,     8,     5,     8,     5,     8,     5,     8,     5,
       8,     3,     8,    44,    45,    48,    49,     3,    48,    49,
      48,    49,    48,    49,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,    42,    47,    46,    -1,    -1,     3,    46,     3,    46,
       3,    46,    46,    46,     3,    46,    46,    46,    46,    46,
       3,    46,    46,    46,    46,    46,    46,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,    46,    46,    46,    46,    46,     3,
       3,    47,     3,    47,     3,    47,    46,     3,    46,    46,
      46,    46,    46,    -1,    46,    46,    46,    46,    46,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    49,    49,    49,    49,    -1,    -1,    49,    -1,    49,
      -1,    49
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    51,    52,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    53,
      54,    55,    56,    44,    45,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     5,     6,     8,     4,     5,
       6,     8,     4,     5,     6,     8,     4,     5,     6,     8,
       4,     5,     6,     8,     4,     5,     6,     8,     4,     5,
       6,     8,     3,     3,     3,     3,     3,    42,    42,    42,
       4,     5,     6,     7,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     5,     6,     8,
      47,    47,     3,     5,     6,     8,     3,     3,     3,    47,
      47,     4,     5,     6,     7,     8,    46,    46,    46,    46,
      46,    46,    46,    46,     3,     3,    46,    46,    46,    46,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
      48,    49,    48,    49,     5,     8,     5,     8,     5,     8,
       5,     8,    48,    49,    48,    49,     5,     8,     5,     8,
       5,     8,     5,     8,    49,    49,    49,    49,    49,    49,
      49,    49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    50,    51,    52,    52,    53,    53,    53,    53,    54,
      55,    55,    55,    55,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     1,     2,
       3,     3,     3,     3,     6,     6,     6,     6,     6,     6,
       6,     6,     4,     4,     4,     4,     4,     4,     8,     8,
       6,     8,     8,     6,     6,     6,     4,     4,     4,     6,
       6,     6,     6,     6,     6,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     8,     8,
       6,     8,     8,     6,     4,     4,     4,     4,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
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
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 9: /* label_def: IDENTIFIER ':'  */
#line 70 "src/tools/assembler/parser.y"
                                        { insert_symbol((yyvsp[-1].label).name, (yyvsp[-1].label).addr, 0); free((yyvsp[-1].label).name); }
#line 1259 "src/tools/assembler/parser.tab.c"
    break;

  case 10: /* assignment: IDENTIFIER '=' IMM16  */
#line 74 "src/tools/assembler/parser.y"
                                        { insert_symbol((yyvsp[-2].label).name, ((uint16_t)(yyvsp[0].imm16).upper << 8) | (yyvsp[0].imm16).lower, 1); free((yyvsp[-2].label).name); }
#line 1265 "src/tools/assembler/parser.tab.c"
    break;

  case 11: /* assignment: IDENTIFIER '=' OFFSET  */
#line 75 "src/tools/assembler/parser.y"
                                        { insert_symbol((yyvsp[-2].label).name, (yyvsp[0].num), 1); free((yyvsp[-2].label).name); }
#line 1271 "src/tools/assembler/parser.tab.c"
    break;

  case 12: /* assignment: IDENTIFIER '=' IMM8  */
#line 76 "src/tools/assembler/parser.y"
                                        { insert_symbol((yyvsp[-2].label).name, (yyvsp[0].num), 1); free((yyvsp[-2].label).name); }
#line 1277 "src/tools/assembler/parser.tab.c"
    break;

  case 13: /* assignment: IDENTIFIER '=' IMM5  */
#line 77 "src/tools/assembler/parser.y"
                                        { insert_symbol((yyvsp[-2].label).name, (yyvsp[0].num), 1); free((yyvsp[-2].label).name); }
#line 1283 "src/tools/assembler/parser.tab.c"
    break;

  case 14: /* instruction: ADD REG ',' REG ',' REG  */
#line 81 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00000, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1289 "src/tools/assembler/parser.tab.c"
    break;

  case 15: /* instruction: SUB REG ',' REG ',' REG  */
#line 82 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00001, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1295 "src/tools/assembler/parser.tab.c"
    break;

  case 16: /* instruction: AND REG ',' REG ',' REG  */
#line 83 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00010, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1301 "src/tools/assembler/parser.tab.c"
    break;

  case 17: /* instruction: OR REG ',' REG ',' REG  */
#line 84 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00011, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1307 "src/tools/assembler/parser.tab.c"
    break;

  case 18: /* instruction: XOR REG ',' REG ',' REG  */
#line 85 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00100, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1313 "src/tools/assembler/parser.tab.c"
    break;

  case 19: /* instruction: SHL REG ',' REG ',' REG  */
#line 86 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00101, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1319 "src/tools/assembler/parser.tab.c"
    break;

  case 20: /* instruction: SHR REG ',' REG ',' REG  */
#line 87 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00110, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 0); }
#line 1325 "src/tools/assembler/parser.tab.c"
    break;

  case 21: /* instruction: SRA REG ',' REG ',' REG  */
#line 88 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-5].pc), 0b00110, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].reg), 1); }
#line 1331 "src/tools/assembler/parser.tab.c"
    break;

  case 22: /* instruction: MOV REG ',' REG  */
#line 89 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-3].pc), 0b00111, (yyvsp[-2].reg), (yyvsp[0].reg), 0, 0); }
#line 1337 "src/tools/assembler/parser.tab.c"
    break;

  case 23: /* instruction: CMP REG ',' REG  */
#line 90 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-3].pc), 0b00111, (yyvsp[-2].reg), (yyvsp[0].reg), 0, 1); }
#line 1343 "src/tools/assembler/parser.tab.c"
    break;

  case 24: /* instruction: NOT REG ',' REG  */
#line 91 "src/tools/assembler/parser.y"
                                                    { emit_reg_format((yyvsp[-3].pc), 0b00111, (yyvsp[-2].reg), (yyvsp[0].reg), 0, 2); }
#line 1349 "src/tools/assembler/parser.tab.c"
    break;

  case 25: /* instruction: LDI REG ',' IMM8  */
#line 93 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); }
#line 1355 "src/tools/assembler/parser.tab.c"
    break;

  case 26: /* instruction: LDI REG ',' IMM5  */
#line 94 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); }
#line 1361 "src/tools/assembler/parser.tab.c"
    break;

  case 27: /* instruction: LDI REG ',' IDENTIFIER  */
#line 95 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].label).name, yylineno); }
#line 1367 "src/tools/assembler/parser.tab.c"
    break;

  case 28: /* instruction: LDW REG ',' '[' REG '+' IMM5 ']'  */
#line 96 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-7].pc), 0b01001, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].num), yylineno); }
#line 1373 "src/tools/assembler/parser.tab.c"
    break;

  case 29: /* instruction: LDW REG ',' '[' REG '+' IDENTIFIER ']'  */
#line 97 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-7].pc), 0b01001, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].label).name, yylineno); }
#line 1379 "src/tools/assembler/parser.tab.c"
    break;

  case 30: /* instruction: LDW REG ',' '[' REG ']'  */
#line 98 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01001, (yyvsp[-4].reg), (yyvsp[-1].reg), 0, yylineno); }
#line 1385 "src/tools/assembler/parser.tab.c"
    break;

  case 31: /* instruction: STW REG ',' '[' REG '+' IMM5 ']'  */
#line 99 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-7].pc), 0b01010, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].num), yylineno); }
#line 1391 "src/tools/assembler/parser.tab.c"
    break;

  case 32: /* instruction: STW REG ',' '[' REG '+' IDENTIFIER ']'  */
#line 100 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-7].pc), 0b01010, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].label).name, yylineno); }
#line 1397 "src/tools/assembler/parser.tab.c"
    break;

  case 33: /* instruction: STW REG ',' '[' REG ']'  */
#line 101 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01010, (yyvsp[-4].reg), (yyvsp[-1].reg), 0, yylineno); }
#line 1403 "src/tools/assembler/parser.tab.c"
    break;

  case 34: /* instruction: ADDI REG ',' REG ',' IMM5  */
#line 102 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01011, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].num), yylineno); }
#line 1409 "src/tools/assembler/parser.tab.c"
    break;

  case 35: /* instruction: ADDI REG ',' REG ',' IDENTIFIER  */
#line 103 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-5].pc), 0b01011, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].label).name, yylineno); }
#line 1415 "src/tools/assembler/parser.tab.c"
    break;

  case 36: /* instruction: LUI REG ',' IMM5  */
#line 104 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-3].pc), 0b01100, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); }
#line 1421 "src/tools/assembler/parser.tab.c"
    break;

  case 37: /* instruction: LUI REG ',' IMM8  */
#line 105 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-3].pc), 0b01100, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); }
#line 1427 "src/tools/assembler/parser.tab.c"
    break;

  case 38: /* instruction: LUI REG ',' IDENTIFIER  */
#line 106 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-3].pc), 0b01100, (yyvsp[-2].reg), 0, (yyvsp[0].label).name, yylineno); }
#line 1433 "src/tools/assembler/parser.tab.c"
    break;

  case 39: /* instruction: ANDI REG ',' REG ',' IMM5  */
#line 107 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01101, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].num), yylineno); }
#line 1439 "src/tools/assembler/parser.tab.c"
    break;

  case 40: /* instruction: ANDI REG ',' REG ',' IDENTIFIER  */
#line 108 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-5].pc), 0b01101, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].label).name, yylineno); }
#line 1445 "src/tools/assembler/parser.tab.c"
    break;

  case 41: /* instruction: ORI REG ',' REG ',' IMM5  */
#line 109 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01110, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].num), yylineno); }
#line 1451 "src/tools/assembler/parser.tab.c"
    break;

  case 42: /* instruction: ORI REG ',' REG ',' IDENTIFIER  */
#line 110 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-5].pc), 0b01110, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].label).name, yylineno); }
#line 1457 "src/tools/assembler/parser.tab.c"
    break;

  case 43: /* instruction: XORI REG ',' REG ',' IMM5  */
#line 111 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_with_line((yyvsp[-5].pc), 0b01111, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].num), yylineno); }
#line 1463 "src/tools/assembler/parser.tab.c"
    break;

  case 44: /* instruction: XORI REG ',' REG ',' IDENTIFIER  */
#line 112 "src/tools/assembler/parser.y"
                                                    { emit_imm_format_identifier((yyvsp[-5].pc), 0b01111, (yyvsp[-4].reg), (yyvsp[-2].reg), (yyvsp[0].label).name, yylineno); }
#line 1469 "src/tools/assembler/parser.tab.c"
    break;

  case 45: /* instruction: JMP IDENTIFIER  */
#line 114 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10000, (yyvsp[0].label).name); }
#line 1475 "src/tools/assembler/parser.tab.c"
    break;

  case 46: /* instruction: JMP IMM5  */
#line 115 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10000, (yyvsp[0].num)); }
#line 1481 "src/tools/assembler/parser.tab.c"
    break;

  case 47: /* instruction: JMP IMM8  */
#line 116 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10000, (yyvsp[0].num)); }
#line 1487 "src/tools/assembler/parser.tab.c"
    break;

  case 48: /* instruction: JMP OFFSET  */
#line 117 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10000, (yyvsp[0].num)); }
#line 1493 "src/tools/assembler/parser.tab.c"
    break;

  case 49: /* instruction: JEQ IDENTIFIER  */
#line 118 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10001, (yyvsp[0].label).name); }
#line 1499 "src/tools/assembler/parser.tab.c"
    break;

  case 50: /* instruction: JEQ IMM5  */
#line 119 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10001, (yyvsp[0].num)); }
#line 1505 "src/tools/assembler/parser.tab.c"
    break;

  case 51: /* instruction: JEQ IMM8  */
#line 120 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10001, (yyvsp[0].num)); }
#line 1511 "src/tools/assembler/parser.tab.c"
    break;

  case 52: /* instruction: JEQ OFFSET  */
#line 121 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10001, (yyvsp[0].num)); }
#line 1517 "src/tools/assembler/parser.tab.c"
    break;

  case 53: /* instruction: JNE IDENTIFIER  */
#line 122 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10010, (yyvsp[0].label).name); }
#line 1523 "src/tools/assembler/parser.tab.c"
    break;

  case 54: /* instruction: JNE IMM5  */
#line 123 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10010, (yyvsp[0].num)); }
#line 1529 "src/tools/assembler/parser.tab.c"
    break;

  case 55: /* instruction: JNE IMM8  */
#line 124 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10010, (yyvsp[0].num)); }
#line 1535 "src/tools/assembler/parser.tab.c"
    break;

  case 56: /* instruction: JNE OFFSET  */
#line 125 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10010, (yyvsp[0].num)); }
#line 1541 "src/tools/assembler/parser.tab.c"
    break;

  case 57: /* instruction: JGT IDENTIFIER  */
#line 126 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10011, (yyvsp[0].label).name); }
#line 1547 "src/tools/assembler/parser.tab.c"
    break;

  case 58: /* instruction: JGT IMM5  */
#line 127 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10011, (yyvsp[0].num)); }
#line 1553 "src/tools/assembler/parser.tab.c"
    break;

  case 59: /* instruction: JGT IMM8  */
#line 128 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10011, (yyvsp[0].num)); }
#line 1559 "src/tools/assembler/parser.tab.c"
    break;

  case 60: /* instruction: JGT OFFSET  */
#line 129 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10011, (yyvsp[0].num)); }
#line 1565 "src/tools/assembler/parser.tab.c"
    break;

  case 61: /* instruction: JLT IDENTIFIER  */
#line 130 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10100, (yyvsp[0].label).name); }
#line 1571 "src/tools/assembler/parser.tab.c"
    break;

  case 62: /* instruction: JLT IMM5  */
#line 131 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10100, (yyvsp[0].num)); }
#line 1577 "src/tools/assembler/parser.tab.c"
    break;

  case 63: /* instruction: JLT IMM8  */
#line 132 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10100, (yyvsp[0].num)); }
#line 1583 "src/tools/assembler/parser.tab.c"
    break;

  case 64: /* instruction: JLT OFFSET  */
#line 133 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10100, (yyvsp[0].num)); }
#line 1589 "src/tools/assembler/parser.tab.c"
    break;

  case 65: /* instruction: JSR IDENTIFIER  */
#line 134 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_identifier((yyvsp[-1].pc), 0b10101, (yyvsp[0].label).name); }
#line 1595 "src/tools/assembler/parser.tab.c"
    break;

  case 66: /* instruction: JSR IMM5  */
#line 135 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10101, (yyvsp[0].num)); }
#line 1601 "src/tools/assembler/parser.tab.c"
    break;

  case 67: /* instruction: JSR IMM8  */
#line 136 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10101, (yyvsp[0].num)); }
#line 1607 "src/tools/assembler/parser.tab.c"
    break;

  case 68: /* instruction: JSR OFFSET  */
#line 137 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[-1].pc), 0b10101, (yyvsp[0].num)); }
#line 1613 "src/tools/assembler/parser.tab.c"
    break;

  case 69: /* instruction: RET  */
#line 138 "src/tools/assembler/parser.y"
                                                    { emit_jb_format_imm((yyvsp[0].pc), 0b10110, 0); }
#line 1619 "src/tools/assembler/parser.tab.c"
    break;

  case 70: /* instruction: HLT  */
#line 140 "src/tools/assembler/parser.y"
                                                    { emit_special_format((yyvsp[0].pc), 0b11000, 0, 0, 0); }
#line 1625 "src/tools/assembler/parser.tab.c"
    break;

  case 71: /* instruction: NOP  */
#line 141 "src/tools/assembler/parser.y"
                                                    { emit_special_format((yyvsp[0].pc), 0b11001, 0, 0, 0); }
#line 1631 "src/tools/assembler/parser.tab.c"
    break;

  case 72: /* instruction: NOP IDENTIFIER  */
#line 142 "src/tools/assembler/parser.y"
                                                    { 
                                                              sort_symbols();
                                                              uint16_t value = get_symbol((yyvsp[0].label).name, NULL);
                                                              for (int i = 0; i < value; i++) {
                                                                emit_special_format((yyvsp[-1].pc) + (i * 2), 0b11001, 0, 0, 0);
                                                              }
                                                              free((yyvsp[0].label).name);
                                                            }
#line 1644 "src/tools/assembler/parser.tab.c"
    break;

  case 73: /* instruction: NOP OFFSET  */
#line 150 "src/tools/assembler/parser.y"
                                                    { 
                                                              for (int i = 0; i < (yyvsp[0].num); i++) {
                                                                emit_special_format((yyvsp[-1].pc) + (i * 2), 0b11001, 0, 0, 0);
                                                              }
                                                            }
#line 1654 "src/tools/assembler/parser.tab.c"
    break;

  case 74: /* instruction: NOP IMM8  */
#line 155 "src/tools/assembler/parser.y"
                                                  { 
                                                            for (int i = 0; i < (yyvsp[0].num); i++) {
                                                              emit_special_format((yyvsp[-1].pc) + (i * 2), 0b11001, 0, 0, 0);
                                                            }
                                                          }
#line 1664 "src/tools/assembler/parser.tab.c"
    break;

  case 75: /* instruction: NOP IMM5  */
#line 160 "src/tools/assembler/parser.y"
                                                  { 
                                                            for (int i = 0; i < (yyvsp[0].num); i++) {
                                                              emit_special_format((yyvsp[-1].pc) + (i * 2), 0b11001, 0, 0, 0);
                                                            }
                                                          }
#line 1674 "src/tools/assembler/parser.tab.c"
    break;

  case 76: /* instruction: INC REG  */
#line 166 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-1].pc), 0b11010, (yyvsp[0].reg), 0, 0); }
#line 1680 "src/tools/assembler/parser.tab.c"
    break;

  case 77: /* instruction: DEC REG  */
#line 167 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-1].pc), 0b11011, (yyvsp[0].reg), 0, 0); }
#line 1686 "src/tools/assembler/parser.tab.c"
    break;

  case 78: /* instruction: STB REG ',' '[' REG '+' IMM5 ']'  */
#line 168 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-7].pc), 0b11101, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].num)); }
#line 1692 "src/tools/assembler/parser.tab.c"
    break;

  case 79: /* instruction: STB REG ',' '[' REG '+' IDENTIFIER ']'  */
#line 169 "src/tools/assembler/parser.y"
                                                  { emit_special_format_identifier((yyvsp[-7].pc), 0b11101, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].label).name, yylineno); }
#line 1698 "src/tools/assembler/parser.tab.c"
    break;

  case 80: /* instruction: STB REG ',' '[' REG ']'  */
#line 170 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-5].pc), 0b11101, (yyvsp[-4].reg), (yyvsp[-1].reg), 0); }
#line 1704 "src/tools/assembler/parser.tab.c"
    break;

  case 81: /* instruction: LDB REG ',' '[' REG '+' IMM5 ']'  */
#line 171 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-7].pc), 0b11100, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].num)); }
#line 1710 "src/tools/assembler/parser.tab.c"
    break;

  case 82: /* instruction: LDB REG ',' '[' REG '+' IDENTIFIER ']'  */
#line 172 "src/tools/assembler/parser.y"
                                                  { emit_special_format_identifier((yyvsp[-7].pc), 0b11100, (yyvsp[-6].reg), (yyvsp[-3].reg), (yyvsp[-1].label).name, yylineno); }
#line 1716 "src/tools/assembler/parser.tab.c"
    break;

  case 83: /* instruction: LDB REG ',' '[' REG ']'  */
#line 173 "src/tools/assembler/parser.y"
                                                  { emit_special_format((yyvsp[-5].pc), 0b11100, (yyvsp[-4].reg), (yyvsp[-1].reg), 0); }
#line 1722 "src/tools/assembler/parser.tab.c"
    break;

  case 84: /* instruction: LOAD16 REG ',' IMM16  */
#line 175 "src/tools/assembler/parser.y"
                                                  { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].imm16).lower, yylineno); emit_imm_format_with_line((yyvsp[-3].pc) + 2, 0b01100, (yyvsp[-2].reg), 0, (yyvsp[0].imm16).upper, yylineno);}
#line 1728 "src/tools/assembler/parser.tab.c"
    break;

  case 85: /* instruction: LOAD16 REG ',' OFFSET  */
#line 176 "src/tools/assembler/parser.y"
                                                  { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].num) & 0xFF, yylineno); emit_imm_format_with_line((yyvsp[-3].pc) + 2, 0b01100, (yyvsp[-2].reg), 0, (yyvsp[0].num) >> 8, yylineno);}
#line 1734 "src/tools/assembler/parser.tab.c"
    break;

  case 86: /* instruction: LOAD16 REG ',' IMM8  */
#line 177 "src/tools/assembler/parser.y"
                                                  { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); emit_imm_format_with_line((yyvsp[-3].pc) + 2, 0b01100, (yyvsp[-2].reg), 0, 0, yylineno);}
#line 1740 "src/tools/assembler/parser.tab.c"
    break;

  case 87: /* instruction: LOAD16 REG ',' IMM5  */
#line 178 "src/tools/assembler/parser.y"
                                                  { emit_imm_format_with_line((yyvsp[-3].pc), 0b01000, (yyvsp[-2].reg), 0, (yyvsp[0].num), yylineno); emit_imm_format_with_line((yyvsp[-3].pc) + 2, 0b01100, (yyvsp[-2].reg), 0, 0, yylineno);}
#line 1746 "src/tools/assembler/parser.tab.c"
    break;

  case 88: /* instruction: LOAD16 REG ',' IDENTIFIER  */
#line 179 "src/tools/assembler/parser.y"
                                                  { emit_load16_identifier((yyvsp[-3].pc), (yyvsp[-2].reg), (yyvsp[0].label).name, yylineno); }
#line 1752 "src/tools/assembler/parser.tab.c"
    break;


#line 1756 "src/tools/assembler/parser.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

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
                      yytoken, &yylval);
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 182 "src/tools/assembler/parser.y"


void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "Invalid token: %s\n", current_token);
}
