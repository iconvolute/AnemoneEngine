#include "AnemoneRuntime/Platform/Linux/LinuxProcess.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxFileHandle.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <array>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <spawn.h>
#include <link.h>

namespace Anemone
{
    LinuxProcess::LinuxProcess(pid_t handle)
        : _handle{handle}
    {
    }

    LinuxProcess::LinuxProcess(LinuxProcess&& other) noexcept
        : _handle{std::exchange(other._handle, 0)}
    {
    }

    LinuxProcess& LinuxProcess::operator=(LinuxProcess&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    LinuxProcess::~LinuxProcess()
    {
    }

    std::expected<LinuxProcess, ErrorCode> LinuxProcess::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        LinuxFileHandle* input,
        LinuxFileHandle* output,
        LinuxFileHandle* error)
    {
#if !ANEMONE_PLATFORM_LINUX
        (void)path;
        (void)params;
        (void)workingDirectory;
        (void)input;
        (void)output;
        (void)error;
        return {};
        return std::unexpected(ErrorCode::NotImplemented);
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
            return std::unexpected(ErrorCode::InvalidArgument);
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
        FileHandle fhWriteInput{};
        FileHandle fhReadInput{};
        FileHandle fhWriteOutput{};
        FileHandle fhReadOutput{};
        FileHandle fhWriteError{};
        FileHandle fhReadError{};

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
                    return std::unexpected(ErrorCode::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteInput = FileHandle{fd[1]};
                fhReadInput = FileHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[0], STDIN_FILENO);
            }

            if (redirectOutput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(ErrorCode::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteOutput = FileHandle{fd[1]};
                fhReadOutput = FileHandle{fd[0]};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDOUT_FILENO);
            }

            if (redirectError)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(ErrorCode::InvalidOperation);
                }

                // Create file handle for parent process.
                fhWriteError = FileHandle{fd[1]};
                fhReadError = FileHandle{fd[0]};

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

            return LinuxProcess{process_id};
        }
        else
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }
#endif
    }

    std::expected<int32_t, ErrorCode> LinuxProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
    {
        auto sc = Start(path, params, workingDirectory, nullptr, nullptr, nullptr).and_then([](LinuxProcess process)
        {
            return process.Wait();
        });
        return sc;
    }

    std::expected<int32_t, ErrorCode> LinuxProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        std::string& output,
        std::string& error)
    {
        // TODO: This code is common to all supported platforms.
        LinuxFileHandle pipeOutput{};
        LinuxFileHandle pipeError{};

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

    std::expected<int32_t, ErrorCode> LinuxProcess::Wait()
    {
        AE_ASSERT(this->_handle > 0);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Wait not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        auto& error = errno;

        int status{};
        int rc{};

        do
        {
            rc = waitpid(this->_handle, &status, 0);
        } while ((rc < 0) and (error == EINTR));

        if (rc != this->_handle)
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return WTERMSIG(status) + 256;
#endif
    }

    std::expected<int32_t, ErrorCode> LinuxProcess::TryWait()
    {
        AE_ASSERT(this->_handle > 0);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        auto& error = errno;

        int status;
        int rc;

        do
        {
            rc = waitpid(this->_handle, &status, WNOHANG);
        } while ((rc < 0) and (error == EINTR));

        if (rc == 0)
        {
            // Child process still running.
            return std::unexpected(ErrorCode::OperationInProgress);
        }

        if (rc != this->_handle)
        {
            // Failed to wait for child process.
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        // Child process has exited.
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return WTERMSIG(status) + 256;
#endif
    }

    std::expected<void, ErrorCode> LinuxProcess::Terminate()
    {
        AE_ASSERT(this->_handle > 0);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Terminate not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        if (kill(this->_handle, SIGKILL) != 0)
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        return {};
#endif
    }
}
