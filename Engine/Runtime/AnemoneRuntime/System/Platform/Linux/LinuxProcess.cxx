#include "AnemoneRuntime/System/Platform/Linux/LinuxProcess.hxx"
#include "AnemoneRuntime/Storage/Platform/Linux/LinuxFileHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"
#include "AnemoneRuntime/Storage/FileOutputStream.hxx"
#include "AnemoneRuntime/Storage/FileInputStream.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include "AnemoneRuntime/Interop/Linux/Process.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Interop/Linux/Threading.hxx"

#include <array>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <spawn.h>
#include <link.h>
#include <poll.h>


namespace Anemone
{
    Reference<Process> Process::Start(
        ProcessStartInfo const& processStartInfo)
    {
        return LinuxProcess::Start(processStartInfo);
    }
}

namespace Anemone
{
    LinuxProcess::LinuxProcess(
        Interop::Linux::SafePidFdHandle handle,
        Reference<StreamReader> standardOutputReader,
        Reference<StreamReader> standardErrorReader,
        Reference<StreamWriter> standardInputWriter)
        : _handle{std::move(handle)}
    {
        this->_standardOutputReader = std::move(standardOutputReader);
        this->_standardErrorReader = std::move(standardErrorReader);
        this->_standardInputWriter = std::move(standardInputWriter);
    }

    std::expected<int32_t, Error> LinuxProcess::Wait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            siginfo_t si;
            int rc;

            do
            {
                rc = waitid(P_PIDFD, this->_handle.Get(), &si, WEXITED);
            } while ((rc < 0) and (errno == EINTR));

            if (rc < 0)
            {
                return std::unexpected(Error::Failure);
            }

            if (si.si_code == CLD_EXITED)
            {
                return si.si_status;
            }
            else
            {
                return si.si_status + 128;
            }
        }

        return std::unexpected(Error::InvalidHandle);
    }

    std::expected<int32_t, Error> LinuxProcess::TryWait(
        Duration timeout)
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            // Convert timeout to native API.
            timespec tsTimeout = Interop::Linux::ToTimespec(timeout);

            // Time when waiting started.
            timespec tsStart{};
            clock_gettime(CLOCK_MONOTONIC, &tsStart);

            // Time what elapsed since waiting started.
            timespec tsElapsed{};

            while (true)
            {
                if (Interop::Linux::CompareGreaterEqual(tsElapsed, tsTimeout))
                {
                    // tsElapsed >= tsTimeout
                    return std::unexpected(Error::Timeout);
                }

                // Remaining time to wait.
                timespec tsRemaining = Interop::Linux::TimespecDifference(tsTimeout, tsElapsed);

                // Prepare pollfd
                struct pollfd pfd;
                pfd.fd = this->_handle.Get();
                pfd.events = POLLIN;
                pfd.revents = 0;

                int rc = ppoll(&pfd, 1, &tsRemaining, nullptr);

                if (rc > 0)
                {
                    // Process has exited
                    siginfo_t si;
                    int rcWaitid;

                    do
                    {
                        rcWaitid = waitid(P_PIDFD, this->_handle.Get(), &si, WEXITED);
                    } while ((rcWaitid < 0) and (errno == EINTR));

                    if (rcWaitid < 0)
                    {
                        return std::unexpected(Error::Failure);
                    }

                    if (si.si_signo == 0)
                    {
                        // Child process still running.
                        return std::unexpected(Error::Timeout);
                    }

                    if (si.si_code == CLD_EXITED)
                    {
                        return si.si_status;
                    }
                    else
                    {
                        return si.si_status + 128;
                    }
                }
                else if (rc == 0)
                {
                    // Timed out while waiting for process
                    return std::unexpected(Error::Timeout);
                }
                else
                {
                    if (errno == EINTR)
                    {
                        // Syscall interrupted. Recompute new timeout.
                        timespec tsCurrent{};
                        clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

                        tsElapsed = Interop::Linux::TimespecDifference(tsCurrent, tsStart);
                        continue;
                    }

                    // Other kind of error
                    return std::unexpected(Error::Failure);
                }
            }
        }

        return std::unexpected(Error::InvalidHandle);
    }

    std::expected<int32_t, Error> LinuxProcess::TryWait()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            siginfo_t si;
            int rc;

            do
            {
                rc = waitid(P_PIDFD, this->_handle.Get(), &si, WEXITED | WNOHANG);
            } while ((rc < 0) and (errno == EINTR));

            if (rc < 0)
            {
                return std::unexpected(Error::Failure);
            }

            if (si.si_signo == 0)
            {
                // Child process still running.
                return std::unexpected(Error::Timeout);
            }

            if (si.si_code == CLD_EXITED)
            {
                return si.si_status;
            }
            else
            {
                return si.si_status + 128;
            }
        }

        return std::unexpected(Error::InvalidHandle);
    }

    Reference<Process> LinuxProcess::Start(
        ProcessStartInfo const& processStartInfo)
    {
        // need a copy because spawn function requires writable string
        std::string fullPath{processStartInfo.FileName};

        // Convert span of string views to char**

        std::vector<std::string> fullParamsStorage{};
        fullParamsStorage.reserve(processStartInfo.Parameters.size());

        for (auto const& param : processStartInfo.Parameters)
        {
            fullParamsStorage.emplace_back(param);
        }

        // Do not interleave these two phases.
        // Some platforms implement std::string with SBO, invalidating char*.
        std::vector<char*> fullParams{};
        fullParams.reserve(processStartInfo.Parameters.size() + 2uz);

        // argv of child process expects path to executable.
        fullParams.emplace_back(fullPath.data());

        for (auto& param : fullParamsStorage)
        {
            fullParams.emplace_back(param.data());
        }

        // and nullptr at end of the array.
        fullParams.emplace_back(nullptr);

        posix_spawnattr_t spawn_attributes{};
        posix_spawnattr_init(&spawn_attributes);

        short spawn_flags{};

        sigset_t empty_sigset{};
        sigemptyset(&empty_sigset);
        posix_spawnattr_setsigmask(&spawn_attributes, &empty_sigset);

        spawn_flags |= POSIX_SPAWN_SETSIGMASK;

        sigset_t default_sigset{};
        sigemptyset(&default_sigset);

        for (int i = SIGRTMIN; i < SIGRTMAX; ++i)
        {
            sigaddset(&default_sigset, i);
        }

        posix_spawnattr_setsigdefault(&spawn_attributes, &default_sigset);

        spawn_flags |= POSIX_SPAWN_SETSIGDEF;

        // TODO: Determine if we can do this in normal FileHandle::CreatePipe() function.
        Interop::Linux::SafeFdHandle fhWriteInput{};
        Interop::Linux::SafeFdHandle fhReadInput{};
        Interop::Linux::SafeFdHandle fhWriteOutput{};
        Interop::Linux::SafeFdHandle fhReadOutput{};
        Interop::Linux::SafeFdHandle fhWriteError{};
        Interop::Linux::SafeFdHandle fhReadError{};

        posix_spawn_file_actions_t files{};
        posix_spawn_file_actions_init(&files);

        if (processStartInfo.StartupDirectory)
        {
            posix_spawn_file_actions_addchdir_np(&files, processStartInfo.StartupDirectory->c_str());
        }

        if (processStartInfo.RedirectStandardInput or processStartInfo.RedirectStandardOutput or processStartInfo.RedirectStandardError)
        {
            if (processStartInfo.RedirectStandardInput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return {};
                }

                // Create file handle for parent process.
                fhWriteInput = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadInput = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[0], STDIN_FILENO);
            }

            if (processStartInfo.RedirectStandardOutput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return {};
                }

                // Create file handle for parent process.
                fhWriteOutput = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadOutput = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDOUT_FILENO);
            }

            if (processStartInfo.RedirectStandardError)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return {};
                }

                // Create file handle for parent process.
                fhWriteError = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadError = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDERR_FILENO);
            }
        }

        Interop::Linux::SafePidFdHandle handle{};

        int spawn_result = pidfd_spawnp(
            handle.GetAddressOf(),
            processStartInfo.FileName.c_str(),
            &files,
            &spawn_attributes,
            fullParams.data(),
            environ);

        posix_spawn_file_actions_destroy(&files);

        if (spawn_result == 0)
        {
            Reference<StreamReader> standardOutputReader{};
            Reference<StreamReader> standardErrorReader{};
            Reference<StreamWriter> standardInputWriter{};

            constexpr size_t BufferCapacity = 4096;

            if (processStartInfo.RedirectStandardInput)
            {
                standardInputWriter = MakeReference<StreamWriter>(
                    MakeReference<FileOutputStream>(
                        MakeReference<LinuxFileHandle>(std::move(fhWriteInput)),
                        BufferCapacity),
                    BufferCapacity);

                standardInputWriter->SetAutoFlush(true);
            }

            if (processStartInfo.RedirectStandardOutput)
            {
                standardOutputReader = MakeReference<StreamReader>(
                    MakeReference<FileInputStream>(
                        MakeReference<LinuxFileHandle>(std::move(fhReadOutput)),
                        BufferCapacity),
                    BufferCapacity);
            }

            if (processStartInfo.RedirectStandardError)
            {
                standardErrorReader = MakeReference<StreamReader>(
                    MakeReference<FileInputStream>(
                        MakeReference<LinuxFileHandle>(std::move(fhReadError)),
                        BufferCapacity),
                    BufferCapacity);
            }

            return MakeReference<LinuxProcess>(
                std::move(handle),
                std::move(standardOutputReader),
                std::move(standardErrorReader),
                std::move(standardInputWriter));
        }

        return {};
    }
}

#if false
#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Interop/Linux/Process.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Storage/Platform/Linux/FileHandle.hxx"

#include <array>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <spawn.h>
#include <link.h>

namespace Anemone
{
    std::expected<int32_t, Error> Process::Wait()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Wait not implemented");
        return std::unexpected(Error::NotImplemented);
#else

        Internal::PlatformProcess const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            int status;
            int rc = Interop::Linux::WaitForProcess(handle.Get(), status, 0);

            if (rc != handle.Get())
            {
                return std::unexpected(Error::InvalidOperation);
            }

            if (WIFEXITED(status))
            {
                return WEXITSTATUS(status);
            }

            return WTERMSIG(status) + 256;
        }

        return std::unexpected(Error::InvalidHandle);

#endif
    }

    std::expected<int32_t, Error> Process::TryWait()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return std::unexpected(Error::NotImplemented);
#else

        if (this->_handle)
        {
            int status;
            int rc = Interop::Linux::WaitForProcess(this->_handle.Get(), status, WNOHANG);

            if (rc == 0)
            {
                // Child process still running.
                return std::unexpected(Error::OperationInProgress);
            }

            if (rc != this->_handle.Get())
            {
                this->_handle = {};

                // Failed to wait for child process.
                return std::unexpected(Error::InvalidOperation);
            }

            // Clear handle.
            this->_handle = {};

            // Child process has exited.
            if (WIFEXITED(status))
            {
                return WEXITSTATUS(status);
            }

            return WTERMSIG(status) + 256;
        }

        return std::unexpected(Error::InvalidHandle);
#endif
    }

    std::expected<void, Error> Process::Terminate()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Terminate not implemented");
        return std::unexpected(Error::NotImplemented);
#else

        Internal::PlatformProcess const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (Interop::Linux::TerminateProcess(handle.Get()))
            {
                return {};
            }

            return std::unexpected(Error::InvalidOperation);
        }

        return std::unexpected(Error::InvalidHandle);
#endif
    }

    auto Process::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
        -> std::expected<Process, Error>
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
        std::unique_ptr<FileHandle>* input,
        std::unique_ptr<FileHandle>* output,
        std::unique_ptr<FileHandle>* error)
        -> std::expected<Process, Error>
    {
#if !ANEMONE_PLATFORM_LINUX
        (void)path;
        (void)params;
        (void)workingDirectory;
        (void)input;
        (void)output;
        (void)error;
        return {};
        return std::unexpected(Error::NotImplemented);
#else

        bool const redirectInput = input != nullptr;
        bool const redirectOutput = output != nullptr;
        bool const redirectError = error != nullptr;

        std::string fullPath{path};

        std::string fullParams = fmt::format("\"{}\"", fullPath);

        if (params)
        {
            fullParams += " ";
            fullParams += *params;
        }

        wordexp_t wordexpStatus{};

        int exp_result = wordexp(fullParams.c_str(), &wordexpStatus, 0);

        if (exp_result != 0)
        {
            return std::unexpected(Error::InvalidArgument);
        }

        pid_t process_id{};

        posix_spawnattr_t spawn_attributes{};
        posix_spawnattr_init(&spawn_attributes);

        short spawn_flags{};

        sigset_t empty_sigset{};
        sigemptyset(&empty_sigset);
        posix_spawnattr_setsigmask(&spawn_attributes, &empty_sigset);

        spawn_flags |= POSIX_SPAWN_SETSIGMASK;

        sigset_t default_sigset{};
        sigemptyset(&default_sigset);

        for (int i = SIGRTMIN; i < SIGRTMAX; ++i)
        {
            sigaddset(&default_sigset, i);
        }

        posix_spawnattr_setsigdefault(&spawn_attributes, &default_sigset);

        spawn_flags |= POSIX_SPAWN_SETSIGDEF;

        // TODO: Determine if we can do this in normal FileHandle::CreatePipe() function.
        Interop::Linux::SafeFdHandle fhWriteInput{};
        Interop::Linux::SafeFdHandle fhReadInput{};
        Interop::Linux::SafeFdHandle fhWriteOutput{};
        Interop::Linux::SafeFdHandle fhReadOutput{};
        Interop::Linux::SafeFdHandle fhWriteError{};
        Interop::Linux::SafeFdHandle fhReadError{};

        posix_spawn_file_actions_t files{};
        posix_spawn_file_actions_init(&files);

        if (workingDirectory)
        {
            posix_spawn_file_actions_addchdir_np(&files, std::string{*workingDirectory}.c_str());
        }

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Error::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteInput = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadInput = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[0], STDIN_FILENO);
            }

            if (redirectOutput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Error::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteOutput = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadOutput = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDOUT_FILENO);
            }

            if (redirectError)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Error::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteError = Interop::Linux::SafeFdHandle{fd[1]};
                fhReadError = Interop::Linux::SafeFdHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDERR_FILENO);
            }
        }

        int spawn_result = posix_spawn(
            &process_id,
            fullPath.c_str(),
            &files,
            &spawn_attributes,
            wordexpStatus.we_wordv,
            environ);

        posix_spawn_file_actions_destroy(&files);

        wordfree(&wordexpStatus);

        if (spawn_result == 0)
        {
            if (input != nullptr)
            {
                *input = std::make_unique<LinuxFileHandle>(std::move(fhWriteInput));
            }

            if (output != nullptr)
            {
                *output = std::make_unique<LinuxFileHandle>(std::move(fhReadOutput));
            }

            if (error != nullptr)
            {
                *error = std::make_unique<LinuxFileHandle>(std::move(fhReadError));
            }

            return Process{Internal::PlatformProcess{process_id}};
        }
        else
        {
            return std::unexpected(Error::InvalidOperation);
        }
#endif
    }

    auto Process::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
        -> std::expected<int32_t, Error>
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
        -> std::expected<int32_t, Error>
    {
        std::unique_ptr<FileHandle> pipeOutput{};
        std::unique_ptr<FileHandle> pipeError{};

        if (auto process = Process::Start(path, params, workingDirectory, nullptr, &pipeOutput, &pipeError))
        {
            std::array<char, 512> buffer;
            std::span view = std::as_writable_bytes(std::span{buffer});

            while (true)
            {
                auto rc = process->TryWait();

                while (size_t processed = pipeOutput->Read(view))
                {
                    output(std::string_view{buffer.data(), processed});
                }

                while (size_t processed = pipeError->Read(view))
                {
                    error(std::string_view{buffer.data(), processed});
                }

                if (rc)
                {
                    return *rc;
                }

                if (rc.error() != Error::OperationInProgress)
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
#endif
