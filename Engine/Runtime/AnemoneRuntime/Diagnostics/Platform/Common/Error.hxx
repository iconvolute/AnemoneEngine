#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <string_view>
#include <source_location>

namespace Anemone::Internal
{
    RUNTIME_API void ReportErrorErrno(int error, std::source_location const& location);

    RUNTIME_API Status TranslateErrorCodeErrno(int error);
}

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY_ERRNO(error) \
    do \
    { \
        Anemone::Internal::ReportErrorErrno(error, std::source_location::current()); \
    } while (false)

#else

#define AE_VERIFY_ERRNO(error)

#endif
