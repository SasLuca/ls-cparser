#include "lscp.h"
#include "stdint.h"
#include "stdio.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "string.h"

#define LSCP_BAD_INDEX (-1)
#define STR_LEN(str_lit) (sizeof(str_lit) - 1)
#define STR(str_lit) ((lscp_str) { str_lit, STR_LEN(str_lit) })

// region internal
LSCP_INTERNAL lscp_bool is_digit(char it)
{
    return it >= '0' && it <= '9';
}

LSCP_INTERNAL lscp_bool is_whitespace(char it)
{
    return it == ' ' || it == '\r' || it == '\n' || it == '\t';
}

LSCP_INTERNAL lscp_bool is_symbol(char it)
{
    const char* symbols = LSCP_C_SYMBOLS;
    for (int i = 0; i < LSCP_C_SYMBOLS_COUNT; i++) if (it == symbols[i]) return 1;
    return 0;
}

LSCP_INTERNAL lscp_bool is_letter(char it)
{
    return (it >= 'a' && it <= 'z') || (it >= 'A' && it <= 'Z');
}

LSCP_INTERNAL lscp_int zstring_len(const char* zstr)
{
    const char* iter = zstr;
    while (*iter++) {}
    return (lscp_int) (iter - zstr);
}

LSCP_INTERNAL lscp_bool string_match(lscp_str a, lscp_str b)
{
    if (a.len != b.len) return 0;

    const char* a_end  = a.chars + a.len;
    const char* b_end  = b.chars + b.len;

    const char* a_iter = a.chars;
    const char* b_iter = b.chars;

    while (a_iter != a_end && b_iter != b_end)
    {
        if (*a_iter != *b_iter) return 0;
        a_iter++;
        b_iter++;
    }

    return 1;
}

LSCP_INTERNAL lscp_bool starts_with(lscp_str str, lscp_str match)
{
    if (str.len <= match.len) return 0;
    for (int i = 0; i < match.len; i++) if (str.chars[i] != match.chars[i]) return 0;
    return 1;
}

LSCP_INTERNAL lscp_bool starts_with_zstr(lscp_str str, const char* match) { return starts_with(str, STR(match)); }

LSCP_INTERNAL lscp_bool ends_with(lscp_str str, lscp_str match)
{
    if (str.len <= match.len) return 0;
    for (int i = 0; i < match.len; i++) if (str.chars[str.len - 1 - i] != match.chars[i]) return 0;
    return 1;
}

LSCP_INTERNAL lscp_bool ends_with_zstr(lscp_str str, const char* match) { return ends_with(str, STR(match)); }

LSCP_INTERNAL lscp_token_type get_token_type_if_keyword(lscp_str str)
{
    if (string_match(str, STR("auto")))           return LSCP_TK_AUTO_KEYWORD;
    if (string_match(str, STR("break")))          return LSCP_TK_BREAK_KEYWORD;
    if (string_match(str, STR("case")))           return LSCP_TK_CASE_KEYWORD;
    if (string_match(str, STR("char")))           return LSCP_TK_CHAR_KEYWORD;
    if (string_match(str, STR("const")))          return LSCP_TK_CONST_KEYWORD;
    if (string_match(str, STR("continue")))       return LSCP_TK_CONTINUE_KEYWORD;
    if (string_match(str, STR("default")))        return LSCP_TK_DEFAULT_KEYWORD;
    if (string_match(str, STR("do")))             return LSCP_TK_DO_KEYWORD;
    if (string_match(str, STR("double")))         return LSCP_TK_DOUBLE_KEYWORD;
    if (string_match(str, STR("else")))           return LSCP_TK_ELSE_KEYWORD;
    if (string_match(str, STR("enum")))           return LSCP_TK_ENUM_KEYWORD;
    if (string_match(str, STR("extern")))         return LSCP_TK_EXTERN_KEYWORD;
    if (string_match(str, STR("float")))          return LSCP_TK_FLOAT_KEYWORD;
    if (string_match(str, STR("for")))            return LSCP_TK_FOR_KEYWORD;
    if (string_match(str, STR("goto")))           return LSCP_TK_GOTO_KEYWORD;
    if (string_match(str, STR("if")))             return LSCP_TK_IF_KEYWORD;
    if (string_match(str, STR("inline")))         return LSCP_TK_INLINE_KEYWORD;
    if (string_match(str, STR("int")))            return LSCP_TK_INT_KEYWORD;
    if (string_match(str, STR("long")))           return LSCP_TK_LONG_KEYWORD;
    if (string_match(str, STR("register")))       return LSCP_TK_REGISTER_KEYWORD;
    if (string_match(str, STR("restrict")))       return LSCP_TK_RESTRICT_KEYWORD;
    if (string_match(str, STR("return")))         return LSCP_TK_RETURN_KEYWORD;
    if (string_match(str, STR("short")))          return LSCP_TK_SHORT_KEYWORD;
    if (string_match(str, STR("signed")))         return LSCP_TK_SIGNED_KEYWORD;
    if (string_match(str, STR("sizeof")))         return LSCP_TK_SIZEOF_KEYWORD;
    if (string_match(str, STR("static")))         return LSCP_TK_STATIC_KEYWORD;
    if (string_match(str, STR("struct")))         return LSCP_TK_STRUCT_KEYWORD;
    if (string_match(str, STR("switch")))         return LSCP_TK_SWITCH_KEYWORD;
    if (string_match(str, STR("typedef")))        return LSCP_TK_TYPEDEF_KEYWORD;
    if (string_match(str, STR("union")))          return LSCP_TK_UNION_KEYWORD;
    if (string_match(str, STR("unsigned")))       return LSCP_TK_UNSIGNED_KEYWORD;
    if (string_match(str, STR("void")))           return LSCP_TK_VOID_KEYWORD;
    if (string_match(str, STR("volatile")))       return LSCP_TK_VOLATILE_KEYWORD;
    if (string_match(str, STR("while")))          return LSCP_TK_WHILE_KEYWORD;
    if (string_match(str, STR("_Alignas")))       return LSCP_TK_ALIGNAS_KEYWORD;
    if (string_match(str, STR("_Alignof")))       return LSCP_TK_ALIGNOF_KEYWORD;
    if (string_match(str, STR("_Atomic")))        return LSCP_TK_ATOMIC_KEYWORD;
    if (string_match(str, STR("_Bool")))          return LSCP_TK_BOOL_KEYWORD;
    if (string_match(str, STR("_Complex")))       return LSCP_TK_COMPLEX_KEYWORD;
    if (string_match(str, STR("_Generic")))       return LSCP_TK_GENERIC_KEYWORD;
    if (string_match(str, STR("_Imaginary")))     return LSCP_TK_IMAGINARY_KEYWORD;
    if (string_match(str, STR("_Noreturn")))      return LSCP_TK_NORETURN_KEYWORD;
    if (string_match(str, STR("_Static_assert"))) return LSCP_TK_STATIC_ASSERT_KEYWORD;
    if (string_match(str, STR("_Thread_local")))  return LSCP_TK_THREAD_LOCAL_KEYWORD;

    return LSCP_TK_UNKNOWN;
}
// endregion

LSCP_API lscp_bool lscp_is_primitive_type(lscp_token_type type)
{
    return type == LSCP_TK_INT_KEYWORD     ||
           type == LSCP_TK_FLOAT_KEYWORD   ||
           type == LSCP_TK_DOUBLE_KEYWORD  ||
           type == LSCP_TK_CHAR_KEYWORD    ||
           type == LSCP_TK_BOOL_KEYWORD    ||
           type == LSCP_TK_VOID_KEYWORD    ||
           type == LSCP_TK_COMPLEX_KEYWORD;
}

LSCP_API lscp_bool lscp_is_storage_class_specifier(lscp_token_type type)
{
    return type == LSCP_TK_AUTO_KEYWORD     ||
           type == LSCP_TK_REGISTER_KEYWORD ||
           type == LSCP_TK_STATIC_KEYWORD   ||
           type == LSCP_TK_EXTERN_KEYWORD   ||
           type == LSCP_TK_TYPEDEF_KEYWORD;
}

LSCP_API lscp_bool lscp_is_type_specifier(lscp_token_type type)
{
    return type == LSCP_TK_SIGNED_KEYWORD   ||
           type == LSCP_TK_UNSIGNED_KEYWORD ||
           type == LSCP_TK_SHORT_KEYWORD    ||
           type == LSCP_TK_LONG_KEYWORD;
}

LSCP_INTERNAL lscp_bool is_parsing(const lscp_parser* parser)
{
    return parser->curr < parser->tokens_count && parser->valid;
}

LSCP_INTERNAL lscp_token peek_token(lscp_parser* parser, int i)
{
    lscp_token result = {0};

    if (parser->curr + i < parser->tokens_count)
        result = parser->tokens[parser->curr + i];

    return result;
}

LSCP_INTERNAL lscp_token peek_tk_from(const lscp_parser* parser, lscp_int from, lscp_int peek_by)
{
    lscp_int target = from + peek_by;
    if (target < 0 || target > parser->tokens_count)
    {
        return parser->tokens[target];
    }

    return (lscp_token) { .type = LSCP_TK_UNKNOWN };
}

LSCP_API void print_decl(const lscp_parser* parser, lscp_int name_tk, lscp_ast_node* node)
{
    assert(node->type == LSCP_AST_DECLARATION);

    char text[1024] = {0};
    strncpy(text, parser->code + parser->tokens[name_tk].begin, parser->tokens[name_tk].len);

    printf("%s :: ", text);
    for (int ts = 0; ts < arrlen(node->decl.types); ts++)
    {
        switch (node->decl.types[ts].type)
        {
            case LSCP_TS_ARRAY: printf("[]"); break;
            case LSCP_TS_POINTER: printf("*"); break;
            case LSCP_TS_CONST: printf("const"); break;
            case LSCP_TS_RESTRICT: printf("restrict"); break;
            case LSCP_TS_UNSIGNED: printf("unsigned"); break;
            case LSCP_TS_SIGNED: printf("signed"); break;
            case LSCP_TS_INT: printf("int"); break;
            case LSCP_TS_SHORT: printf("short"); break;
            case LSCP_TS_LONG: printf("long"); break;
            case LSCP_TS_CHAR: printf("char"); break;
            case LSCP_TS_FLOAT: printf("float"); break;
            case LSCP_TS_DOUBLE: printf("double"); break;
            case LSCP_TS_FUNCTION: printf("() ->"); break;
            default: assert(0);
        }
        printf(" ");
    }

    printf("\n\n");
}

LSCP_INTERNAL lscp_bool is_struct_union_enum(lscp_token_type tk)
{
    return tk == LSCP_TK_STRUCT_KEYWORD || tk == LSCP_TK_UNION_KEYWORD || tk == LSCP_TK_ENUM_KEYWORD;
}

LSCP_INTERNAL lscp_ast_node parse_declaration(lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end);

lscp_ast_node* parse_params(lscp_parser* parser, lscp_tk_index params_begin, lscp_tk_index params_end)
{
    lscp_ast_node* params = NULL;
    lscp_tk_index begin = params_begin + 1;
    for (lscp_tk_index i = begin; i < params_end; i++)
    {
        if (parser->tokens[i].type == LSCP_TK_COMMA)
        {
            if (parser->tokens[i].type == LSCP_TK_DOTDOTDOT)
            {
                lscp_ast_node param = {
                    .type = LSCP_AST_FUNCTION_ARG,
                    .tk_begin = begin,
                };
            }
            else
            {
                lscp_ast_node param = parse_declaration(parser, begin, i);
                param.type = LSCP_AST_FUNCTION_ARG;
                arrput(params, param);
            }

            begin = i + 1;
        }
    }

    lscp_ast_node n = parse_declaration(parser, begin, params_end);
    if (n.valid)
    {
        arrput(params, n);
    }

    return params;
}

LSCP_INTERNAL lscp_ast_node parse_declaration(lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end)
{
    // Find name
    lscp_tk_index name = LSCP_BAD_INDEX;
    for (lscp_tk_index i = tk_begin; i < tk_end; i++)
    {
        if (parser->tokens[i].type == LSCP_TK_IDENTIFIER)
        {
            name = i;
            break;
        }
    }

    // Go left and right collecting the tokens
    lscp_type_specifier* specifiers = 0;
    {
        lscp_int left_i = name - 1;
        lscp_int right_i = name + 1;
        lscp_int going_right = right_i < tk_end;

        while (left_i >= tk_begin || right_i < tk_end)
        {
            if (going_right)
            {
                assert(right_i < tk_end);

                switch (parser->tokens[right_i].type)
                {
                    case LSCP_TK_SEMICOLON:
                    case LSCP_TK_EQUAL:
                    case LSCP_TK_CLOSE_PAREN:
                        going_right = 0;
                        break;

                    case LSCP_TK_STAR:
                    case LSCP_TK_CONST_KEYWORD:
                    case LSCP_TK_RESTRICT_KEYWORD:
                    case LSCP_TK_UNSIGNED_KEYWORD:
                    case LSCP_TK_SIGNED_KEYWORD:
                    case LSCP_TK_INT_KEYWORD:
                    case LSCP_TK_SHORT_KEYWORD:
                    case LSCP_TK_LONG_KEYWORD:
                    case LSCP_TK_CHAR_KEYWORD:
                    case LSCP_TK_FLOAT_KEYWORD:
                    case LSCP_TK_DOUBLE_KEYWORD:
                        assert(0);
                        break;

                    case LSCP_TK_OPEN_SQUARE_BRACKET:
                        arrput(specifiers, (lscp_type_specifier) { .type = LSCP_TS_ARRAY });
                        while (parser->tokens[right_i].type != LSCP_TK_CLOSE_SQUARE_BRACKET && right_i < tk_end)
                        {
                            right_i++;
                        }
                        assert(parser->tokens[right_i].type == LSCP_TK_CLOSE_SQUARE_BRACKET);
                        break;

                    case LSCP_TK_OPEN_PAREN:
                        lscp_type_specifier ts = { .type = LSCP_TS_FUNCTION };
                        lscp_tk_index params_begin = right_i;
                        while (parser->tokens[right_i].type != LSCP_TK_CLOSE_PAREN && right_i < tk_end)
                        {
                            right_i++;
                        }
                        assert(parser->tokens[right_i].type == LSCP_TK_CLOSE_PAREN);
                        lscp_tk_index params_end = right_i;
                        ts.func.args = parse_params(parser, params_begin, params_end);
                        arrput(specifiers, ts);
                        break;

                    default: assert(0);
                }

                right_i++;
                if (right_i == tk_end)
                {
                    going_right = 0;
                }
            }
            else
            {
                assert(left_i >= tk_begin);

                lscp_token_type tk = parser->tokens[left_i].type;

                switch (tk)
                {
                    case LSCP_TK_STATIC_KEYWORD:
                    case LSCP_TK_EXTERN_KEYWORD:
                    case LSCP_TK_REGISTER_KEYWORD:
                    case LSCP_TK_AUTO_KEYWORD:
                    case LSCP_TK_TYPEDEF_KEYWORD:

                    case LSCP_TK_STAR:
                    case LSCP_TK_CONST_KEYWORD:
                    case LSCP_TK_RESTRICT_KEYWORD:
                    case LSCP_TK_UNSIGNED_KEYWORD:
                    case LSCP_TK_SIGNED_KEYWORD:
                    case LSCP_TK_INT_KEYWORD:
                    case LSCP_TK_SHORT_KEYWORD:
                    case LSCP_TK_LONG_KEYWORD:
                    case LSCP_TK_CHAR_KEYWORD:
                    case LSCP_TK_FLOAT_KEYWORD:
                    case LSCP_TK_DOUBLE_KEYWORD:
                        arrput(specifiers, (lscp_type_specifier) { .type = tk });
                        break;

                    case LSCP_TK_OPEN_PAREN:
                        going_right = 1;
                        break;

                    default: assert(0);
                }

                left_i--;
                if (left_i < tk_begin)
                {
                    going_right = 1;
                }
            }
        }

        //arrdeln(type_specifiers, 0, arrlen(type_specifiers));
    }

    lscp_ast_node result = {
        .type = LSCP_AST_VARIABLE_DECL,
        .decl.types = specifiers,
        .decl.name = name
    };

    print_decl(parser, name, &result);

    return result;
}

// Find declaration end, a top-level ";" or "=" or "{"
LSCP_INTERNAL lscp_tk_index find_decl_end(const lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end)
{
    lscp_tk_index result = LSCP_BAD_INDEX;
    lscp_tk_index curr = parser->curr;
    lscp_token_type curr_tk = parser->tokens[curr].type;
    lscp_token_type* nesting_stack = 0;
    lscp_tk_index end = (curr_tk == LSCP_TK_SEMICOLON || curr_tk == LSCP_TK_EQUAL || curr_tk == LSCP_TK_OPEN_CURLY) && arrlen(nesting_stack) == 0;
    while (!end && curr < parser->tokens_count)
    {
        switch (curr_tk)
        {
            case LSCP_TK_OPEN_PAREN: arrput(nesting_stack, LSCP_TK_CLOSE_PAREN); break;
            case LSCP_TK_OPEN_SQUARE_BRACKET: arrput(nesting_stack, LSCP_TK_CLOSE_SQUARE_BRACKET); break;
            case LSCP_TK_OPEN_CURLY:
                if (is_struct_union_enum(peek_tk_from(parser, curr_tk, -1).type) ||
                   (peek_tk_from(parser, curr_tk, -1).type == LSCP_TK_IDENTIFIER && is_struct_union_enum(peek_tk_from(parser, curr_tk, -2).type)))
                {
                    arrput(nesting_stack, LSCP_TK_OPEN_CURLY);
                }
                break;

            case LSCP_TK_CLOSE_PAREN:
            case LSCP_TK_CLOSE_SQUARE_BRACKET:
            case LSCP_TK_CLOSE_CURLY:
                assert(nesting_stack[arrlen(nesting_stack) - 1] == LSCP_TK_CLOSE_PAREN);
                arrdel(nesting_stack, arrlen(nesting_stack) - 1);
                break;

            default: break;
        }

        curr++;
        curr_tk = parser->tokens[curr].type;
        end = (curr_tk == LSCP_TK_SEMICOLON || curr_tk == LSCP_TK_EQUAL || curr_tk == LSCP_TK_OPEN_CURLY) && arrlen(nesting_stack) == 0;
    }

    assert(arrlen(nesting_stack) == 0);

    if (end)
    {
        result = curr;
    }

    return result;
}

LSCP_INTERNAL void parse_translation_unit(const char* code, lscp_int code_len, const lscp_token* tks, lscp_int tks_count)
{
    lscp_parser parser = {
        .code = code,
        .code_len = code_len,
        .tokens = tks,
        .tokens_count = tks_count,
        .curr = 0,
        .valid = 1
    };

    while (is_parsing(&parser))
    {
        lscp_int decl_end = find_decl_end(&parser, parser.curr, parser.tokens_count);
        assert(decl_end != LSCP_BAD_INDEX);
        lscp_ast_node decl = parse_declaration(&parser, parser.curr, decl_end);
        parser.curr = decl_end + 1;
    }

    printf("");
}

LSCP_API lscp_parse_result lscp_parse(const char* code, lscp_int len)
{
    lscp_parse_result result = {0};
    lscp_tokenizer_result tks = lscp_tokenize(code, len);

    parse_translation_unit(code, len, tks.tokens, tks.count);

    if (!tks.error)
    {
        result.tokens       = tks.tokens;
        result.tokens_count = tks.count;
        result.valid        = result.toplevel.type != LSCP_AST_UNKNOWN;
    }

    return result;
}

LSCP_API lscp_parse_result lscp_parse_cstr(const char* code) { return lscp_parse(code, strlen(code)); }