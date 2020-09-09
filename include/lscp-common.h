#ifndef LSCP_COMMON_H
#define LSCP_COMMON_H

#ifndef lscp_api
    #ifdef __cplusplus
        #define lscp_api extern "C"
    #else
        #define lscp_api extern
    #endif
#endif

#ifndef lscp_internal
    #define lscp_internal static inline
#endif

// Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define lscp_literal(type) type
#else
    #define lscp_literal(type) (type)
#endif

#ifndef lscp_thread_local
    #if __cplusplus >= 201103L
        #define lscp_thread_local thread_local
    #elif __STDC_VERSION_ >= 201112L
        #define lscp_thread_local _Thread_local
    #elif defined(__GNUC__) || defined(__clang__)
        #define lscp_thread_local __thread
    #elif defined(_MSC_VER)
        #define lscp_thread_local __declspec(thread)
    #endif
#endif

typedef signed long long lscp_int;
typedef int lscp_bool;

#define lscp_current_source_location (lscp_literal(lscp_source_location) { __FILE__, __FUNCTION__, __LINE__ })

typedef struct lscp_source_location
{
    const char* file_name;
    const char* proc_name;
    lscp_int line_in_file;
} lscp_source_location;

typedef enum lscp_error
{
    LSCP_NO_ERROR = 0,
} lscp_error;

typedef struct lscp_str
{
    char* chars;
    lscp_int len;
} lscp_str;

#endif // LSCP_COMMON_H