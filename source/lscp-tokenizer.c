#include "lscp-common-internal.h"
#include "lscp-internal-utils.h"
#include "lscp-tokenizer.h"
#include "malloc.h"
#include "string.h"

// Returns the position of the char
lscp_internal lscp_int lscp_tokenizer_find_char(const lscp_tokenizer* ctx, char until_c)
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

lscp_api lscp_tokenizer lscp_tokenizer_create(const char* src, lscp_int src_len)
{
    return (lscp_tokenizer) { src, src_len, };
}

lscp_api lscp_bool lscp_tokenizer_is_done(lscp_tokenizer ctx)
{
    return ctx.curr >= ctx.len || ctx.error;
}

lscp_api lscp_token lscp_get_next_token(lscp_tokenizer* ctx)
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
                    lscp_int ending_quote_pos = lscp_bad_index; // Position of the ending quotation mark

                    ctx->curr++;

                    // Look until the end of the string for an ending quotation mark
                    for (lscp_int escape_next_char = 0; ctx->curr < ctx->len && ending_quote_pos == -1; ctx->curr++)
                    {
                        if (ctx->code[ctx->curr] == '\n')
                        {
                            //ctx->error = lscp_string_literal_not_closed;
                            ctx->error = 1;
                            return (lscp_token) {0};
                        }

                        if (ctx->code[ctx->curr] == '"' && !escape_next_char)
                        {
                            ending_quote_pos = ctx->curr;
                        }

                        escape_next_char = ctx->code[ctx->curr] == '\\' && !escape_next_char;
                    }

                    if (ending_quote_pos == lscp_bad_index)
                    {
                        //ctx->error = lscp_string_literal_not_closed;
                        ctx->error = 1;
                        return (lscp_token) {0};
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
                        lscp_str s = {&code[begin], len };

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

                        lscp_str str = {&code[begin], ctx->curr - begin };

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

    return (lscp_token) {0};
}

lscp_api lscp_tokenizer_result lscp_tokenize(const char* src, lscp_int src_len)
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
                if (tk.type != LSCP_TK_UNKNOWN) dst[curr_count++] = tk;
            }

            result.tokens = dst;
            result.count = curr_count;
            result.buffer_size = (lscp_int) (dst_count * sizeof(lscp_token));
            result.error = tk_ctx.error;
        }
    }

    return result;
}