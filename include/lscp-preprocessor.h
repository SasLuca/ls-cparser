#ifndef LSCP_PREPROCESSOR_H
#define LSCP_PREPROCESSOR_H

#include "lscp-common.h"
#include "lscp-allocator.h"

typedef struct lscp_preprocessor_result
{
    char* result;
    int result_size;
    lscp_bool valid;
} lscp_preprocessor_result;

lscp_api lscp_str lscp_remove_comments_to_buffer(const char* source_code, lscp_int source_code_size, char* dst, lscp_int dst_size);
lscp_api lscp_str lscp_remove_comments(const char* source_code, lscp_int source_code_size, lscp_allocator allocator);

lscp_api lscp_preprocessor_result lscp_preprocess_code(const char* source_code, lscp_int source_code_size, lscp_allocator allocator);

#endif // LSCP_PREPROCESSOR_H