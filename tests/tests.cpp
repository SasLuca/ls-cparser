#include "catch.hpp"
#include "string.h"
#include "lscp.h"
#include "lscp-internal-utils.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

TEST_CASE("LSCP_C_KEYWORDS_COUNT is correct")
{
    const char* keywords[] = LSCP_C_KEYWORDS;
    REQUIRE(LSCP_C_KEYWORDS_COUNT == ARRAY_SIZE(keywords));
}

TEST_CASE("LSCP_C_SYMBOLS_COUNT is correct")
{
    const char chars[]         = LSCP_C_SYMBOLS;
    const lscp_token_type tk[] = LSCP_C_SYMBOLS_TOKENS;

    REQUIRE(sizeof(chars) - 1 == LSCP_C_SYMBOLS_COUNT); // -1 for the null terminator in the string
    REQUIRE(ARRAY_SIZE(tk) == LSCP_C_SYMBOLS_COUNT);
}

TEST_CASE("Remove single comment", "[preprocessor]")
{
    const char chars[]   = "test // test";
    const int chars_size = sizeof(chars);

    lscp_str result = lscp_remove_comments(chars, chars_size, lscp_default_allocator);

    REQUIRE(str_match_lit(result, "test "));
}

TEST_CASE("Remove multiline comment", "[preprocessor]")
{
    const char chars[]   = "test /* test */";
    const int chars_size = sizeof(chars);

    lscp_str result = lscp_remove_comments(chars, chars_size, lscp_default_allocator);

    REQUIRE(str_match_lit(result, "test "));
}

TEST_CASE("Tokenize symbols and ensure same order in LSCP_C_SYMBOLS_TOKENS and LSCP_C_SYMBOLS", "[tokenizer]")
{
    const lscp_token_type tk[] = LSCP_C_SYMBOLS_TOKENS;
    const char chars[]         = LSCP_C_SYMBOLS;

    for (int i = 0; i < LSCP_C_SYMBOLS_COUNT; i++)
    {
        lscp_tokenizer_result result = lscp_tokenize(&chars[i], 1);

        REQUIRE(!result.error);
        REQUIRE(result.count == 1);
        REQUIRE(result.tokens[0].type == tk[i]);
    }
}

TEST_CASE("Tokenize keywords", "[tokenizer]")
{
    const char* keywords[]     = LSCP_C_KEYWORDS;
    const int   keywords_count = LSCP_C_KEYWORDS_COUNT;

    for (int i = 0; i < keywords_count; i++)
    {
        lscp_tokenizer tokenizer = lscp_tokenizer_create(keywords[i], strlen(keywords[i]));
        lscp_token tk = lscp_get_next_token(&tokenizer);

        REQUIRE(tk.type != LSCP_TK_UNKNOWN);
        REQUIRE(lscp_tokenizer_is_done(tokenizer));
    }
}

TEST_CASE("Tokenize single digit integers", "[tokenizer]")
{
    for (int i = 0; i < 10; i++)
    {
        const char digit = '0' + i;

        lscp_tokenizer_result result = lscp_tokenize(&digit, 1);

        REQUIRE(!result.error);
        REQUIRE(result.count == 1);
        REQUIRE(result.tokens[0].type == LSCP_TK_NUMERIC_LITERAL);
    }
}

TEST_CASE("Tokenize integers 0 to 1000", "[tokenizer]")
{
    for (int i = 0; i < 1000; i++)
    {
        char src[5] = {0};
        snprintf(src, sizeof(src), "%d", i);

        lscp_tokenizer_result result = lscp_tokenize(src, strlen(src));

        REQUIRE(!result.error);
        REQUIRE(result.count == 1);
        REQUIRE(result.tokens[0].type == LSCP_TK_NUMERIC_LITERAL);
    }
}

TEST_CASE("Tokenize octals", "[tokenizer]")
{
    // Good octals
    for (int i = 0; i < 8; i++)
    {
        char src[5] = {0};
        snprintf(src, sizeof(src), "0%d", i);

        lscp_tokenizer_result result = lscp_tokenize(src, strlen(src));

        REQUIRE(!result.error);
        REQUIRE(result.count == 1);
        REQUIRE(result.tokens[0].type == LSCP_TK_NUMERIC_LITERAL);
    }

    // Bad octals
    lscp_tokenizer_result result = lscp_tokenize("08", strlen("08"));

    REQUIRE(result.error);
}

TEST_CASE("Tokenize hello-world program", "[tokenizer]")
{
    const char* src = "int main(void) {"
                       "    printf(\"hello world\");"
                       "}";

    lscp_token_type tk_types[] = {
        LSCP_TK_INT_KEYWORD, LSCP_TK_IDENTIFIER, LSCP_TK_OPEN_PAREN, LSCP_TK_VOID_KEYWORD, LSCP_TK_CLOSE_PAREN, LSCP_TK_OPEN_CURLY,
        LSCP_TK_IDENTIFIER, LSCP_TK_OPEN_PAREN, LSCP_TK_STRING_LITERAL, LSCP_TK_CLOSE_PAREN, LSCP_TK_SEMICOLON,
        LSCP_TK_CLOSE_CURLY,
    };

    int tk_types_count = sizeof(tk_types) / sizeof(lscp_token_type);

    lscp_tokenizer_result result = lscp_tokenize(src, strlen(src));

    REQUIRE(!result.error);
    REQUIRE(result.count == tk_types_count);

    for (int i = 0; i < tk_types_count; i++)
    {
        REQUIRE(result.tokens[i].type == tk_types[i]);
    }
}

TEST_CASE("Tokenize simple program", "[tokenizer]")
{
    const char* src = "int main(void) {"
                       "    float f = 20 + 59;"
                       "}";

    lscp_token_type tk_types[] = {
        LSCP_TK_INT_KEYWORD, LSCP_TK_IDENTIFIER, LSCP_TK_OPEN_PAREN, LSCP_TK_VOID_KEYWORD, LSCP_TK_CLOSE_PAREN, LSCP_TK_OPEN_CURLY,
        LSCP_TK_FLOAT_KEYWORD, LSCP_TK_IDENTIFIER, LSCP_TK_EQUAL, LSCP_TK_NUMERIC_LITERAL, LSCP_TK_PLUS, LSCP_TK_NUMERIC_LITERAL, LSCP_TK_SEMICOLON,
        LSCP_TK_CLOSE_CURLY,
    };

    int tk_types_count = sizeof(tk_types) / sizeof(lscp_token_type);

    lscp_tokenizer_result result = lscp_tokenize(src, strlen(src));

    REQUIRE(!result.error);
    REQUIRE(result.count == tk_types_count);

    for (int i = 0; i < tk_types_count; i++)
    {
        REQUIRE(result.tokens[i].type == tk_types[i]);
    }
}

TEST_CASE("Parse primitive variable decl", "[parsing]")
{
//    const char* typenames[] = { "char", "int", "float", "double", "_Bool", "_Complex" };
//    lscp_token_type types[] = { LSCP_TK_CHAR_KEYWORD, LSCP_TK_INT_KEYWORD, LSCP_TK_FLOAT_KEYWORD, LSCP_TK_DOUBLE_KEYWORD, LSCP_TK_BOOL_KEYWORD, LSCP_TK_COMPLEX_KEYWORD };
//    int count = sizeof(types) / sizeof(lscp_token_type);
//
//    for (int i = 0; i < count; i++)
//    {
//        char src[1024];
//        snprintf(src, sizeof(src), "%s foo;", typenames[i]);
//        int src_len = strlen(src);
//
//        lscp_parse_result result = lscp_parse(src, src_len);
//        lscp_ast_node ast = result.toplevel;
//        lscp_token* tokens = result.tokens;
//
//        REQUIRE(ast.type == LSCP_AST_TRANSLATION_UNIT);
//        REQUIRE(ast.tks_begin == 0);
//        REQUIRE(ast.tks_count == 3);
//        REQUIRE(ast.tu.decls[0].type == LSCP_AST_VARIABLE_DECL);
//        REQUIRE(ast.tu.decls[0].tks_begin == 0);
//        REQUIRE(ast.tu.decls[0].tks_count == 3);
//        REQUIRE(tokens[0].type == types[i]);
//        REQUIRE(tokens[1].type == LSCP_TK_IDENTIFIER);
//        REQUIRE(tokens[2].type == LSCP_TK_SEMICOLON);
//    }
}

TEST_CASE("Don't allow primitives twice in a decl", "[parsing]")
{
//    const char* typenames[] = { "char", "int", "float", "double", "_Bool", "_Complex" };
//    int count = sizeof(typenames) / sizeof(typenames[0]);
//
//    for (int i = 0; i < count; i++)
//    {
//        char src[1024];
//        snprintf(src, sizeof(src), "%s foo;", typenames[i]);
//        int src_len = strlen(src);
//
//        lscp_parse_result result = lscp_parse(src, src_len);
//
//        REQUIRE(!result.valid);
//    }
}

TEST_CASE("Playground", "[parsing]")
{
    const char* source = "int i;";
    lscp_parse_cstr(source);
}