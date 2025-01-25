#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

namespace Anemone::System
{
    Process::Process(Interop::NativeProcess native)
        : m_native{native}
    {
    }

    Process::Process()
        : m_native{}
    {
    }

    Process::Process(Process&& other) noexcept
        : m_native{std::exchange(other.m_native, {})}
    {
    }

    Process& Process::operator=(Process&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->m_native.Handle)
            {
                CloseHandle(this->m_native.Handle);
            }

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    Process::~Process()
    {
        if (this->m_native.Handle)
        {
            CloseHandle(this->m_native.Handle);
            this->m_native.Handle = nullptr;
        }
    }

    static std::expected<void, ErrorCode> CreatePipeForRedirection(FileHandle& parentHandle, FileHandle& childHandle, bool parentInputs)
    {
        Interop::NativeFileHandle nativeParent{INVALID_HANDLE_VALUE};
        Interop::NativeFileHandle nativeChild{INVALID_HANDLE_VALUE};

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE hTmp;

        if (parentInputs)
        {
            CreatePipe(&nativeChild.Handle, &hTmp, &sa, 0);
        }
        else
        {
            CreatePipe(&hTmp, &nativeChild.Handle, &sa, 0);
        }

        if (not DuplicateHandle(GetCurrentProcess(), hTmp, GetCurrentProcess(), &nativeParent.Handle, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            DWORD dwError = GetLastError();
            CloseHandle(hTmp);

            parentHandle = {};
            childHandle = {};

            return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
        }

        CloseHandle(hTmp);

        parentHandle = FileHandle{nativeParent};
        childHandle = FileHandle{nativeChild};
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

        FileHandle fhReadInput{};
        FileHandle fhWriteInput{};
        FileHandle fhReadOutput{};
        FileHandle fhWriteOutput{};
        FileHandle fhReadError{};
        FileHandle fhWriteError{};

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                if (auto rc = CreatePipeForRedirection(fhWriteInput, fhReadInput, true))
                {
                    startup_info.hStdInput = fhReadInput.GetNative().Handle;
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
                    startup_info.hStdOutput = fhWriteOutput.GetNative().Handle;
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
                    startup_info.hStdError = fhWriteError.GetNative().Handle;
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

            startup_info.dwFlags = STARTF_USESTDHANDLES;
        }

        PROCESS_INFORMATION process_information{};

        std::wstring wPath{};
        std::wstring wParams{};
        std::wstring wWorkingDirectory{};

        Interop::win32_WidenString(wPath, path);

        if (params)
        {
            Interop::win32_WidenString(wParams, params.value());
            wParams.insert(0, L" ");
            wParams.insert(0, wPath);
        }

        if (workingDirectory)
        {
            Interop::win32_WidenString(wWorkingDirectory, workingDirectory.value());
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

        DWORD dwError = GetLastError();

        if (bCreated)
        {
            if (input != nullptr)
            {
                *input = std::move(fhWriteInput);
            }

            if (output != nullptr)
            {
                *output = std::move(fhReadOutput);
            }

            if (error != nullptr)
            {
                *error = std::move(fhReadError);
            }

            CloseHandle(process_information.hThread);

            return Process{Interop::NativeProcess{
                .Handle = process_information.hProcess,
                .Id = process_information.dwProcessId,
                .Owned = true,
            }};
        }

        return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
    }

    std::expected<int32_t, ErrorCode> Process::Wait() const
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        if (WaitForSingleObject(this->m_native.Handle, INFINITE) != WAIT_OBJECT_0)
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        DWORD dwExitCode;

        if (GetExitCodeProcess(this->m_native.Handle, &dwExitCode) == FALSE)
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return dwExitCode;
    }

    std::expected<std::optional<int32_t>, ErrorCode> Process::TryWait() const
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        DWORD dwExitCode;

        if (GetExitCodeProcess(this->m_native.Handle, &dwExitCode) == FALSE)
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        if (dwExitCode != STILL_ACTIVE)
        {
            return dwExitCode;
        }

        return std::nullopt;
    }

    std::expected<void, ErrorCode> Process::Terminate()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        DWORD dwError = 0;

        if (not TerminateProcess(this->m_native.Handle, 0))
        {
            dwError = GetLastError();

            WaitForSingleObject(this->m_native.Handle, INFINITE);
            CloseHandle(this->m_native.Handle);
            this->m_native = {};
        }

        if (dwError != 0)
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
        }

        return {};
    }
}
