#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <string_view>
#include <source_location>

namespace Anemone
{
    class WindowsDebug final
    {
    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();

        RUNTIME_API static void ReportErrorWin32(DWORD error, std::source_location const& location);

        RUNTIME_API static void ReportErrorHRESULT(HRESULT hresult, std::source_location const& location);

        RUNTIME_API static Error TranslateErrorCodeHRESULT(HRESULT hr);

        RUNTIME_API static Error TranslateErrorCodeWin32(DWORD error);

        WindowsDebug();
        ~WindowsDebug();

    private:
        static void CrashHandler(LPEXCEPTION_POINTERS pExceptionPointers);
        static LONG CALLBACK UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers);
        static LONG CALLBACK UnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers);
        static void __cdecl PureCallHandler();
        static void __cdecl InvalidParameterHandler(
            wchar_t const*,
            wchar_t const*,
            wchar_t const*,
            unsigned int,
            uintptr_t);
        static void __CRTDECL TerminateHandler();

    private:
        PTOP_LEVEL_EXCEPTION_FILTER topLevelExceptionFilter{};
        _purecall_handler pureCallHandler{};
        _invalid_parameter_handler invalidParameterHandler{};
        std::terminate_handler terminateHandler{};
    };

    using PlatformDebug = WindowsDebug;
}

#define AE_ENSURE_WIN32(error) \
    do \
    { \
        Anemone::WindowsDebug::ReportErrorWin32(error, std::source_location::current()); \
    } while (false)

#define AE_ENSURE_HRESULT(error) \
    do \
    { \
        Anemone::WindowsDebug::ReportErrorHRESULT(error, std::source_location::current()); \
    } while (false)


#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY_WIN32(error) AE_ENSURE_WIN32(error)

#define AE_VERIFY_HRESULT(error) AE_ENSURE_HRESULT(error)

#else

#define AE_VERIFY_WIN32(error)

#define AE_VERIFY_HRESULT(error)

#endif
