#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <unistd.h>
#include <sys/file.h>

namespace Anemone::Internal
{
    constexpr mode_t TranslateToOpenMode(Flags<FileOption> options)
    {
        mode_t result = S_IRUSR | S_IWUSR;

        if (options.Any(FileOption::ShareDelete))
        {
            result |= 0;
        }

        if (options.Any(FileOption::ShareRead))
        {
            result |= S_IRGRP | S_IROTH;
        }

        if (options.Any(FileOption::ShareWrite))
        {
            result |= S_IWGRP | S_IWOTH;
        }

        return result;
    }

    constexpr int TranslateToOpenFlags(FileMode mode, Flags<FileAccess> access, Flags<FileOption> options, bool failForSymlinks)
    {
        int result = O_CLOEXEC;

        if (failForSymlinks)
        {
            result |= O_NOFOLLOW;
        }

        switch (mode)
        {
        case FileMode::OpenExisting:
            break;

        case FileMode::TruncateExisting:
            result |= O_TRUNC;
            break;

        case FileMode::OpenAlways:
            result |= O_CREAT;
            break;

        case FileMode::CreateAlways:
            result |= O_CREAT | O_TRUNC;
            break;

        case FileMode::CreateNew:
            result |= O_CREAT | O_EXCL;
            break;
        }

        switch (access)
        {
        case FileAccess::Read:
            result |= O_RDONLY;
            break;

        case FileAccess::Write:
            result |= O_WRONLY;
            break;

        case FileAccess::ReadWrite:
            result |= O_RDWR;
            break;
        }

        if (options.Any(FileOption::WriteThrough))
        {
            result |= O_SYNC;
        }

        return result;
    }
}

namespace Anemone
{
    std::expected<FileHandle, Status> FileHandle::Create(
        std::string_view path,
        FileMode mode,
        Flags<FileAccess> access,
        Flags<FileOption> options)
    {
        int const flags = Internal::TranslateToOpenFlags(mode, access, options, false);
        mode_t const fmode = Internal::TranslateToOpenMode(options);

        Interop::string_buffer<char, 256> spath{path};
        Interop::Linux::SafeFdHandle fd{open(spath.c_str(), flags, fmode)};

        if (fd)
        {
            if (flock(fd.Get(), LOCK_EX | LOCK_NB))
            {
                int const error = errno;
                bool failed = false;

                if ((error == EAGAIN) or (error == EWOULDBLOCK))
                {
                    failed = true;
                }

                if (failed)
                {
                    return std::unexpected(Status::InvalidOperation);
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(fd.Get(), 0))
                {
                    return std::unexpected(Status::InvalidOperation);
                }
            }

            return FileHandle{std::move(fd)};
        }

        return std::unexpected(Status::InvalidArgument);
    }

    std::expected<void, Status> FileHandle::CreatePipe(FileHandle& read, FileHandle& write)
    {
        int fd[2];

        if (pipe2(fd, O_CLOEXEC | O_NONBLOCK))
        {
            return std::unexpected(Status::InvalidOperation);
        }

        read = FileHandle{Interop::Linux::SafeFdHandle{fd[0]}};
        write = FileHandle{Interop::Linux::SafeFdHandle{fd[1]}};

        return {};
    }

    std::expected<void, Status> FileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        if (fsync(this->_handle.Get()))
        {
            return std::unexpected(Status::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, Status> FileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        struct stat64 st{0};

        if (fstat64(this->_handle.Get(), &st))
        {
            return std::unexpected(Status::InvalidHandle);
        }

        return st.st_size;
    }

    std::expected<void, Status> FileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->_handle);

        if (ftruncate64(this->_handle.Get(), length))
        {
            return std::unexpected(Status::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, Status> FileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        off64_t const position = lseek64(this->_handle.Get(), 0, SEEK_CUR);

        if (position < 0)
        {
            return std::unexpected(Status::InvalidHandle);
        }

        return position;
    }

    std::expected<void, Status> FileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (lseek64(this->_handle.Get(), position, SEEK_SET) < 0)
        {
            return std::unexpected(Status::InvalidHandle);
        }

        return {};
    }

    std::expected<size_t, Status> FileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = read(this->_handle.Get(), buffer.data(), requested);

                if (processed < 0)
                {
                    int const error = errno;

                    if (error == EINTR)
                    {
                        // The call was interrupted by a signal before any data was read.
                        continue;
                    }

                    if ((error == EAGAIN) or (error == EWOULDBLOCK))
                    {
                        // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the read would block.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Status::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, Status> FileHandle::ReadAt(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = pread64(this->_handle.Get(), buffer.data(), requested, position);

                if (processed < 0)
                {
                    int const error = errno;

                    if (error == EINTR)
                    {
                        // The call was interrupted by a signal before any data was read.
                        continue;
                    }

                    if ((error == EAGAIN) or (error == EWOULDBLOCK))
                    {
                        // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the read would block.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Status::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, Status> FileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = write(this->_handle.Get(), buffer.data(), requested);

                if (processed < 0)
                {
                    int const error = errno;

                    if (error == EINTR)
                    {
                        // The call was interrupted by a signal before any data was read.
                        continue;
                    }

                    if ((error == EAGAIN) or (error == EWOULDBLOCK))
                    {
                        // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the write would block.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Status::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, Status> FileHandle::WriteAt(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = pwrite64(this->_handle.Get(), buffer.data(), requested, position);

                if (processed < 0)
                {
                    int const error = errno;
                    if (error == EINTR)
                    {
                        // The call was interrupted by a signal before any data was read.
                        continue;
                    }
                    if ((error == EAGAIN) or (error == EWOULDBLOCK))
                    {
                        // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the write would block.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Status::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }
}
