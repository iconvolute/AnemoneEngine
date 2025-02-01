#include "AnemoneRuntime/Platform/Windows/WindowsProcess.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    WindowsProcess::WindowsProcess(HANDLE handle)
        : _handle{handle}
    {
    }

    WindowsProcess::WindowsProcess(WindowsProcess&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
    {
    }

    WindowsProcess& WindowsProcess::operator=(WindowsProcess&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle)
            {
                if (!CloseHandle(this->_handle))
                {
                    AE_TRACE(Error, "Failed to close process: handle={}, error={}", fmt::ptr(this->_handle), GetLastError());
                }
            }

            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    WindowsProcess::~WindowsProcess()
    {
        if (this->_handle)
        {
            if (!CloseHandle(this->_handle))
            {
                AE_TRACE(Error, "Failed to close process: handle={}, error={}", fmt::ptr(this->_handle), GetLastError());
            }
        }
    }

    static std::expected<void, ErrorCode> CreatePipeForRedirection(
        HANDLE& hOutParent,
        HANDLE& hOutChild,
        bool parentWritesToChild)
    {
        hOutParent = NULL;
        hOutChild = NULL;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE hChild = NULL;
        HANDLE hTemp = NULL;

        if (parentWritesToChild)
        {
            if (!CreatePipe(&hChild, &hTemp, &sa, 0))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }
        }
        else
        {
            if (!CreatePipe(&hTemp, &hChild, &sa, 0))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }
        }

        HANDLE hProcess = GetCurrentProcess();
        HANDLE hParent = NULL;

        if (!DuplicateHandle(hProcess, hTemp, hProcess, &hParent, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            CloseHandle(hTemp);
            CloseHandle(hChild);
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        CloseHandle(hTemp);

        hOutParent = hParent;
        hOutChild = hChild;
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

        Interop::win32_SafeHandle fhReadInput{};
        Interop::win32_SafeHandle fhWriteInput{};
        Interop::win32_SafeHandle fhReadOutput{};
        Interop::win32_SafeHandle fhWriteOutput{};
        Interop::win32_SafeHandle fhReadError{};
        Interop::win32_SafeHandle fhWriteError{};

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                if (auto rc = CreatePipeForRedirection(fhWriteInput.Handle, fhReadInput.Handle, true))
                {
                    startup_info.hStdInput = fhReadInput.Handle;
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
                if (auto rc = CreatePipeForRedirection(fhReadOutput.Handle, fhWriteOutput.Handle, false))
                {
                    startup_info.hStdOutput = fhWriteOutput.Handle;
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
                if (auto rc = CreatePipeForRedirection(fhReadError.Handle, fhWriteError.Handle, false))
                {
                    startup_info.hStdError = fhWriteError.Handle;
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
                (*input) = WindowsFileHandle{fhWriteInput.Detach()};
            }

            if (output)
            {
                (*output) = WindowsFileHandle{fhReadOutput.Detach()};
            }

            if (error)
            {
                (*error) = WindowsFileHandle{fhReadError.Detach()};
            }

            CloseHandle(process_information.hThread);

            return WindowsProcess{process_information.hProcess};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
    {
        auto sc = Start(path, params, workingDirectory, nullptr, nullptr, nullptr).and_then([](WindowsProcess process)
        {
            return process.Wait();
        });
        return sc;
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        std::string& output,
        std::string& error)
    {
        // TODO: This code is common to all supported platforms.
        WindowsFileHandle pipeOutput{};
        WindowsFileHandle pipeError{};

        if (auto process = Start(path, params, workingDirectory, nullptr, &pipeOutput, &pipeError))
        {
            std::array<char, 1024> buffer;

            for (;;)
            {
                auto ret = process->TryWait();

                while (auto processed = pipeOutput.Read(std::as_writable_bytes(std::span{buffer})))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    output.append(reinterpret_cast<char const*>(buffer.data()), *processed);
                }

                while (auto processed = pipeError.Read(std::as_writable_bytes(std::span{buffer})))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    error.append(reinterpret_cast<char const*>(buffer.data()), *processed);
                }

                if (ret)
                {
                    return *ret;
                }

                if (ret.error() != ErrorCode::OperationInProgress)
                {
                    return std::unexpected(ret.error());
                }
            }
        }
        else
        {
            return std::unexpected(process.error());
        }
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::Wait()
    {
        AE_ASSERT(this->_handle);

        if (WaitForSingleObject(this->_handle, INFINITE) != WAIT_OBJECT_0)
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        DWORD dwExitCode;

        if (!GetExitCodeProcess(this->_handle, &dwExitCode))
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        // TODO: Should we close the handle here?

        return static_cast<int32_t>(dwExitCode);
    }

    std::expected<int32_t, ErrorCode> WindowsProcess::TryWait()
    {
        AE_ASSERT(this->_handle);

        DWORD dwExitCode;

        if (!GetExitCodeProcess(this->_handle, &dwExitCode))
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        if (dwExitCode == STILL_ACTIVE)
        {
            return std::unexpected(ErrorCode::OperationInProgress);
        }

        // TODO: Should we close the handle here?

        return static_cast<int32_t>(dwExitCode);
    }

    std::expected<void, ErrorCode> WindowsProcess::Terminate()
    {
        AE_ASSERT(this->_handle);

        if (HANDLE const handle = std::exchange(this->_handle, nullptr))
        {
            if (!TerminateProcess(handle, 0))
            {
                CloseHandle(handle);
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            if (WaitForSingleObject(handle, INFINITE) == WAIT_FAILED)
            {
                CloseHandle(handle);
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            CloseHandle(handle);
        }

        return {};
    }
}
