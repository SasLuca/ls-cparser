#include <lscparser.h>
#include <stdint.h>
#include "stdio.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "string.h"

#define LSCP_INDEX_NOT_FOUND (-1)
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

//region tokenizer

// Returns the position of the char
LSCP_INTERNAL lscp_int lscp_tokenizer_find_char(const lscp_tokenizer* ctx, char until_c)
{
    for (lscp_int i = ctx->curr; i < ctx->len; i++)
    {
        if (ctx->code[ctx->curr] == until_c)
        {
            return i;
        }
    }

    return ctx->len;
}

LSCP_API lscp_tokenizer lscp_tokenizer_create(const char* src, lscp_int src_len)
{
    return (lscp_tokenizer) { src, src_len, };
}

LSCP_API lscp_bool lscp_tokenizer_is_done(lscp_tokenizer ctx)
{
    return ctx.curr >= ctx.len || ctx.error;
}

LSCP_API lscp_token lscp_get_next_token(lscp_tokenizer* ctx)
{
    if (ctx && !ctx->error)
    {
        // Aliases for convenience
        const char* code = ctx->code;

        while (ctx->curr < ctx->len)
        {
            switch (code[ctx->curr])
            {
                // region Whitespace
                case '\n':
                {
                    ctx->curr_line++;
                    ctx->tokens_on_line_count = 0;

                    ctx->curr++;
                    continue;
                }
                case '\t':
                case '\r':
                case ' ' :
                {
                    ctx->curr++;
                    continue;
                }
                //endregion

                //region Symbols
                #define LSCP__ONE_CHAR_TOKEN_CASE(c, t) case c: return (lscp_token) { .type = t, .begin = ctx->curr++, .len = 1, .line = ctx->curr_line, .number_in_line = ctx->tokens_on_line_count++, }
                LSCP__ONE_CHAR_TOKEN_CASE('~',  LSCP_TK_TILDE);
                LSCP__ONE_CHAR_TOKEN_CASE('!',  LSCP_TK_EXCLAMATION_MARK);
                LSCP__ONE_CHAR_TOKEN_CASE('%',  LSCP_TK_PERCENT);
                LSCP__ONE_CHAR_TOKEN_CASE('^',  LSCP_TK_HAT);
                LSCP__ONE_CHAR_TOKEN_CASE('&',  LSCP_TK_AMPERSAND);
                LSCP__ONE_CHAR_TOKEN_CASE('*',  LSCP_TK_STAR);
                LSCP__ONE_CHAR_TOKEN_CASE('(',  LSCP_TK_OPEN_PAREN);
                LSCP__ONE_CHAR_TOKEN_CASE(')',  LSCP_TK_CLOSE_PAREN);
                LSCP__ONE_CHAR_TOKEN_CASE('-',  LSCP_TK_MINUS);
                LSCP__ONE_CHAR_TOKEN_CASE('+',  LSCP_TK_PLUS);
                LSCP__ONE_CHAR_TOKEN_CASE('=',  LSCP_TK_EQUAL);
                LSCP__ONE_CHAR_TOKEN_CASE(':',  LSCP_TK_COLON);
                LSCP__ONE_CHAR_TOKEN_CASE('<',  LSCP_TK_LEFT_ARROW);
                LSCP__ONE_CHAR_TOKEN_CASE('>',  LSCP_TK_RIGHT_ARROW);
                LSCP__ONE_CHAR_TOKEN_CASE(',',  LSCP_TK_COMMA);
                LSCP__ONE_CHAR_TOKEN_CASE('.',  LSCP_TK_DOT);
                LSCP__ONE_CHAR_TOKEN_CASE('[',  LSCP_TK_OPEN_SQUARE_BRACKET);
                LSCP__ONE_CHAR_TOKEN_CASE(']',  LSCP_TK_CLOSE_SQUARE_BRACKET);
                LSCP__ONE_CHAR_TOKEN_CASE('{',  LSCP_TK_OPEN_CURLY);
                LSCP__ONE_CHAR_TOKEN_CASE('}',  LSCP_TK_CLOSE_CURLY);
                LSCP__ONE_CHAR_TOKEN_CASE('|',  LSCP_TK_BAR);
                LSCP__ONE_CHAR_TOKEN_CASE(';',  LSCP_TK_SEMICOLON);
                LSCP__ONE_CHAR_TOKEN_CASE('/',  LSCP_TK_FORWARD_SLASH);
                LSCP__ONE_CHAR_TOKEN_CASE('\\', LSCP_TK_BACKWARD_SLASH);
                #undef LSCP__ONE_CHAR_TOKEN_CASE
                //endregion

                // String literals
                case '"':
                {
                    lscp_int begin = ctx->curr; // Position of the first quotation mark
                    lscp_int ending_quote_pos = LSCP_INDEX_NOT_FOUND; // Position of the ending quotation mark

                    ctx->curr++;

                    // Look until the end of the string for an ending quotation mark
                    for (lscp_int escape_next_char = 0; ctx->curr < ctx->len && ending_quote_pos == -1; ctx->curr++)
                    {
                        if (ctx->code[ctx->curr] == '\n')
                        {
                            //ctx->error = lscp_string_literal_not_closed;
                            ctx->error = 1;
                            return (lscp_token) {};
                        }

                        if (ctx->code[ctx->curr] == '"' && !escape_next_char)
                        {
                            ending_quote_pos = ctx->curr;
                        }

                        escape_next_char = ctx->code[ctx->curr] == '\\' && !escape_next_char;
                    }

                    if (ending_quote_pos == LSCP_INDEX_NOT_FOUND)
                    {
                        //ctx->error = lscp_string_literal_not_closed;
                        ctx->error = 1;
                        return (lscp_token) {};
                    }

                    return (lscp_token)
                    {
                        .type  = LSCP_TK_STRING_LITERAL,
                        .begin = begin,
                        .len   = ctx->curr - begin,
                        .line  = ctx->curr_line,
                        .number_in_line = ctx->tokens_on_line_count++,
                    };
                }

                default:
                {
                    // Numbers
                    if (is_digit(code[ctx->curr]))
                    {
                        const int begin = ctx->curr;

                        int found_dot = 0;

                        // Eat until whitespace or symbol
                        while (ctx->curr < ctx->len)
                        {
                                 if (code[ctx->curr] == '.') found_dot++;
                            else if (is_whitespace(code[ctx->curr]) || is_symbol(code[ctx->curr])) break;

                            ctx->curr++;
                        }

                        const int len = ctx->curr - begin;
                        lscp_str s = { &code[begin], len };

                        if (found_dot)
                        {

                        }
                        else
                        {
                            int suffix_begin = ctx->curr;
                            if (ends_with_zstr(s, "ll") || ends_with_zstr(s, "LL")) suffix_begin = len - 2;
                            else if (code[len - 1] == 'u' || code[len - 1] == 'U' || code[len - 1] == 'l' || code[len - 1] == 'L') suffix_begin = len - 1;

                            if (code[begin] == '0')
                            {
                                if (starts_with_zstr(s, "0x") || starts_with_zstr(s, "0X"))
                                {

                                }
                                else
                                {
                                    // Check if it's valid octal
                                    for (int i = begin; i < suffix_begin; i++)
                                    {
                                        // Valid octal digits are between 0 and 7
                                        if (!(code[i] >= '0' && code[i] <= '7'))
                                        {
                                            ctx->error = 1;
                                            return (lscp_token) {0};
                                        }
                                    }
                                }
                            }
                        }

                        return (lscp_token)
                        {
                            .type   = LSCP_TK_NUMERIC_LITERAL,
                            .begin   = begin,
                            .len    = ctx->curr - begin,
                            .line   = ctx->curr_line,
                            .number_in_line = ctx->tokens_on_line_count++,
                        };
                    }

                    // Identifiers and keywords
                    else if (is_letter(code[ctx->curr]) || code[ctx->curr] == '_')
                    {
                        const lscp_int begin = ctx->curr;

                        ctx->curr++;

                        // @Todo: Add utf8 parsing to check for valid identifiers in C
                        // Eat characters until its no longer a valid word (http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf check Annex D)
                        while (ctx->curr < ctx->len && // There are characters left
                              (is_letter(code[ctx->curr]) || code[ctx->curr] == '_' ||
                               is_digit(code[ctx->curr]))) // Is a letter, _ or digit
                        {
                            ctx->curr++;
                        }

                        lscp_str str = { &code[begin], ctx->curr - begin };

                        // Check if its a keyword, if it's not then it's an identifier
                        lscp_token_type maybe_type = get_token_type_if_keyword(str);
                        lscp_token_type token_type = maybe_type ? maybe_type : LSCP_TK_IDENTIFIER;

                        return (lscp_token) {
                            .type   = token_type,
                            .begin   = begin,
                            .len    = str.len,
                            .line   = ctx->curr_line,
                            .number_in_line = ctx->tokens_on_line_count++,
                        };
                    }
                }
            }
        }
    }

    return (lscp_token) {};
}

LSCP_API lscp_tokenizer_result lscp_tokenize(const char* src, lscp_int src_len)
{
    lscp_tokenizer_result result = {0};

    if (src && src_len > 0)
    {
        lscp_int dst_count = ((src_len / 2 == 0) ? (src_len) : (src_len / 2));
        lscp_token* dst = malloc(dst_count * sizeof(lscp_token));

        if (dst)
        {
            lscp_int curr_count = 0;
            lscp_tokenizer tk_ctx = lscp_tokenizer_create(src, src_len);

            while (!lscp_tokenizer_is_done(tk_ctx))
            {
                if (curr_count == dst_count)
                {
                    lscp_int new_count = dst_count * 2;
                    lscp_token* new_dst = malloc(new_count * sizeof(lscp_token));

                    memcpy(new_dst, dst, dst_count * sizeof(lscp_token));
                    free(dst);

                    dst = new_dst;
                    dst_count = new_count;
                }

                lscp_token tk = lscp_get_next_token(&tk_ctx);
                dst[curr_count++] = tk;
            }

            result.tokens = dst;
            result.count = curr_count;
            result.buffer_size = (lscp_int) (dst_count * sizeof(lscp_token));
            result.error = tk_ctx.error;
        }
    }

    return result;
}

//endregion

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

typedef enum type_specifier
{
    ARRAY = LSCP_TK_OPEN_SQUARE_BRACKET,
    POINTER = LSCP_TK_STAR,
    CONST = LSCP_TK_CONST_KEYWORD,
    RESTRICT = LSCP_TK_RESTRICT_KEYWORD,
    UNSIGNED = LSCP_TK_UNSIGNED_KEYWORD,
    SIGNED = LSCP_TK_SIGNED_KEYWORD,
    INT = LSCP_TK_INT_KEYWORD,
    SHORT = LSCP_TK_SHORT_KEYWORD,
    LONG = LSCP_TK_LONG_KEYWORD,
    CHAR = LSCP_TK_CHAR_KEYWORD,
    FLOAT = LSCP_TK_FLOAT_KEYWORD,
    DOUBLE = LSCP_TK_DOUBLE_KEYWORD,
    FUNCTION = LSCP_TK_OPEN_PAREN
} type_specifier;

void print_decl(lscp_parser* parser, lscp_int name_tk, type_specifier* specifiers)
{
    char text[1024] = {0};
    strncpy(text, parser->code + parser->tokens[name_tk].begin, parser->tokens[name_tk].len);

    printf("%s :: ", text);
    for (int ts = 0; ts < arrlen(specifiers); ts++)
    {
        switch (specifiers[ts])
        {
            case ARRAY: printf("[]"); break;
            case POINTER: printf("*"); break;
            case CONST: printf("const"); break;
            case RESTRICT: printf("restrict"); break;
            case UNSIGNED: printf("unsigned"); break;
            case SIGNED: printf("signed"); break;
            case INT: printf("int"); break;
            case SHORT: printf("short"); break;
            case LONG: printf("long"); break;
            case CHAR: printf("char"); break;
            case FLOAT: printf("float"); break;
            case DOUBLE: printf("double"); break;
            case FUNCTION: printf("() ->"); break;
        }
        printf(" ");
    }

    printf("\n\n");
}

lscp_token peek_tk_from(lscp_parser* parser, lscp_int from, lscp_int peek_by)
{
    lscp_int target = from + peek_by;
    if (target < 0 || target > parser->tokens_count)
    {
        return parser->tokens[target];
    }

    return (lscp_token) { .type = LSCP_TK_UNKNOWN };
}

lscp_bool is_struct_union_enum(lscp_token_type tk)
{
    return tk == LSCP_TK_STRUCT_KEYWORD || tk == LSCP_TK_UNION_KEYWORD || tk == LSCP_TK_ENUM_KEYWORD;
}

LSCP_INTERNAL void parse_declaration(lscp_parser* parser)
{
    // Find declaration end, a top-level ";" or "=" or "{"
    int decl_end = LSCP_INDEX_NOT_FOUND;
    {
        int curr = parser->curr;
        lscp_token_type curr_tk = parser->tokens[curr].type;
        lscp_token_type* nesting_stack = 0;
        int end = (curr_tk == LSCP_TK_SEMICOLON || curr_tk == LSCP_TK_EQUAL || curr_tk == LSCP_TK_OPEN_CURLY) && arrlen(nesting_stack) == 0;
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
            decl_end = curr;
        }
    }

    // Find name
    int name = LSCP_INDEX_NOT_FOUND;
    for (int i = parser->curr; i < decl_end; i++)
    {
        if (parser->tokens[i].type == LSCP_TK_IDENTIFIER)
        {
            name = i;
            break;
        }
    }

    // Go left and right collecting the tokens
    type_specifier* specifiers = 0;
    {
        const int target_begin = parser->curr;
        const int target_end   = decl_end;

        int left_i = name - 1;
        int right_i = name + 1;
        int going_right = 1;

        while (left_i >= target_begin || right_i < target_end)
        {
            if (going_right)
            {
                assert(right_i < target_end);

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
                        arrput(specifiers, parser->tokens[right_i].type);
                        while (parser->tokens[right_i].type != LSCP_TK_CLOSE_SQUARE_BRACKET && right_i < target_end)
                        {
                            right_i++;
                        }
                        assert(parser->tokens[right_i].type == LSCP_TK_CLOSE_SQUARE_BRACKET);
                        break;

                    case LSCP_TK_OPEN_PAREN:
                        arrput(specifiers, parser->tokens[right_i].type);
                        while (parser->tokens[right_i].type != LSCP_TK_CLOSE_PAREN && right_i < target_end)
                        {
                            right_i++;
                        }
                        assert(parser->tokens[right_i].type == LSCP_TK_CLOSE_PAREN);
                        break;

                    default: assert(0);
                }

                right_i++;
                if (right_i == target_end)
                {
                    going_right = 0;
                }
            }
            else
            {
                assert(left_i >= target_begin);

                lscp_token_type tk = parser->tokens[left_i].type;

                switch (tk)
                {
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
                    case LSCP_TK_STATIC_KEYWORD:
                    case LSCP_TK_EXTERN_KEYWORD:
                    case LSCP_TK_REGISTER_KEYWORD:
                    case LSCP_TK_AUTO_KEYWORD:
                    case LSCP_TK_TYPEDEF_KEYWORD:
                        arrput(specifiers, tk);
                        break;

                    case LSCP_TK_OPEN_PAREN:
                        going_right = 1;
                        break;

                    default: assert(0);
                }

                left_i--;
                if (left_i < target_begin)
                {
                    going_right = 1;
                }
            }
        }

        parser->curr = decl_end + 1;
        //arrdeln(type_specifiers, 0, arrlen(type_specifiers));
    }

    print_decl(parser, name, specifiers);
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
        parse_declaration(&parser);
    }
}

LSCP_API lscp_parse_result lscp_parse_cstr(const char* code)
{
    return lscp_parse(code, strlen(code));
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