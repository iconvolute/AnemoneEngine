#include "AnemoneRuntime/Platform/Process.hxx"
#include "AnemoneRuntime/Platform/FileHandle.hxx"
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
    Process::Process(Internal::NativeProcessHandle handle)
        : _handle{handle}
    {
    }

    Process::Process(Process&& other) noexcept
        : _handle{std::exchange(other._handle, Internal::NativeProcessHandle::Invalid())}
    {
    }

    Process& Process::operator=(Process&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->_handle = std::exchange(other._handle, Internal::NativeProcessHandle::Invalid());
        }

        return *this;
    }

    Process::~Process()
    {
    }

    std::expected<Process, ErrorCode> Process::Start(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        FileHandle* input,
        FileHandle* output,
        FileHandle* error)
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
                fhWriteInput = FileHandle{ Internal::NativeFileHandle {fd[1]}};
                fhReadInput = FileHandle{Internal::NativeFileHandle{fd[0]}};

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
                fhWriteOutput = FileHandle{Internal::NativeFileHandle{fd[1]}};
                fhReadOutput = FileHandle{Internal::NativeFileHandle{fd[0]}};

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
                fhWriteError = FileHandle{Internal::NativeFileHandle{fd[1]}};
                fhReadError = FileHandle{Internal::NativeFileHandle{fd[0]}};

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

            return Process{Internal::NativeProcessHandle{process_id}};
        }
        else
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }
#endif
    }

    std::expected<int32_t, ErrorCode> Process::Wait()
    {
        AE_ASSERT(this->_handle.IsValid());

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Wait not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        auto& error = errno;

        int status{};
        int rc{};

        do
        {
            rc = waitpid(this->_handle.Value, &status, 0);
        } while ((rc < 0) and (error == EINTR));

        if (rc != this->_handle.Value)
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        this->_handle = Internal::NativeProcessHandle::Invalid();

        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return WTERMSIG(status) + 256;
#endif
    }

    std::expected<int32_t, ErrorCode> Process::TryWait()
    {
        AE_ASSERT(this->_handle.IsValid());

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        int status;
        int rc = Interop::posix_WaitForProcess(this->_handle.Value, status, WNOHANG);

        if (rc == 0)
        {
            // Child process still running.
            return std::unexpected(ErrorCode::OperationInProgress);
        }

        if (rc != this->_handle.Value)
        {
            // Failed to wait for child process.
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        // Clear handle.
        this->_handle = Internal::NativeProcessHandle::Invalid();

        // Child process has exited.
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return WTERMSIG(status) + 256;
#endif
    }

    std::expected<int32_t, ErrorCode> Process::TryWait(Duration timeout)
    {
        AE_ASSERT(this->_handle.IsValid());

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else
        pid_t rc;
        int status;

        // Try to get child process status.

        rc = Interop::posix_WaitForProcess(this->_handle.Value, status, WNOHANG);

        if (rc == 0)
        {
            // Child process still running. Try wait with timeout.
            useconds_t const sleepTime = static_cast<useconds_t>(timeout.ToMicroseconds());

            usleep(sleepTime);

            Interop::posix_WaitForProcess(this->_handle.Value, status, WNOHANG);
        }

        if (rc == 0)
        {
            // Child process is still running.
            return std::unexpected(ErrorCode::OperationInProgress);
        }
        else if (rc != this->_handle.Value)
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

    std::expected<void, ErrorCode> Process::Terminate()
    {
        AE_ASSERT(this->_handle.IsValid());

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Terminate not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
#else

        Internal::NativeProcessHandle const handle = std::exchange(this->_handle, Internal::NativeProcessHandle::Invalid());

        if (kill(handle.Value, SIGKILL) != 0)
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        return {};
#endif
    }
}
