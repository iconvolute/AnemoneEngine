#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"

#include <iterator>
#include <array>
#include <exception>

namespace Anemone::Diagnostic::Debug
{
    AssertAction PlatformSpecificAssertionHandler(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message)
    {
#if ANEMONE_PLATFORM_WINDOWS
        constexpr std::array<TASKDIALOG_BUTTON, 3> buttons{
            {
                {
                    .nButtonID = static_cast<int>(AssertAction::Abort),
                    .pszButtonText = L"&Abort",
                },
                {
                    .nButtonID = static_cast<int>(AssertAction::Debug),
                    .pszButtonText = L"&Debug",
                },
                {
                    .nButtonID = static_cast<int>(AssertAction::Continue),
                    .pszButtonText = L"&Continue",
                },
            }};

        fmt::memory_buffer buffer{};
        auto out = std::back_inserter(buffer);
        out = fmt::format_to(out, "location: {}:{}\n\n", location.file_name(), location.line());
        out = fmt::format_to(out, "expression: {}\n\n", expression);
        out = fmt::format_to(out, "function: {}\n\n", location.function_name());
        out = fmt::format_to(out, "message: {}", message);
        std::string_view viewBuffer{buffer.data(), buffer.size()};

        using namespace Anemone::Platform;
        win32_string_buffer<wchar_t, 512> wContent{};
        win32_WidenString(wContent, viewBuffer);

        TASKDIALOGCONFIG tgc{};
        tgc.cbSize = sizeof(tgc);
        tgc.hInstance = GetModuleHandleW(nullptr);
        tgc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
        tgc.pszWindowTitle = L"Assertion Failed";
        tgc.pszMainIcon = TD_ERROR_ICON;
        tgc.pszMainInstruction = L"Assertion Failed";
        tgc.pszContent = wContent;
        tgc.cButtons = static_cast<UINT>(buttons.size());
        tgc.pButtons = buttons.data();
        tgc.nDefaultButton = static_cast<int>(AssertAction::Debug);

        INT result{};
        if (FAILED(TaskDialogIndirect(&tgc, &result, nullptr, nullptr)))
        {
            return AssertAction::Abort;
        }

        return static_cast<AssertAction>(result);
#else
        return AssertAction::Abort;
#endif
    }

    void PlatformSpecificPanicHandler()
    {
#if ANEMONE_PLATFORM_WINDOWS
        TASKDIALOGCONFIG tgc{};
        tgc.cbSize = sizeof(TASKDIALOGCONFIG);
        tgc.hwndParent = nullptr;
        tgc.hInstance = GetModuleHandleW(nullptr);
        tgc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
        tgc.pszWindowTitle = L"Critical Error";
        tgc.pszMainIcon = TD_ERROR_ICON;
        tgc.pszMainInstruction = L"Critical Error Occured";
        tgc.pszContent = L"Application will stop. Please provide crash dump and logs";
        tgc.dwCommonButtons = TDCBF_OK_BUTTON;

        TaskDialogIndirect(&tgc, nullptr, nullptr, nullptr);
#endif
    }

    void Break()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
        Debug::WaitForDebugger();

        // Break into debugger. This will also generate a crash if no debugger is attached.
        anemone_debugbreak();

        // But even so, if we're still here, terminate the process.
        std::terminate();
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
        while (not IsDebuggerAttached())
        {
            Threading::SleepThread(Duration::FromMilliseconds(100));
        }

        anemone_debugbreak();
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
