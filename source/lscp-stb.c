#include "lscp-common-internal.h"
#include "lscp-allocator.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context,ptr,size) lscp_realloc(lscp__global_allocator, ptr, size)
#define STBDS_FREE(context,ptr)         lscp_free(lscp__global_allocator, ptr)
#include "stb_ds.h"