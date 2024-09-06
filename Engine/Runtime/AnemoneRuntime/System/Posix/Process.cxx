#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#if ANEMONE_PLATFORM_LINUX

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <spawn.h>
#include <link.h>

ANEMONE_EXTERNAL_HEADERS_END

#endif

namespace Anemone::System
{
    Process::Process(Platform::NativeProcess native)
    {
        Platform::Create(this->_native, native);
    }

    Process::Process()
    {
        Platform::Create(this->_native);
    }

    Process::Process(Process&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    Process& Process::operator=(Process&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            Platform::NativeProcess& nativeThis = Platform::Get(this->_native);
            Platform::NativeProcess& nativeOther = Platform::Get(other._native);

            nativeThis = std::exchange(nativeOther, {});
        }

        return *this;
    }

    Process::~Process()
    {
        Platform::Destroy(this->_native);
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
        return std::unexpected(ErrorCode::InvalidArgument);
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
        FileHandle fhReadOutput{};
        FileHandle fhReadError{};

        posix_spawn_file_actions_t files{};
        posix_spawn_file_actions_init(&files);

        if (redirectInput or redirectOutput or redirectError)
        {
            if (redirectInput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Private::ErrorCodeFromErrno(errno));
                }

                // Create file handle for parent process.
                fhWriteInput = FileHandle{Platform::NativeFileHandle{fd[1]}};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[0], STDIN_FILENO);
            }

            if (redirectOutput)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Private::ErrorCodeFromErrno(errno));
                }

                // Create file handle for parent process.
                fhReadOutput = FileHandle{Platform::NativeFileHandle{fd[0]}};

                // Prepare file actions for child process.
                posix_spawn_file_actions_adddup2(&files, fd[1], STDOUT_FILENO);
            }

            if (redirectError)
            {
                int fd[2];

                if (pipe2(fd, O_CLOEXEC | O_NONBLOCK) != 0)
                {
                    return std::unexpected(Private::ErrorCodeFromErrno(errno));
                }

                // Create file handle for parent process.
                fhReadError = FileHandle{Platform::NativeFileHandle{fd[0]}};

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

            return Process{Platform::NativeProcess{process_id}};
        }
        else
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }
#endif
    }

    std::expected<int32_t, ErrorCode> Process::Wait() const
    {
#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Wait not implemented");
        return {};
#else
        Platform::NativeProcess const& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Pid != 0);

        auto& error = errno;

        int status{};
        int rc{};

        do
        {
            rc = waitpid(nativeThis.Pid, &status, 0);
        } while ((rc < 0) and (error == EINTR));

        if (rc != nativeThis.Pid)
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return WTERMSIG(status) + 256;
#endif
    }

    std::expected<std::optional<int32_t>, ErrorCode> Process::TryWait() const
    {
#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return {};
#else
        Platform::NativeProcess const& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Pid != 0);

        auto& error = errno;

        int status;
        int rc;

        do
        {
            rc = waitpid(nativeThis.Pid, &status, WNOHANG);
        } while ((rc < 0) and (error == EINTR));

        if (rc == 0)
        {
            // Child process still running.
            return {};
        }

        if (rc != nativeThis.Pid)
        {
            // Failed to wait for child process.
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
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
#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Terminate not implemented");
        return {};
#else
        Platform::NativeProcess& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Pid != 0);


        if (kill(nativeThis.Pid, SIGKILL) != 0)
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return {};
#endif
    }
}
