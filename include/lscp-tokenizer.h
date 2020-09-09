#ifndef LSCP_TOKENIZER_H
#define LSCP_TOKENIZER_H

#include "lscp-common.h"

typedef lscp_int lscp_tk_index;
typedef lscp_int lscp_ast_index;

#define LSCP_C_KEYWORDS_COUNT (44)
#define LSCP_C_KEYWORDS       { "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return","short", "signed", "sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while","_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic","_Imaginary","_Noreturn","_Static_assert","_Thread_local",}

#define LSCP_C_SYMBOLS        "~!%^&*()-+=:<>,.[]{}|;/\\"
#define LSCP_C_SYMBOLS_COUNT  (24)
#define LSCP_C_SYMBOLS_TOKENS { LSCP_TK_TILDE,LSCP_TK_EXCLAMATION_MARK,LSCP_TK_PERCENT,LSCP_TK_HAT,LSCP_TK_AMPERSAND,LSCP_TK_STAR,LSCP_TK_OPEN_PAREN,LSCP_TK_CLOSE_PAREN,LSCP_TK_MINUS,LSCP_TK_PLUS,LSCP_TK_EQUAL,LSCP_TK_COLON,LSCP_TK_LEFT_ARROW,LSCP_TK_RIGHT_ARROW,LSCP_TK_COMMA,LSCP_TK_DOT,LSCP_TK_OPEN_SQUARE_BRACKET,LSCP_TK_CLOSE_SQUARE_BRACKET,LSCP_TK_OPEN_CURLY,LSCP_TK_CLOSE_CURLY,LSCP_TK_BAR,LSCP_TK_SEMICOLON,LSCP_TK_FORWARD_SLASH,LSCP_TK_BACKWARD_SLASH }

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
    LSCP_TK_DOTDOTDOT,             // ...
    LSCP_TK_OPEN_SQUARE_BRACKET,   // [
    LSCP_TK_CLOSE_SQUARE_BRACKET,  // ]
    LSCP_TK_OPEN_CURLY,            // {
    LSCP_TK_CLOSE_CURLY,           // }
    LSCP_TK_BAR,                   // |
    LSCP_TK_SEMICOLON,             // ;
    LSCP_TK_FORWARD_SLASH,         // /
    LSCP_TK_BACKWARD_SLASH,        // \

} lscp_token_type;

typedef struct lscp_token
{
    lscp_token_type type;
    lscp_int        begin;
    lscp_int        len;
    lscp_int        line;
    lscp_int        number_in_line; // The number of the token in the line its on.
} lscp_token;

typedef struct lscp_tokenizer
{
    const char* code;
    lscp_int    len;

    lscp_int    curr;                 // Current position in the source code
    lscp_int    curr_line;            // Current line the source code
    lscp_int    tokens_on_line_count; // Used to count the number of tokens on a line
    lscp_error  error;
} lscp_tokenizer;

typedef struct lscp_tokenizer_result
{
    lscp_token* tokens;      // Tokens array
    lscp_int    count;       // Number of tokens
    lscp_int    buffer_size; // Size in bytes of the result array
    lscp_error  error;
} lscp_tokenizer_result;

lscp_api lscp_tokenizer        lscp_tokenizer_create(const char* src, lscp_int src_len);
lscp_api lscp_bool             lscp_tokenizer_is_done(lscp_tokenizer ctx);
lscp_api lscp_token            lscp_get_next_token(lscp_tokenizer* ctx);
lscp_api lscp_tokenizer_result lscp_tokenize(const char* src, lscp_int src_len);

#endif // LSCP_TOKENIZER_H