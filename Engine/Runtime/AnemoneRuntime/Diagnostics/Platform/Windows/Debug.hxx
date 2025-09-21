#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <string_view>
#include <source_location>

namespace Anemone::Diagnostics
{
    class WindowsDebug
    {
    public:
        RUNTIME_API static void ReportErrorWin32(DWORD error, std::source_location const& location);

        RUNTIME_API static void ReportErrorHRESULT(HRESULT hresult, std::source_location const& location);

        RUNTIME_API static Error TranslateErrorCodeHRESULT(HRESULT hr);

        RUNTIME_API static Error TranslateErrorCodeWin32(DWORD error);        
    };
}

#define AE_ENSURE_WIN32(error) \
    do \
    { \
        Anemone::Diagnostics::WindowsDebug::ReportErrorWin32(error, std::source_location::current()); \
    } while (false)

#define AE_ENSURE_HRESULT(error) \
    do \
    { \
        Anemone::Diagnostics::WindowsDebug::ReportErrorHRESULT(error, std::source_location::current()); \
    } while (false)


#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY_WIN32(error) AE_ENSURE_WIN32(error)

#define AE_VERIFY_HRESULT(error) AE_ENSURE_HRESULT(error)

#else

#define AE_VERIFY_WIN32(error)

#define AE_VERIFY_HRESULT(error)

#endif
