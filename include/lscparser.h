#ifndef LS_CPARSER_H
#define LS_CPARSER_H

// region typedefs and macros
typedef signed long long lscp_int;
typedef int lscp_bool;

#ifdef __cplusplus
    #ifndef LSCP_API
        #define LSCP_API extern "C"
    #endif
#else
    #ifndef LSCP_API
        #define LSCP_API extern
    #endif
#endif

#ifndef LSCP_INTERNAL
    #define LSCP_INTERNAL static
#endif

#define LSCP_C_KEYWORDS_COUNT (44)
#define LSCP_C_KEYWORDS { "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return","short", "signed", "sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while","_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic","_Imaginary","_Noreturn","_Static_assert","_Thread_local",}

#define LSCP_C_SYMBOLS "~!%^&*()-+=:<>,.[]{}|;/\\"
#define LSCP_C_SYMBOLS_COUNT (24)
#define LSCP_C_SYMBOLS_TOKENS { LSCP_TK_TILDE,LSCP_TK_EXCLAMATION_MARK,LSCP_TK_PERCENT,LSCP_TK_HAT,LSCP_TK_AMPERSAND,LSCP_TK_STAR,LSCP_TK_OPEN_PAREN,LSCP_TK_CLOSE_PAREN,LSCP_TK_MINUS,LSCP_TK_PLUS,LSCP_TK_EQUAL,LSCP_TK_COLON,LSCP_TK_LEFT_ARROW,LSCP_TK_RIGHT_ARROW,LSCP_TK_COMMA,LSCP_TK_DOT,LSCP_TK_OPEN_SQUARE_BRACKET,LSCP_TK_CLOSE_SQUARE_BRACKET,LSCP_TK_OPEN_CURLY,LSCP_TK_CLOSE_CURLY,LSCP_TK_BAR,LSCP_TK_SEMICOLON,LSCP_TK_FORWARD_SLASH,LSCP_TK_BACKWARD_SLASH }
// endregion

// region enums
typedef enum lscp_token_type
{
    LSCP_TK_UNKNOWN = 0,

    //region keywords

    LSCP_TK_AUTO_KEYWORD,
    LSCP_TK_BREAK_KEYWORD,
    LSCP_TK_CASE_KEYWORD,
    LSCP_TK_CHAR_KEYWORD,
    LSCP_TK_CONST_KEYWORD,
    LSCP_TK_CONTINUE_KEYWORD,
    LSCP_TK_DEFAULT_KEYWORD,
    LSCP_TK_DO_KEYWORD,
    LSCP_TK_DOUBLE_KEYWORD,
    LSCP_TK_ELSE_KEYWORD,
    LSCP_TK_ENUM_KEYWORD,
    LSCP_TK_EXTERN_KEYWORD,
    LSCP_TK_FLOAT_KEYWORD,
    LSCP_TK_FOR_KEYWORD,
    LSCP_TK_GOTO_KEYWORD,
    LSCP_TK_IF_KEYWORD,
    LSCP_TK_INLINE_KEYWORD,
    LSCP_TK_INT_KEYWORD,
    LSCP_TK_LONG_KEYWORD,
    LSCP_TK_REGISTER_KEYWORD,
    LSCP_TK_RESTRICT_KEYWORD,
    LSCP_TK_RETURN_KEYWORD,
    LSCP_TK_SHORT_KEYWORD,
    LSCP_TK_SIGNED_KEYWORD,
    LSCP_TK_SIZEOF_KEYWORD,
    LSCP_TK_STATIC_KEYWORD,
    LSCP_TK_STRUCT_KEYWORD,
    LSCP_TK_SWITCH_KEYWORD,
    LSCP_TK_TYPEDEF_KEYWORD,
    LSCP_TK_UNION_KEYWORD,
    LSCP_TK_UNSIGNED_KEYWORD,
    LSCP_TK_VOID_KEYWORD,
    LSCP_TK_VOLATILE_KEYWORD,
    LSCP_TK_WHILE_KEYWORD,
    LSCP_TK_ALIGNAS_KEYWORD,
    LSCP_TK_ALIGNOF_KEYWORD,
    LSCP_TK_ATOMIC_KEYWORD,
    LSCP_TK_BOOL_KEYWORD,
    LSCP_TK_COMPLEX_KEYWORD,
    LSCP_TK_GENERIC_KEYWORD,
    LSCP_TK_IMAGINARY_KEYWORD,
    LSCP_TK_NORETURN_KEYWORD,
    LSCP_TK_STATIC_ASSERT_KEYWORD,
    LSCP_TK_THREAD_LOCAL_KEYWORD,

    //endregion

    LSCP_TK_IDENTIFIER,
    LSCP_TK_STRING_LITERAL,
    LSCP_TK_NUMERIC_LITERAL,

    LSCP_TK_TILDE,                 // ~
    LSCP_TK_EXCLAMATION_MARK,      // !
    LSCP_TK_PERCENT,               // %
    LSCP_TK_HAT,                   // ^
    LSCP_TK_AMPERSAND,             // &
    LSCP_TK_STAR,                  // *
    LSCP_TK_OPEN_PAREN,            // (
    LSCP_TK_CLOSE_PAREN,           // )
    LSCP_TK_MINUS,                 // -
    LSCP_TK_PLUS,                  // +
    LSCP_TK_EQUAL,                 // =
    LSCP_TK_COLON,                 // :
    LSCP_TK_LEFT_ARROW,            // <
    LSCP_TK_RIGHT_ARROW,           // >
    LSCP_TK_COMMA,                 // ,
    LSCP_TK_DOT,                   // .
    LSCP_TK_OPEN_SQUARE_BRACKET,   // [
    LSCP_TK_CLOSE_SQUARE_BRACKET,  // ]
    LSCP_TK_OPEN_CURLY,            // {
    LSCP_TK_CLOSE_CURLY,           // }
    LSCP_TK_BAR,                   // |
    LSCP_TK_SEMICOLON,             // ;
    LSCP_TK_FORWARD_SLASH,         // /
    LSCP_TK_BACKWARD_SLASH,        // \

} lscp_token_type;

typedef enum lscp_ast_type
{
    LSCP_AST_UNKNOWN = 0,
    LSCP_AST_TRANSLATION_UNIT,
    LSCP_AST_VARIABLE_DECL
} lscp_ast_type;

typedef enum lscp_error
{
    LSCP_NO_ERROR = 0,
} lscp_error;
// endregion

// region structs

typedef struct lscp_str lscp_str;
struct lscp_str
{
    const char* chars;
    lscp_int len;
};

typedef struct lscp_token lscp_token;
struct lscp_token
{
    lscp_token_type type;
    lscp_int        begin;
    lscp_int        len;
    lscp_int        line;
    lscp_int        number_in_line; // The number of the token in the line its on.
};

typedef struct lscp_tokenizer lscp_tokenizer;
struct lscp_tokenizer
{
    const char* code;
    lscp_int    len;

    lscp_int    curr;                 // Current position in the source code
    lscp_int    curr_line;            // Current line the source code
    lscp_int    tokens_on_line_count; // Used to count the number of tokens on a line

    lscp_error  error;
};

typedef struct lscp_tokenizer_result lscp_tokenizer_result;
struct lscp_tokenizer_result
{
    lscp_token* tokens;      // Tokens array
    lscp_int    count;       // Number of tokens
    lscp_int    buffer_size; // Size in bytes of the result array
    lscp_error  error;
};

typedef struct lscp_kv lscp_kv;
struct lscp_kv
{
    char* key;
    int value;
};

typedef struct lscp_scope lscp_scope;
struct lscp_scope
{
    lscp_int* decls;
};

typedef struct lscp_ast_node lscp_ast_node;
struct lscp_ast_node
{
    lscp_ast_type type;

    lscp_int code_begin;
    lscp_int code_len;

    lscp_int tks_begin;
    lscp_int tks_count;


};

typedef struct lscp_parse_result lscp_parse_result;
struct lscp_parse_result
{
    lscp_token*   tokens;
    lscp_int      tokens_count;
    lscp_ast_node toplevel;
    lscp_bool     valid;
};

typedef struct lscp_parser lscp_parser;
struct lscp_parser
{
    const char*       code;
    lscp_int          code_len;

    const lscp_token* tokens;
    lscp_int          tokens_count;

    lscp_ast_node*    ast_nodes;

    lscp_int          curr;
    lscp_bool         valid;
};
// endregion

LSCP_API lscp_tokenizer lscp_tokenizer_create(const char* src, lscp_int src_len);
LSCP_API lscp_bool      lscp_tokenizer_is_done(lscp_tokenizer ctx);
LSCP_API lscp_token     lscp_get_next_token(lscp_tokenizer* ctx);
LSCP_API lscp_tokenizer_result lscp_tokenize(const char* src, lscp_int src_len);

LSCP_API lscp_bool lscp_is_primitive_type(lscp_token_type type);
LSCP_API lscp_parse_result lscp_parse(const char* code, lscp_int len);
LSCP_API lscp_parse_result lscp_parse_cstr(const char* code);

#endif // LS_CPARSER_H