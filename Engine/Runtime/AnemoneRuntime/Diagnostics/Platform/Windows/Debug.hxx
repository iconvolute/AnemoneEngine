#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <string_view>
#include <source_location>

namespace Anemone::Diagnostics
{
    class WindowsDebug
    {
    public:
        RUNTIME_API static void ReportErrorWin32(DWORD error, std::source_location const& location);

        RUNTIME_API static void ReportErrorHRESULT(HRESULT hresult, std::source_location const& location);

        RUNTIME_API static Status TranslateErrorCodeHRESULT(HRESULT hr);

        RUNTIME_API static Status TranslateErrorCodeWin32(DWORD error);        
    };
}

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY_WIN32(error) \
    do \
    { \
        Anemone::Diagnostics::WindowsDebug::ReportErrorWin32(error, std::source_location::current()); \
    } while (false)

#define AE_VERIFY_HRESULT(error) \
    do \
    { \
        Anemone::Diagnostics::WindowsDebug::ReportErrorHRESULT(error, std::source_location::current()); \
    } while (false)

#else

#define AE_VERIFY_WIN32(error)

#define AE_VERIFY_HRESULT(error)

#endif
