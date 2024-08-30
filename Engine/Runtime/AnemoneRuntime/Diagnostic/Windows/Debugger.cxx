#include "AnemoneRuntime/Diagnostic/Debugger.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Diagnostic
{
    void Abort(std::string_view message)
    {
        (void)message;
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    bool IsDebuggerAttached()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#else
        return ::IsDebuggerPresent() != FALSE;
#endif
    }

    void WaitForDebugger()
    {
        while (not IsDebuggerPresent())
        {
            Sleep(100);
        }

        DebugBreak();
    }

    bool AttachDebugger()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#elif ANEMONE_FEATURE_JITDEBUGGER
        if (::IsDebuggerPresent())
        {
            return false;
        }

        wchar_t commandLine[MAX_PATH + 1];
        swprintf_s(commandLine, L"vsjitdebugger.exe -p %lu", GetCurrentProcessId());

        STARTUPINFOW si{
            .cb = sizeof(si),
            .lpReserved = nullptr,
            .lpDesktop = nullptr,
            .lpTitle = nullptr,
            .dwX = static_cast<DWORD>(CW_USEDEFAULT),
            .dwY = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwYSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXCountChars = 0,
            .dwYCountChars = 0,
            .dwFillAttribute = 0,
            .dwFlags = 0,
            .wShowWindow = 0,
            .cbReserved2 = 0,
            .lpReserved2 = nullptr,
            .hStdInput = nullptr,
            .hStdOutput = nullptr,
            .hStdError = nullptr,
        };
        PROCESS_INFORMATION pi{};

        if (CreateProcessW(
        nullptr,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);

            return true;
        }

        return false;
#else
        return false;
#endif
    }
}
