#ifndef LSCP_H
#define LSCP_H

#include "lscp-common.h"
#include "lscp-allocator.h"
#include "lscp-tokenizer.h"

// region enums
typedef enum lscp_ast_type
{
    LSCP_AST_UNKNOWN = 0,
    LSCP_AST_TRANSLATION_UNIT,
    LSCP_AST_DECLARATION,
    LSCP_AST_FUNCTION_ARG,
    LSCP_AST_FUNCTION_DECL,
    LSCP_AST_VARIABLE_DECL,
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

typedef enum lscp_ts_type
{
    LSCP_TS_ARRAY = LSCP_TK_OPEN_SQUARE_BRACKET,
    LSCP_TS_POINTER = LSCP_TK_STAR,
    LSCP_TS_CONST = LSCP_TK_CONST_KEYWORD,
    LSCP_TS_RESTRICT = LSCP_TK_RESTRICT_KEYWORD,
    LSCP_TS_UNSIGNED = LSCP_TK_UNSIGNED_KEYWORD,
    LSCP_TS_SIGNED = LSCP_TK_SIGNED_KEYWORD,
    LSCP_TS_INT = LSCP_TK_INT_KEYWORD,
    LSCP_TS_SHORT = LSCP_TK_SHORT_KEYWORD,
    LSCP_TS_LONG = LSCP_TK_LONG_KEYWORD,
    LSCP_TS_CHAR = LSCP_TK_CHAR_KEYWORD,
    LSCP_TS_FLOAT = LSCP_TK_FLOAT_KEYWORD,
    LSCP_TS_DOUBLE = LSCP_TK_DOUBLE_KEYWORD,
    LSCP_TS_VOID = LSCP_TK_VOID_KEYWORD,
    LSCP_TS_FUNCTION = LSCP_TK_OPEN_PAREN,
    LSCP_TS_STATIC = LSCP_TK_STATIC_KEYWORD,
    LSCP_TS_EXTERN = LSCP_TK_EXTERN_KEYWORD,
    LSCP_TS_REGISTER = LSCP_TK_REGISTER_KEYWORD,
    LSCP_TS_AUTO = LSCP_TK_AUTO_KEYWORD,
    LSCP_TS_INLINE = LSCP_TK_INLINE_KEYWORD,
    LSCP_TS_VARGS = LSCP_TK_DOT,
} lscp_ts_type;

typedef struct lscp_type_specifier lscp_type_specifier;
struct lscp_type_specifier
{
    lscp_ts_type type;
    struct
    {
        struct lscp_ast_node* args;
    } func;
};

typedef struct lscp_ast_node lscp_ast_node;
struct lscp_ast_node
{
    union { lscp_ast_type type, valid; };

    lscp_int tk_begin;
    lscp_int tk_count;

    union
    {
        struct
        {
            lscp_type_specifier* types;
            lscp_tk_index name;
        } decl;

        struct
        {
            lscp_ast_node* decls;
        } var_list;
    };
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

#endif // LSCP_H