#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <string_view>
#include <source_location>

namespace Anemone::Internal
{
    RUNTIME_API void ReportErrorWin32(DWORD error, std::source_location const& location);

    RUNTIME_API void ReportErrorHRESULT(HRESULT hresult, std::source_location const& location);

    RUNTIME_API Status TranslateErrorCodeHRESULT(HRESULT hr);

    RUNTIME_API Status TranslateErrorCodeWin32(DWORD error);
}

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY_WIN32(error) \
    do \
    { \
        Anemone::Internal::ReportErrorWin32(error, std::source_location::current()); \
    } while (false)

#define AE_VERIFY_HRESULT(error) \
    do \
    { \
        Anemone::Internal::ReportErrorHRESULT(error, std::source_location::current()); \
    } while (false)

#else

#define AE_VERIFY_WIN32(error)

#define AE_VERIFY_HRESULT(error)

#endif
