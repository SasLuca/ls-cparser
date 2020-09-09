#include "lscp-allocator.h"
#include "string.h"
#include "malloc.h"

lscp_thread_local lscp_allocator lscp__global_allocator;

lscp_api void* lscp_libc_allocator(struct lscp_allocator* this_allocator, lscp_source_location source_location, lscp_allocator_mode mode, void* ptr, lscp_int new_size)
{
    void* result = 0;

    switch (mode)
    {
        case lscp_am_alloc:
            result = malloc(new_size);
            break;

        case lscp_am_free:
            free(ptr);
            break;

        case lscp_am_realloc:
            result = realloc(ptr, new_size);
            break;

        default: break;
    }

    return result;
}