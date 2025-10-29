#pragma once
#include "AnemoneDiagnostics/Debug.hxx"

#include <cstring>
#include <cerrno>

#if ANEMONE_FEATURE_ASSERTIONS

// Handle Unix function call that returns 0 on success and returns errno_t on failure.
#define AE_UNIX_HANDLE_RESULT(expression) \
    do \
    { \
        if (int const rc = (expression); rc != 0) \
        { \
            AE_PANIC("'" #expression "' failed (rc: {}, '{}')", rc, std::strerror(rc)); \
        } \
    } while (false)

// Handle Unix function call that returns -1 on failure and sets errno.
#define AE_UNIX_HANDLE_ERRNO(expression) \
    do \
    { \
        if ((expression) != 0) \
        { \
            AE_PANIC("'" #expression "' failed ('{}')", std::strerror(errno)); \
        } \
    } while (false)

#else

#define AE_UNIX_HANDLE_RESULT(expression) \
    do \
    { \
        (void)(expression); \
    } while (false)

#define AE_UNIX_HANDLE_ERRNO(expression) \
    do \
    { \
        (void)(expression); \
    } while (false)

#endif
