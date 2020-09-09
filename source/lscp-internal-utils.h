#ifndef LSCP_INTERNAL_UTILS
#define LSCP_INTERNAL_UTILS

#include "lscp-common-internal.h"
#include "lscp-tokenizer.h"
#include "string.h"

lscp_internal lscp_bool is_digit(char it)
{
    return it >= '0' && it <= '9';
}

lscp_internal lscp_bool is_whitespace(char it)
{
    return it == ' ' || it == '\r' || it == '\n' || it == '\t';
}

lscp_internal lscp_bool is_whitespace_nn(char it)
{
    return it == ' ' || it == '\r' || it == '\t';
}

lscp_internal lscp_bool is_symbol(char it)
{
    const char* symbols = LSCP_C_SYMBOLS;
    for (int i = 0; i < LSCP_C_SYMBOLS_COUNT; i++) if (it == symbols[i]) return 1;
    return 0;
}

lscp_internal lscp_bool is_letter(char it)
{
    return (it >= 'a' && it <= 'z') || (it >= 'A' && it <= 'Z');
}

lscp_internal lscp_int zstring_len(const char* zstr)
{
    const char* iter = zstr;
    while (*iter++) {}
    return (lscp_int) (iter - zstr);
}

lscp_internal lscp_str str_from(lscp_str a, lscp_int n)
{
    if (a.len - n <= 0) return (lscp_str) {0};

    return (lscp_str) { a.chars + n, a.len - n };
}

lscp_internal lscp_str str_eat_char(lscp_str a, char c)
{
    lscp_int it = 0;
    for (; it < a.len && a.chars[it] == c; it++);

    return (lscp_str) { a.chars + it, a.len - it };
}

lscp_internal lscp_str str_eat_whitespace(lscp_str a)
{
    lscp_int it = 0;
    for (; it < a.len && (a.chars[it] == ' ' || a.chars[it] == '\r' || a.chars[it] == '\t'); it++);

    return (lscp_str) { a.chars + it, a.len - it };
}

lscp_internal lscp_str str_subn(lscp_str a, lscp_int len)
{
    if (a.len < len) return (lscp_str) {0};
    return (lscp_str) { a.chars, len };
}

lscp_internal lscp_bool str_match(lscp_str a, lscp_str b)
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

lscp_internal lscp_bool str_match_lit(lscp_str a, const char* b)
{
    return str_match(a, (lscp_str) { b, (lscp_int) strlen(b) });
}

lscp_internal lscp_bool starts_with(lscp_str str, lscp_str match)
{
    if (str.len < match.len) return 0;
    for (int i = 0; i < match.len; i++) if (str.chars[i] != match.chars[i]) return 0;
    return 1;
}

lscp_internal lscp_bool starts_with_zstr(lscp_str str, const char* match)
{
    return starts_with(str, lscp_str_zstr(match));
}

lscp_internal lscp_bool ends_with(lscp_str str, lscp_str match)
{
    if (str.len <= match.len) return 0;
    for (int i = 0; i < match.len; i++) if (str.chars[str.len - 1 - i] != match.chars[i]) return 0;
    return 1;
}

lscp_internal lscp_bool ends_with_zstr(lscp_str str, const char* match) { return ends_with(str, lscp_str_zstr(match)); }

lscp_internal lscp_token_type get_token_type_if_keyword(lscp_str str)
{
    if (str_match(str, lscp_str_from_lit("auto")))           return LSCP_TK_AUTO_KEYWORD;
    if (str_match(str, lscp_str_from_lit("break")))          return LSCP_TK_BREAK_KEYWORD;
    if (str_match(str, lscp_str_from_lit("case")))           return LSCP_TK_CASE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("char")))           return LSCP_TK_CHAR_KEYWORD;
    if (str_match(str, lscp_str_from_lit("const")))          return LSCP_TK_CONST_KEYWORD;
    if (str_match(str, lscp_str_from_lit("continue")))       return LSCP_TK_CONTINUE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("default")))        return LSCP_TK_DEFAULT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("do")))             return LSCP_TK_DO_KEYWORD;
    if (str_match(str, lscp_str_from_lit("double")))         return LSCP_TK_DOUBLE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("else")))           return LSCP_TK_ELSE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("enum")))           return LSCP_TK_ENUM_KEYWORD;
    if (str_match(str, lscp_str_from_lit("extern")))         return LSCP_TK_EXTERN_KEYWORD;
    if (str_match(str, lscp_str_from_lit("float")))          return LSCP_TK_FLOAT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("for")))            return LSCP_TK_FOR_KEYWORD;
    if (str_match(str, lscp_str_from_lit("goto")))           return LSCP_TK_GOTO_KEYWORD;
    if (str_match(str, lscp_str_from_lit("if")))             return LSCP_TK_IF_KEYWORD;
    if (str_match(str, lscp_str_from_lit("inline")))         return LSCP_TK_INLINE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("int")))            return LSCP_TK_INT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("long")))           return LSCP_TK_LONG_KEYWORD;
    if (str_match(str, lscp_str_from_lit("register")))       return LSCP_TK_REGISTER_KEYWORD;
    if (str_match(str, lscp_str_from_lit("restrict")))       return LSCP_TK_RESTRICT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("return")))         return LSCP_TK_RETURN_KEYWORD;
    if (str_match(str, lscp_str_from_lit("short")))          return LSCP_TK_SHORT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("signed")))         return LSCP_TK_SIGNED_KEYWORD;
    if (str_match(str, lscp_str_from_lit("sizeof")))         return LSCP_TK_SIZEOF_KEYWORD;
    if (str_match(str, lscp_str_from_lit("static")))         return LSCP_TK_STATIC_KEYWORD;
    if (str_match(str, lscp_str_from_lit("struct")))         return LSCP_TK_STRUCT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("switch")))         return LSCP_TK_SWITCH_KEYWORD;
    if (str_match(str, lscp_str_from_lit("typedef")))        return LSCP_TK_TYPEDEF_KEYWORD;
    if (str_match(str, lscp_str_from_lit("union")))          return LSCP_TK_UNION_KEYWORD;
    if (str_match(str, lscp_str_from_lit("unsigned")))       return LSCP_TK_UNSIGNED_KEYWORD;
    if (str_match(str, lscp_str_from_lit("void")))           return LSCP_TK_VOID_KEYWORD;
    if (str_match(str, lscp_str_from_lit("volatile")))       return LSCP_TK_VOLATILE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("while")))          return LSCP_TK_WHILE_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Alignas")))       return LSCP_TK_ALIGNAS_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Alignof")))       return LSCP_TK_ALIGNOF_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Atomic")))        return LSCP_TK_ATOMIC_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Bool")))          return LSCP_TK_BOOL_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Complex")))       return LSCP_TK_COMPLEX_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Generic")))       return LSCP_TK_GENERIC_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Imaginary")))     return LSCP_TK_IMAGINARY_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Noreturn")))      return LSCP_TK_NORETURN_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Static_assert"))) return LSCP_TK_STATIC_ASSERT_KEYWORD;
    if (str_match(str, lscp_str_from_lit("_Thread_local")))  return LSCP_TK_THREAD_LOCAL_KEYWORD;

    return LSCP_TK_UNKNOWN;
}

#endif // LSCP_INTERNAL_UTILS