#ifndef LSCP_ALLOCATOR_H
#define LSCP_ALLOCATOR_H

#include "lscp-common.h"

#define lscp_default_allocator (lscp_literal(lscp_allocator) { NULL, lscp_libc_allocator })

#define lscp_alloc(allocator, size)            ((allocator).allocator_proc(&(allocator), lscp_current_source_location, lscp_am_alloc, 0, (size)))
#define lscp_free(allocator, ptr)              ((allocator).allocator_proc(&(allocator), lscp_current_source_location, lscp_am_free, (ptr), 0))
#define lscp_realloc(allocator, ptr, new_size) ((allocator).allocator_proc(&(allocator), lscp_current_source_location, lscp_am_realloc, (ptr), (new_size)))

typedef enum lscp_allocator_mode
{
    lscp_am_unknown = 0,
    lscp_am_alloc,
    lscp_am_realloc,
    lscp_am_free,
} lscp_allocator_mode;

struct lscp_allocator;

typedef void* (lscp_allocator_proc)(struct lscp_allocator* this_allocator, lscp_source_location source_location, lscp_allocator_mode mode, void* ptr, lscp_int new_size);

typedef struct lscp_allocator
{
    void* user_data;
    lscp_allocator_proc* allocator_proc;
} lscp_allocator;

extern lscp_thread_local lscp_allocator lscp__global_allocator;

lscp_api void* lscp_libc_allocator(struct lscp_allocator* this_allocator, lscp_source_location source_location, lscp_allocator_mode mode, void* ptr, lscp_int new_size);

#endif // LSCP_ALLOCATOR_H