#include "AnemoneRuntime/System/Platform/Windows/WindowsProcess.hxx"
#include "AnemoneRuntime/System/CommandLine.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Threading.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Storage/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Storage/FileOutputStream.hxx"
#include "AnemoneRuntime/Storage/FileInputStream.hxx"

namespace Anemone
{
    namespace
    {
        bool CreatePipeForRedirection(
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
                return false;
            }

            HANDLE hChild = parentWritesToChild ? hRead : hWrite;
            HANDLE hParent = parentWritesToChild ? hWrite : hRead;

            HANDLE hProcess = GetCurrentProcess();
            HANDLE hParentDuplicate = nullptr;

            if (!DuplicateHandle(hProcess, hParent, hProcess, &hParentDuplicate, 0, FALSE, DUPLICATE_SAME_ACCESS))
            {
                CloseHandle(hParent);
                CloseHandle(hChild);
                return false;
            }

            CloseHandle(hParent);

            hOutParent.Attach(hParentDuplicate);
            hOutChild.Attach(hChild);
            return true;
        }
    }
}

namespace Anemone
{
    WindowsProcess::WindowsProcess() = default;

    WindowsProcess::WindowsProcess(
        Interop::Windows::SafeHandle handle,
        Reference<StreamReader> standardOutputReader,
        Reference<StreamReader> standardErrorReader,
        Reference<StreamWriter> standardInputWriter)
        : _handle{std::move(handle)}
    {
        this->_standardOutputReader = std::move(standardOutputReader);
        this->_standardErrorReader = std::move(standardErrorReader);
        this->_standardInputWriter = std::move(standardInputWriter);
    }

    WindowsProcess::~WindowsProcess() = default;

    std::expected<int32_t, Error> WindowsProcess::Wait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            if (WaitForSingleObject(this->_handle.Get(), INFINITE) != WAIT_OBJECT_0)
            {
                return std::unexpected(Error::Failure);
            }

            DWORD dwExitCode;

            if (not GetExitCodeProcess(this->_handle.Get(), &dwExitCode))
            {
                return std::unexpected(Error::Failure);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(Error::InvalidHandle);
    }

    std::expected<int32_t, Error> WindowsProcess::TryWait(
        Duration timeout)
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            DWORD dwStatus = WaitForSingleObject(
                this->_handle.Get(),
                Interop::Windows::ValidateTimeoutDuration(timeout));

            if (dwStatus == WAIT_TIMEOUT)
            {
                return std::unexpected(Error::Timeout);
            }

            if (dwStatus == WAIT_ABANDONED)
            {
                return std::unexpected(Error::Abandoned);
            }

            if (dwStatus != WAIT_OBJECT_0)
            {
                return std::unexpected(Error::Failure);
            }

            DWORD dwExitCode;

            if (not GetExitCodeProcess(this->_handle.Get(), &dwExitCode))
            {
                return std::unexpected(Error::Failure);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(Error::InvalidHandle);
    }

    std::expected<int32_t, Error> WindowsProcess::TryWait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            DWORD dwExitCode;

            if (not GetExitCodeProcess(this->_handle.Get(), &dwExitCode))
            {
                return std::unexpected(Error::Failure);
            }

            if (dwExitCode == STILL_ACTIVE)
            {
                return std::unexpected(Error::Timeout);
            }

            return static_cast<int32_t>(dwExitCode);
        }

        return std::unexpected(Error::InvalidHandle);
    }

    Reference<Process> WindowsProcess::Start(
        ProcessStartInfo const& processStartInfo)
    {
        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

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

        Interop::Windows::SafeFileHandle fhReadInput{};
        Interop::Windows::SafeFileHandle fhWriteInput{};
        Interop::Windows::SafeFileHandle fhReadOutput{};
        Interop::Windows::SafeFileHandle fhWriteOutput{};
        Interop::Windows::SafeFileHandle fhReadError{};
        Interop::Windows::SafeFileHandle fhWriteError{};

        if (processStartInfo.RedirectStandardInput or processStartInfo.RedirectStandardOutput or processStartInfo.RedirectStandardError)
        {
            if (processStartInfo.RedirectStandardInput)
            {
                if (CreatePipeForRedirection(fhWriteInput, fhReadInput, true))
                {
                    si.hStdInput = fhReadInput.Get();
                }
                else
                {
                    return {};
                }
            }
            else
            {
                si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            }

            if (processStartInfo.RedirectStandardOutput)
            {
                if (CreatePipeForRedirection(fhReadOutput, fhWriteOutput, false))
                {
                    si.hStdOutput = fhWriteOutput.Get();
                }
                else
                {
                    return {};
                }
            }
            else
            {
                si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            }

            if (processStartInfo.RedirectStandardError)
            {
                if (CreatePipeForRedirection(fhReadError, fhWriteError, false))
                {
                    si.hStdError = fhWriteError.Get();
                }
                else
                {
                    return {};
                }
            }
            else
            {
                si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            }

            si.dwFlags |= STARTF_USESTDHANDLES;
        }

        PROCESS_INFORMATION process_information{};

        Interop::Windows::FilePathW wPath{};
        Interop::Windows::WidenString(wPath, processStartInfo.FileName);

        Interop::Windows::FilePathW wParams{};

        if (not processStartInfo.Parameters.empty())
        {
            std::string args = CommandLine::Combine(processStartInfo.FileName, processStartInfo.Parameters);
            Interop::Windows::WidenString(wParams, args);
        }

        Interop::Windows::FilePathW wStartupDirectory{};

        if (processStartInfo.StartupDirectory)
        {
            Interop::Windows::WidenString(wStartupDirectory, *processStartInfo.StartupDirectory);
        }

        BOOL bCreated = CreateProcessW(

            processStartInfo.Parameters.empty() ? wPath.c_str() : nullptr,
            processStartInfo.Parameters.empty() ? nullptr : wParams.data(),
            &sa,
            &sa,
            TRUE,
            CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT | NORMAL_PRIORITY_CLASS,
            nullptr,
            processStartInfo.StartupDirectory ? wStartupDirectory.data() : nullptr,
            &si,
            &process_information);

        if (bCreated)
        {
            CloseHandle(process_information.hThread);

            Reference<StreamReader> standardOutputReader{};
            Reference<StreamReader> standardErrorReader{};
            Reference<StreamWriter> standardInputWriter{};

            constexpr size_t BufferCapacity = 4096;

            if (processStartInfo.RedirectStandardInput)
            {
                standardInputWriter = MakeReference<StreamWriter>(
                    MakeReference<FileOutputStream>(
                        MakeReference<WindowsFileHandle>(std::move(fhWriteInput)),
                        BufferCapacity),
                    BufferCapacity);

                standardInputWriter->SetAutoFlush(true);
            }

            if (processStartInfo.RedirectStandardOutput)
            {
                standardOutputReader = MakeReference<StreamReader>(
                    MakeReference<FileInputStream>(
                        MakeReference<WindowsFileHandle>(std::move(fhReadOutput)),
                        BufferCapacity),
                    BufferCapacity);
            }

            if (processStartInfo.RedirectStandardError)
            {
                standardErrorReader = MakeReference<StreamReader>(
                    MakeReference<FileInputStream>(
                        MakeReference<WindowsFileHandle>(std::move(fhReadError)),
                        BufferCapacity),
                    BufferCapacity);
            }

            return MakeReference<WindowsProcess>(
                Interop::Windows::SafeHandle{process_information.hProcess},
                std::move(standardOutputReader),
                std::move(standardErrorReader),
                std::move(standardInputWriter));
        }

        return {};
    }
}

namespace Anemone
{
    Reference<Process> Process::Start(
        ProcessStartInfo const& processStartInfo)
    {
        return WindowsProcess::Start(processStartInfo);
    }
}
