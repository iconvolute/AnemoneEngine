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
    std::expected<int32_t, Status> Process::Wait()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Wait not implemented");
        return std::unexpected(Status::NotImplemented);
#else

        Internal::PlatformProcess const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            int status;
            int rc = Interop::Linux::WaitForProcess(handle.Get(), status, 0);

            if (rc != handle.Get())
            {
                return std::unexpected(Status::InvalidOperation);
            }

            if (WIFEXITED(status))
            {
                return WEXITSTATUS(status);
            }

            return WTERMSIG(status) + 256;
        }

        return std::unexpected(Status::InvalidHandle);

#endif
    }

    std::expected<int32_t, Status> Process::TryWait()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::TryWait not implemented");
        return std::unexpected(Status::NotImplemented);
#else

        if (this->_handle)
        {
            int status;
            int rc = Interop::Linux::WaitForProcess(this->_handle.Get(), status, WNOHANG);

            if (rc == 0)
            {
                // Child process still running.
                return std::unexpected(Status::OperationInProgress);
            }

            if (rc != this->_handle.Get())
            {
                this->_handle = {};

                // Failed to wait for child process.
                return std::unexpected(Status::InvalidOperation);
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

        return std::unexpected(Status::InvalidHandle);
#endif
    }

    std::expected<void, Status> Process::Terminate()
    {
        AE_ASSERT(this->_handle);

#if !ANEMONE_PLATFORM_LINUX
        AE_TRACE(Critical, "Process::Terminate not implemented");
        return std::unexpected(Status::NotImplemented);
#else

        Internal::PlatformProcess const handle = std::exchange(this->_handle, {});

        if (handle)
        {
            if (Interop::Linux::TerminateProcess(handle.Get()))
            {
                return {};
            }

            return std::unexpected(Status::InvalidOperation);
        }

        return std::unexpected(Status::InvalidHandle);
#endif
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
        std::unique_ptr<FileHandle>* input,
        std::unique_ptr<FileHandle>* output,
        std::unique_ptr<FileHandle>* error)
        -> std::expected<Process, Status>
    {
#if !ANEMONE_PLATFORM_LINUX
        (void)path;
        (void)params;
        (void)workingDirectory;
        (void)input;
        (void)output;
        (void)error;
        return {};
        return std::unexpected(Status::NotImplemented);
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
            return std::unexpected(Status::InvalidArgument);
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
                    return std::unexpected(Status::InvalidOperation);
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
                    return std::unexpected(Status::InvalidOperation);
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
                    return std::unexpected(Status::InvalidOperation);
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
            return std::unexpected(Status::InvalidOperation);
        }
#endif
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
