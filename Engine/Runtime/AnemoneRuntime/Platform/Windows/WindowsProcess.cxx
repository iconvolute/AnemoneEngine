#include "AnemoneRuntime/Platform/Windows/WindowsProcess.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    inline std::expected<void, ErrorCode> CreatePipeForRedirection(
        Interop::Win32SafeHandle& hOutParent,
        Interop::Win32SafeHandle& hOutChild,
        bool parentWritesToChild)
    {
        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE hRead = nullptr;
        HANDLE hWrite = nullptr;

        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        HANDLE hChild = parentWritesToChild ? hRead : hWrite;
        HANDLE hParent = parentWritesToChild ? hWrite : hRead;

        HANDLE hProcess = GetCurrentProcess();
        HANDLE hParentDuplicate = nullptr;

        if (!DuplicateHandle(hProcess, hParent, hProcess, &hParentDuplicate, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            CloseHandle(hParent);
            CloseHandle(hChild);
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        CloseHandle(hParent);

        hOutParent.Attach(hParentDuplicate);
        hOutChild.Attach(hChild);
        return {};
    }

    std::expected<WindowsProcess, ErrorCode> WindowsProcess::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        WindowsFileHandle* input,
        WindowsFileHandle* output,
        WindowsFileHandle* error)
    {
        bool const redirectInput = input != nullptr;
        bool const redirectOutput = output != nullptr;
        bool const redirectError = error != nullptr;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        STARTUPINFOW startup_info{
            .cb = sizeof(startup_info),
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

        Interop::Win32SafeHandle fhReadInput{};
        Interop::Win32SafeHandle fhWriteInput{};
        Interop::Win32SafeHandle fhReadOutput{};
        Interop::Win32SafeHandle fhWriteOutput{};
        Interop::Win32SafeHandle fhReadError{};
        Interop::Win32SafeHandle fhWriteError{};

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                if (auto rc = CreatePipeForRedirection(fhWriteInput, fhReadInput, true))
                {
                    startup_info.hStdInput = fhReadInput.Get();
                }
                else
                {
                    return std::unexpected(rc.error());
                }
            }
            else
            {
                startup_info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            }

            if (redirectOutput)
            {
                if (auto rc = CreatePipeForRedirection(fhReadOutput, fhWriteOutput, false))
                {
                    startup_info.hStdOutput = fhWriteOutput.Get();
                }
                else
                {
                    return std::unexpected(rc.error());
                }
            }
            else
            {
                startup_info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            }

            if (redirectError)
            {
                if (auto rc = CreatePipeForRedirection(fhReadError, fhWriteError, false))
                {
                    startup_info.hStdError = fhWriteError.Get();
                }
                else
                {
                    return std::unexpected(rc.error());
                }
            }
            else
            {
                startup_info.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            }

            startup_info.dwFlags |= STARTF_USESTDHANDLES;
        }

        PROCESS_INFORMATION process_information{};

        std::wstring wPath{};
        Interop::win32_WidenString(wPath, path);

        std::wstring wParams{};

        if (params)
        {
            Interop::win32_WidenString(wParams, *params);
            wParams.insert(0, 1, L' ');
            wParams.insert(0, wPath);
        }

        std::wstring wWorkingDirectory{};

        if (workingDirectory)
        {
            Interop::win32_WidenString(wWorkingDirectory, *workingDirectory);
        }

        BOOL bCreated = CreateProcessW(
            wPath.c_str(),
            params ? wParams.data() : nullptr,
            &sa,
            &sa,
            TRUE,
            CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT | NORMAL_PRIORITY_CLASS,
            nullptr,
            workingDirectory ? wWorkingDirectory.data() : nullptr,
            &startup_info,
            &process_information);

        if (bCreated)
        {
            if (input)
            {
                (*input) = WindowsFileHandle{std::move(fhWriteInput)};
            }

            if (output)
            {
                (*output) = WindowsFileHandle{std::move(fhReadOutput)};
            }

            if (error)
            {
                (*error) = WindowsFileHandle{std::move(fhReadError)};
            }

            CloseHandle(process_information.hThread);

            return WindowsProcess{Interop::Win32SafeHandle{process_information.hProcess}};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::Wait()
    {
        AE_ASSERT(this->_handle);

        Interop::Win32SafeHandle const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (WaitForSingleObject(handle.Get(), INFINITE) != WAIT_OBJECT_0)
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            DWORD dwExitCode;

            if (!GetExitCodeProcess(handle.Get(), &dwExitCode))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::TryWait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            DWORD dwExitCode;

            if (!GetExitCodeProcess(this->_handle.Get(), &dwExitCode))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            if (dwExitCode == STILL_ACTIVE)
            {
                return std::unexpected(ErrorCode::OperationInProgress);
            }

            this->_handle = {};
            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }

    std::expected<void, ErrorCode> WindowsProcess::Terminate()
    {
        AE_ASSERT(this->_handle);

        Interop::Win32SafeHandle const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (!TerminateProcess(handle.Get(), 0))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            if (WaitForSingleObject(handle.Get(), INFINITE) == WAIT_FAILED)
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            return {};
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
