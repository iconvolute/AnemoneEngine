#include "AnemoneRuntime/Platform/Process.hxx"
#include "AnemoneRuntime/Platform/FileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    Process::Process(Internal::NativeProcessHandle handle)
        : _handle{std::move(handle)}
    {
    }

    Process::Process(Process&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
    {
    }

    Process& Process::operator=(Process&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    Process::~Process() = default;

    inline std::expected<void, ErrorCode> CreatePipeForRedirection(
        Internal::NativeFileHandle& hOutParent,
        Internal::NativeFileHandle& hOutChild,
        bool parentWritesToChild)
    {
        hOutParent = {};
        hOutChild = {};

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE hChild = nullptr;
        HANDLE hTemp = nullptr;

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
        HANDLE hParent = nullptr;

        if (!DuplicateHandle(hProcess, hTemp, hProcess, &hParent, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            CloseHandle(hTemp);
            CloseHandle(hChild);
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        CloseHandle(hTemp);

        hOutParent.Attach(hParent);
        hOutChild.Attach(hChild);
        return {};
    }

    std::expected<Process, ErrorCode> Process::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        FileHandle* input,
        FileHandle* output,
        FileHandle* error)
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

        Internal::NativeFileHandle fhReadInput{};
        Internal::NativeFileHandle fhWriteInput{};
        Internal::NativeFileHandle fhReadOutput{};
        Internal::NativeFileHandle fhWriteOutput{};
        Internal::NativeFileHandle fhReadError{};
        Internal::NativeFileHandle fhWriteError{};

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                if (auto rc = CreatePipeForRedirection(fhWriteInput, fhReadInput, true))
                {
                    startup_info.hStdInput = fhReadInput.Value();
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
                    startup_info.hStdOutput = fhWriteOutput.Value();
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
                    startup_info.hStdError = fhWriteError.Value();
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
                (*input) = FileHandle{std::move(fhWriteInput)};
            }

            if (output)
            {
                (*output) = FileHandle{std::move(fhReadOutput)};
            }

            if (error)
            {
                (*error) = FileHandle{std::move(fhReadError)};
            }

            CloseHandle(process_information.hThread);

            return Process{Internal::NativeProcessHandle{process_information.hProcess}};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    std::expected<int32_t, ErrorCode> Process::Wait()
    {
        AE_ASSERT(this->_handle);

        Internal::NativeProcessHandle const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (WaitForSingleObject(handle.Value(), INFINITE) != WAIT_OBJECT_0)
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            DWORD dwExitCode;

            if (!GetExitCodeProcess(handle.Value(), &dwExitCode))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }

    std::expected<int32_t, ErrorCode> Process::TryWait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            DWORD dwExitCode;

            if (!GetExitCodeProcess(this->_handle.Value(), &dwExitCode))
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

    std::expected<void, ErrorCode> Process::Terminate()
    {
        AE_ASSERT(this->_handle);

        Internal::NativeProcessHandle const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (!TerminateProcess(handle.Value(), 0))
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            if (WaitForSingleObject(handle.Value(), INFINITE) == WAIT_FAILED)
            {
                return std::unexpected(ErrorCode::InvalidOperation);
            }

            return {};
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
