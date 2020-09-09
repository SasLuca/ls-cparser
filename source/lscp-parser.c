#include "lscp-common-internal.h"
#include "lscp-parser.h"
#include "lscp-internal-utils.h"

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

#include "stb_ds.h"
#include "lscp-preprocessor.h"

lscp_api lscp_bool lscp_is_primitive_type(lscp_token_type type)
{
    return type == LSCP_TK_INT_KEYWORD     ||
           type == LSCP_TK_FLOAT_KEYWORD   ||
           type == LSCP_TK_DOUBLE_KEYWORD  ||
           type == LSCP_TK_CHAR_KEYWORD    ||
           type == LSCP_TK_BOOL_KEYWORD    ||
           type == LSCP_TK_VOID_KEYWORD    ||
           type == LSCP_TK_COMPLEX_KEYWORD;
}

lscp_api lscp_bool lscp_is_storage_class_specifier(lscp_token_type type)
{
    return type == LSCP_TK_AUTO_KEYWORD     ||
           type == LSCP_TK_REGISTER_KEYWORD ||
           type == LSCP_TK_STATIC_KEYWORD   ||
           type == LSCP_TK_EXTERN_KEYWORD   ||
           type == LSCP_TK_TYPEDEF_KEYWORD;
}

lscp_api lscp_bool lscp_is_type_specifier(lscp_token_type type)
{
    return type == LSCP_TK_SIGNED_KEYWORD   ||
           type == LSCP_TK_UNSIGNED_KEYWORD ||
           type == LSCP_TK_SHORT_KEYWORD    ||
           type == LSCP_TK_LONG_KEYWORD;
}

lscp_internal lscp_bool is_parsing(const lscp_parser* parser)
{
    return parser->curr < parser->tokens_count && parser->valid;
}

lscp_internal lscp_token peek_token(lscp_parser* parser, int i)
{
    lscp_token result = {0};

    if (parser->curr + i < parser->tokens_count)
        result = parser->tokens[parser->curr + i];

    return result;
}

lscp_internal lscp_token peek_tk_from(const lscp_parser* parser, lscp_int from, lscp_int peek_by)
{
    lscp_int target = from + peek_by;
    if (target < 0 || target > parser->tokens_count)
    {
        return parser->tokens[target];
    }

    return (lscp_token) { .type = LSCP_TK_UNKNOWN };
}

lscp_internal  void print_ts_type(lscp_ts_type t)
{
    switch (t)
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
        case LSCP_TS_TYPEDEF: printf("typedef"); break;
        case LSCP_TS_VOID: printf("void"); break;
        default: assert(0);
    }
}

lscp_api char* lscp_get_tk_source(const char* code, const lscp_token* tokens, lscp_int name_tk)
{
    static lscp_thread_local char text[1024] = {0};
    memset(text, 0, sizeof(text));
    strncpy(text, code + tokens[name_tk].begin, tokens[name_tk].len);
    return text;
}

lscp_internal void print_enum(const lscp_parse_result* p, lscp_ast_node* node)
{
    printf("\"%s\": { \"type\": \"enum\", \"constants\": [ ", lscp_get_tk_source(p->src, p->tokens, node->tk_begin + 1));

    for (int it = 0; it < arrlen(node->enumeration.constants); it++)
    {
        if (it != 0) printf(", ");
        printf("\"%s\"", lscp_get_tk_source(p->src, p->tokens, node->enumeration.constants[it]));
    }

    printf(" ] }");
}

lscp_internal void print_decl(const lscp_parse_result* p, lscp_ast_node* node, lscp_bool nested)
{
    if (!nested) printf("\"%s\": \"", lscp_get_tk_source(p->src, p->tokens, node->decl.name));
    else printf("%s: ", lscp_get_tk_source(p->src, p->tokens, node->decl.name));

    for (int ts = 0; ts < arrlen(node->decl.types); ts++)
    {
        lscp_ts_type t = node->decl.types[ts].type;

        if (ts != 0) printf(" ");

        if (t == LSCP_TS_FUNCTION)
        {
            printf("(");
            for (int it = 0; it < arrlen(node->decl.types->func.args); it++)
            {
                if (it != 0) printf(", ");
                print_decl(p, &node->decl.types->func.args[it], 1);
            }
            printf(") ->");
        }
        else print_ts_type(t);
    }
    if (!nested) printf("\"");
}

lscp_internal void print_struct_or_union(const lscp_parse_result* p, lscp_ast_node* node)
{
    const char* struct_or_union_str = node->type == LSCP_AST_STRUCT_DECL ? "struct" : "union";
    printf("\"%s\": { \"type\": \"%s\", \"members\": { ", lscp_get_tk_source(p->src, p->tokens, node->tk_begin + 1), struct_or_union_str);

    for (int it = 0; it < arrlen(node->structure.members); it++)
    {
        if (it != 0) printf(", ");
        print_decl(p, &node->structure.members[it], 0);
    }

    printf(" } }");
}

lscp_internal lscp_bool is_struct_union_enum(lscp_token_type tk)
{
    return tk == LSCP_TK_STRUCT_KEYWORD || tk == LSCP_TK_UNION_KEYWORD || tk == LSCP_TK_ENUM_KEYWORD;
}

lscp_internal lscp_ast_node parse_declaration(lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end);

lscp_api void lscp_ast_to_json(lscp_parse_result p)
{
    printf(" { ");
    //printf("preprocessed source: \"%s\"");

    for (int i = 0; i < arrlen(p.nodes); i++)
    {
        if (i != 0) printf(", ");

        lscp_ast_node* node = &p.nodes[i];

        if (node->type == LSCP_AST_STRUCT_DECL || node->type == LSCP_AST_UNION_DECL)
        {
            print_struct_or_union(&p, node);
        }
        else if (node->type == LSCP_AST_ENUM_DECL)
        {
            print_enum(&p, node);
        }
        else if (node->type == LSCP_AST_VARIABLE_DECL)
        {
            print_decl(&p, node, 0);
        }
    }
    printf(" } ");
}

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

lscp_internal lscp_ast_node parse_declaration(lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end)
{
    // Find name
    lscp_tk_index name = lscp_bad_index;
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
                    {
                        going_right = 0;
                        break;
                    }

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
                    {
                        assert(0);
                        break;
                    }

                    case LSCP_TK_OPEN_SQUARE_BRACKET:
                    {
                        arrput(specifiers, (lscp_type_specifier) { .type = LSCP_TS_ARRAY });
                        while (parser->tokens[right_i].type != LSCP_TK_CLOSE_SQUARE_BRACKET && right_i < tk_end)
                        {
                            right_i++;
                        }
                        assert(parser->tokens[right_i].type == LSCP_TK_CLOSE_SQUARE_BRACKET);
                        if (right_i == tk_end)
                        {
                            going_right = 0;
                        }
                        break;
                    }

                    case LSCP_TK_OPEN_PAREN:
                    {
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
                        if (right_i == tk_end)
                        {
                            going_right = 0;
                        }
                        break;
                    }

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
                    case LSCP_TK_VOID_KEYWORD:
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

    return result;
}

// Find declaration end, a top-level ";" or "=" or "{"
lscp_internal lscp_tk_index find_decl_end(const lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end)
{
    lscp_tk_index result = lscp_bad_index;
    lscp_tk_index curr = tk_begin;
    lscp_token_type curr_tk = parser->tokens[curr].type;
    lscp_token_type* nesting_stack = 0;
    lscp_tk_index end = (curr_tk == LSCP_TK_SEMICOLON || curr_tk == LSCP_TK_EQUAL || curr_tk == LSCP_TK_OPEN_CURLY) && arrlen(nesting_stack) == 0;
    while (!end && curr < tk_end)
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
                if (arrlen(nesting_stack)) arrdel(nesting_stack, arrlen(nesting_stack) - 1);
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

lscp_internal lscp_tk_index find_next_pair_end(const lscp_parser* parser, lscp_tk_index tk_begin, lscp_tk_index tk_end, lscp_token_type open, lscp_token_type close)
{
    lscp_tk_index result = lscp_bad_index;
    lscp_tk_index curr = tk_begin + 1;
    lscp_token_type curr_tk = parser->tokens[curr].type;
    int ct = 0;
    while (curr < tk_end)
    {
        if (curr_tk == open)
        {
            ct++;
        }

        if (curr_tk == close)
        {
            if (ct) ct--;
            else
            {
                result = curr;
                break;
            }
        }

        curr++;
        curr_tk = parser->tokens[curr].type;
    }

    return result;
}

lscp_internal lscp_ast_node* parse_translation_unit(const char* code, lscp_int code_len, const lscp_token* tks, lscp_int tks_count)
{
    lscp_parser parser = {
        .code = code,
        .code_len = code_len,
        .tokens = tks,
        .tokens_count = tks_count,
        .curr = 0,
        .valid = 1
    };

    lscp_ast_node* nodes_result = 0;

    while (is_parsing(&parser))
    {
        lscp_tk_index decl_end = find_decl_end(&parser, parser.curr, parser.tokens_count);
        lscp_tk_index o_decl_end = decl_end;
        if (decl_end == lscp_bad_index) break;

        if (tks[decl_end].type == LSCP_TK_OPEN_CURLY && is_struct_union_enum(tks[decl_end - 2].type))
        {
            lscp_tk_index begin = decl_end;
            lscp_tk_index end = find_next_pair_end(&parser, decl_end, parser.tokens_count, LSCP_TK_OPEN_CURLY, LSCP_TK_CLOSE_CURLY);
            assert(end != lscp_bad_index);

            if (tks[decl_end - 2].type == LSCP_TK_ENUM_KEYWORD)
            {
                lscp_ast_node node = {
                    .tk_begin = begin - 2,
                    .tk_count = end - begin - 2,
                    .enumeration.name = begin - 1,
                    .type = LSCP_AST_ENUM_DECL
                };

                for (lscp_tk_index i = begin + 1; i < end; i++)
                {
                    assert(parser.tokens[i].type == LSCP_TK_IDENTIFIER || parser.tokens[i].type == LSCP_TK_COMMA);
                    if (parser.tokens[i].type == LSCP_TK_IDENTIFIER)
                    {
                        arrput(node.enumeration.constants, i);
                    }
                }

                //print_enum(&parser, &node);

                parser.curr = end + 3;

                arrput(nodes_result, node);
            }
            else if (tks[decl_end - 2].type == LSCP_TK_STRUCT_KEYWORD || tks[decl_end - 2].type == LSCP_TK_UNION_KEYWORD)
            {
                lscp_ast_node node = {
                    .tk_begin = begin - 2,
                    .tk_count = end - begin - 2,
                    .structure.name = begin - 1,
                    .type = (tks[decl_end - 2].type == LSCP_TK_STRUCT_KEYWORD) ? LSCP_AST_STRUCT_DECL : LSCP_AST_UNION_DECL,
                };

                begin++;
                while (begin < end)
                {
                    decl_end = find_decl_end(&parser, begin, parser.tokens_count);
                    lscp_ast_node decl = parse_declaration(&parser, begin, decl_end);
                    arrput(node.structure.members, decl);
                    begin = decl_end + 1;
                }

                parser.curr = decl_end + 3;

                arrput(nodes_result, node);
            }
        }
        else
        {
            lscp_ast_node decl = parse_declaration(&parser, parser.curr, decl_end);
            parser.curr = decl_end + 1;

            arrput(nodes_result, decl);

//            printf("%s: ", lscp_get_tk_source(&parser, decl.decl.name));
//            print_decl(&parser, &decl);
//            printf("\n");
        }

        if (o_decl_end == LSCP_TK_EQUAL)
        {
            int ct = 0;
            for (lscp_tk_index i = o_decl_end + 1; i < parser.tokens_count - parser.curr; i++)
            {
                if (parser.tokens[i].type == LSCP_TK_OPEN_CURLY) ct++;
                if (parser.tokens[i].type == LSCP_TK_CLOSE_CURLY) ct--;
                if (parser.tokens[i].type == LSCP_TK_SEMICOLON && ct == 0)
                {
                    parser.curr = i + 1;
                        break;
                }
            }
        }
        else if (o_decl_end == LSCP_TK_OPEN_CURLY)
        {
            int ct = 0;
            for (lscp_tk_index i = o_decl_end + 1; i < parser.tokens_count - parser.curr; i++)
            {
                if (parser.tokens[i].type == LSCP_TK_OPEN_CURLY) ct++;
                if (parser.tokens[i].type == LSCP_TK_CLOSE_CURLY)
                {
                    if (ct == 0)
                    {
                        parser.curr = i + 1;
                        break;
                    }
                    else ct--;
                }
            }
        }
    }

    return nodes_result;
}

lscp_api lscp_parse_result lscp_parse(const char* code, lscp_int len)
{
    lscp__global_allocator = lscp_default_allocator;
    lscp_parse_result result = {0};
    lscp_str src = lscp_remove_comments(code, len, lscp_default_allocator);
    lscp_preprocessor_result r = lscp_preprocess_code(src.chars, src.len, lscp_default_allocator);
    result.src = r.result;
    result.src_size = r.result_size;
    if (!r.valid)
    {

    }
    lscp_tokenizer_result tks = lscp_tokenize(r.result, r.result_size);

    result.nodes = parse_translation_unit(r.result, r.result_size, tks.tokens, tks.count);

    if (!tks.error)
    {
        result.tokens       = tks.tokens;
        result.tokens_count = tks.count;
        result.valid        = result.nodes->type != LSCP_AST_UNKNOWN;
    }

    return result;
}

lscp_api lscp_parse_result lscp_parse_cstr(const char* code) { return lscp_parse(code, strlen(code)); }