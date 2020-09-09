#ifndef LSCP_COMMON_INTERNAL
#define LSCP_COMMON_INTERNAL

#define lscp_internal_implementation_flag
#include "lscp-common.h"

#define lscp_bad_index (-1)
#define lscp_str_lit_len(str_lit) (sizeof(str_lit) - 1)
#define lscp_str_from_lit(str_lit) ((lscp_str) { str_lit, lscp_str_lit_len(str_lit) })
#define lscp_str_zstr(zstr) ((lscp_str) { zstr, (lscp_int) strlen(zstr) })

#endif  // LSCP_COMMON_INTERNAL