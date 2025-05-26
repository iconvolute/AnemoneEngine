#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

#include <array>

namespace Anemone::Internal
{
    inline std::expected<void, Status> CreatePipeForRedirection(
        Interop::Windows::SafeFileHandle& hOutParent,
        Interop::Windows::SafeFileHandle& hOutChild,
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
            return std::unexpected(Status::InvalidOperation);
        }

        HANDLE hChild = parentWritesToChild ? hRead : hWrite;
        HANDLE hParent = parentWritesToChild ? hWrite : hRead;

        HANDLE hProcess = GetCurrentProcess();
        HANDLE hParentDuplicate = nullptr;

        if (!DuplicateHandle(hProcess, hParent, hProcess, &hParentDuplicate, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            CloseHandle(hParent);
            CloseHandle(hChild);
            return std::unexpected(Status::InvalidOperation);
        }

        CloseHandle(hParent);

        hOutParent.Attach(hParentDuplicate);
        hOutChild.Attach(hChild);
        return {};
    }
}

namespace Anemone
{
    std::expected<int32_t, Status> Process::Wait()
    {
        AE_ASSERT(this->_handle);

        if (Interop::Windows::SafeHandle const handle = std::exchange(this->_handle, {}))
        {
            if (WaitForSingleObject(handle.Get(), INFINITE) != WAIT_OBJECT_0)
            {
                return std::unexpected(Status::InvalidOperation);
            }

            DWORD dwExitCode;

            if (not GetExitCodeProcess(handle.Get(), &dwExitCode))
            {
                return std::unexpected(Status::InvalidOperation);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(Status::InvalidHandle);
    }

    std::expected<int32_t, Status> Process::TryWait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            DWORD dwExitCode;

            if (not GetExitCodeProcess(this->_handle.Get(), &dwExitCode))
            {
                return std::unexpected(Status::InvalidOperation);
            }

            if (dwExitCode == STILL_ACTIVE)
            {
                return std::unexpected(Status::OperationInProgress);
            }

            this->_handle = {};
            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(Status::InvalidHandle);
    }

    std::expected<void, Status> Process::Terminate()
    {
        AE_ASSERT(this->_handle);

        if (Interop::Windows::SafeHandle const handle = std::exchange(this->_handle, {}))
        {
            if (!TerminateProcess(handle.Get(), 0))
            {
                return std::unexpected(Status::InvalidOperation);
            }

            if (WaitForSingleObject(handle.Get(), INFINITE) == WAIT_FAILED)
            {
                return std::unexpected(Status::InvalidOperation);
            }

            return {};
        }

        return std::unexpected(Status::InvalidHandle);
    }

    auto Process::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
        -> std::expected<Process, Status>
    {
        return Process::Start(
            path,
            params,
            workingDirectory,
            nullptr,
            nullptr,
            nullptr);
    }

    auto Process::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        FileHandle* input,
        FileHandle* output,
        FileHandle* error)
        -> std::expected<Process, Status>
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

        Interop::Windows::SafeFileHandle fhReadInput{};
        Interop::Windows::SafeFileHandle fhWriteInput{};
        Interop::Windows::SafeFileHandle fhReadOutput{};
        Interop::Windows::SafeFileHandle fhWriteOutput{};
        Interop::Windows::SafeFileHandle fhReadError{};
        Interop::Windows::SafeFileHandle fhWriteError{};

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                if (auto rc = Internal::CreatePipeForRedirection(fhWriteInput, fhReadInput, true))
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
                if (auto rc = Internal::CreatePipeForRedirection(fhReadOutput, fhWriteOutput, false))
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
                if (auto rc = Internal::CreatePipeForRedirection(fhReadError, fhWriteError, false))
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
        Interop::Windows::WidenString(wPath, path);

        std::wstring wParams{};

        if (params)
        {
            Interop::Windows::WidenString(wParams, *params);
            wParams.insert(0, 1, L' ');
            wParams.insert(0, wPath);
        }

        std::wstring wWorkingDirectory{};

        if (workingDirectory)
        {
            Interop::Windows::WidenString(wWorkingDirectory, *workingDirectory);
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

            return Process{Interop::Windows::SafeHandle{process_information.hProcess}};
        }

        return std::unexpected(Status::InvalidOperation);
    }

    auto Process::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
        -> std::expected<int32_t, Status>
    {
        return Process::Start(path, params, workingDirectory, nullptr, nullptr, nullptr)
            .and_then([](Process process)
        {
            return process.Wait();
        });
    }

    auto Process::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        FunctionRef<void(std::string_view)> output,
        FunctionRef<void(std::string_view)> error)
        -> std::expected<int32_t, Status>
    {
        FileHandle pipeOutput{};
        FileHandle pipeError{};

        if (auto process = Process::Start(path, params, workingDirectory, nullptr, &pipeOutput, &pipeError))
        {
            std::array<char, 512> buffer;
            std::span view = std::as_writable_bytes(std::span{buffer});

            while (true)
            {
                auto rc = process->TryWait();

                while (auto processed = pipeOutput.Read(view))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    output(std::string_view{buffer.data(), *processed});
                }

                while (auto processed = pipeError.Read(view))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    error(std::string_view{buffer.data(), *processed});
                }

                if (rc)
                {
                    return *rc;
                }

                if (rc.error() != Status::OperationInProgress)
                {
                    return std::unexpected(rc.error());
                }
            }
        }
        else
        {
            return std::unexpected(process.error());
        }
    }
}
