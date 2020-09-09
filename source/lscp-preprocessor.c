#include "lscp-common-internal.h"
#include "lscp-internal-utils.h"
#include "lscp-preprocessor.h"
#include "stb_ds.h"

lscp_api lscp_str lscp_remove_comments_to_buffer(const char* source_code, lscp_int source_code_size, char* dst, lscp_int dst_size)
{
    if (dst_size < source_code_size) return (lscp_str){0};
    lscp_str result = { .chars = dst };
    lscp_str source = { source_code, source_code_size };

    int dst_it = 0;
    for (int code_it = 0; code_it < source_code_size; code_it++)
    {
        lscp_str next2 = str_subn(str_from(source, code_it), 2);

        if (starts_with_zstr(next2, "//"))
        {
            code_it += 2;
            for (;source_code[code_it] != '\n' && code_it < source_code_size; code_it++);
            if (source_code[code_it - 1] == '\n') dst[dst_it++] = '\n';
        }
        else if (starts_with_zstr(next2, "/*"))
        {
            code_it += 2;
            for (;code_it < source_code_size; code_it++)
            {
                next2 = str_subn(str_from(source, code_it), 2);
                if (starts_with_zstr(next2, "*/"))
                {
                    code_it += 1;
                    break;
                }
            }
        }
        else
        {
            dst[dst_it++] = source_code[code_it];
        }
    }

    dst[dst_it] = 0;
    result.len = dst_it;

    return result;
}

lscp_api lscp_str lscp_remove_comments(const char* source_code, lscp_int source_code_size, lscp_allocator allocator)
{
    if (!source_code) return (lscp_str) {0};
    char* dst = lscp_alloc(allocator, source_code_size);
    lscp_str result = {0};

    if (dst)
    {
        result = lscp_remove_comments_to_buffer(source_code, source_code_size, dst, source_code_size);
    }

    return result;
}

lscp_internal void consume_line(lscp_str* src, lscp_str* dst)
{
    int i = 0;
    for (; i < src->len && src->chars[i] != '\n'; i++);
    if (src->chars[i] == '\n') i++;
    memcpy(dst->chars, src->chars, i);
    src->chars += i;
    src->len -= i;
    dst->chars[i] = '\n';
    dst->chars += i;
    dst->len += i;
}

lscp_internal void skip_line(lscp_str* src)
{
    int i = 0;
    for (; i < src->len && src->chars[i] != '\n'; i++);
    if (src->chars[i] == '\n') i++;
    src->chars += i;
    src->len -= i;
}

lscp_internal void advance_str(lscp_str* str, int by)
{
    str->chars += by;
    str->len -= by;
}

lscp_internal int find_def(lscp_str* strs, lscp_str match)
{
    for (int i = 0; i < arrlen(strs); i++)
    {
        if (str_match(strs[i], match)) return i;
    }

    return lscp_bad_index;
}

lscp_internal lscp_str get_next_word(lscp_str str)
{
    for (int i = 0; i < str.len; i++)
        if (is_whitespace_nn(str.chars[i]))
        {
            advance_str(&str, i);
            break;
        }

    for (int i = 0; i < str.len; i++)
        if (!is_whitespace_nn(str.chars[i]))
        {
            advance_str(&str, i);
            break;
        }

    for (int i = 0; i < str.len; i++)
        if (is_whitespace(str.chars[i]))
        {
            str.len = i;
            return str;
        }

    return (lscp_str){0};
}

lscp_api lscp_preprocessor_result lscp_preprocess_code(const char* source_code, lscp_int source_code_size, lscp_allocator allocator)
{
    if (!source_code || source_code_size < 0) return (lscp_preprocessor_result) {0};

    lscp_preprocessor_result result = {0};
    lscp_str src = { source_code, source_code_size };
    char* dst_buf = lscp_alloc(allocator, source_code_size);
    lscp_str dst = { dst_buf, 0 };
    lscp_str* defines = 0;
    result.result = dst_buf;

    while (src.len > 0)
    {
        if (starts_with_zstr(src, "#define "))
        {
            lscp_str arg = get_next_word(src);
            arrput(defines, arg);
            skip_line(&src);
        }
        else if (starts_with_zstr(src, "#undef "))
        {
            lscp_str arg = get_next_word(src);
            int f = find_def(defines, arg);
            if (f != lscp_bad_index)
                arrdel(defines, f);
            skip_line(&src);
        }
        else if (starts_with_zstr(src, "#endif"))
        {
            skip_line(&src);
        }
        else if (starts_with_zstr(src, "#ifndef ") || starts_with_zstr(src, "#ifdef "))
        {
            lscp_str arg = get_next_word(src);
            int f = find_def(defines, arg);
            if ((f != lscp_bad_index && starts_with_zstr(src, "#ifndef ")) || (f == lscp_bad_index && starts_with_zstr(src, "#ifdef ")))
            {
                int ct = 0;
                while(1)
                {
                    skip_line(&src);
                    if (starts_with_zstr(src, "#ifndef") || starts_with_zstr(src, "#ifdef"))
                    {
                        ct++;
                    }
                    else if (starts_with_zstr(src, "#endif"))
                    {
                        if (ct == 0)
                        {
                            skip_line(&src);
                            break;
                        }
                        else ct--;
                    }
                }
            }
            else skip_line(&src);
        }
        else
        {
            consume_line(&src, &dst);
        }
    }

    result.result_size = dst.len;
    result.valid = 1;
    return result;
}