#include "AnemoneRuntime/Platform/FileHandle.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxFileHandle.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <unistd.h>
#include <sys/file.h>

namespace Anemone
{
    namespace
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
}

namespace Anemone
{
    std::expected<LinuxFileHandle, ErrorCode> LinuxFileHandle::Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOption> options)
    {
        int const flags = TranslateToOpenFlags(mode, access, options, false);
        mode_t const fmode = TranslateToOpenMode(options);

        Interop::string_buffer<char, 256> spath{path};
        Interop::UnixSafeFdHandle fd{open(spath.c_str(), flags, fmode)};

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
                    return std::unexpected(ErrorCode::InvalidOperation);
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(fd.Get(), 0))
                {
                    return std::unexpected(ErrorCode::InvalidOperation);
                }
            }

            return LinuxFileHandle{std::move(fd)};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> LinuxFileHandle::CreatePipe(LinuxFileHandle& read, LinuxFileHandle& write)
    {
        int fd[2];

        if (pipe2(fd, O_CLOEXEC | O_NONBLOCK))
        {
            return std::unexpected(ErrorCode::InvalidOperation);
        }

        read = LinuxFileHandle{Interop::UnixSafeFdHandle{fd[0]}};
        write = LinuxFileHandle{Interop::UnixSafeFdHandle{fd[1]}};

        return {};
    }

    std::expected<void, ErrorCode> LinuxFileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        if (fsync(this->_handle.Get()))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> LinuxFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        struct stat64 st{0};

        if (fstat64(this->_handle.Get(), &st))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return st.st_size;
    }

    std::expected<void, ErrorCode> LinuxFileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->_handle);

        if (ftruncate64(this->_handle.Get(), length))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> LinuxFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        off64_t const position = lseek64(this->_handle.Get(), 0, SEEK_CUR);

        if (position < 0)
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return position;
    }

    std::expected<void, ErrorCode> LinuxFileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (lseek64(this->_handle.Get(), position, SEEK_SET) < 0)
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<size_t, ErrorCode> LinuxFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

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

                    return std::unexpected(ErrorCode::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> LinuxFileHandle::ReadAt(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

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

                    return std::unexpected(ErrorCode::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> LinuxFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

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

                    return std::unexpected(ErrorCode::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> LinuxFileHandle::WriteAt(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

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

                    return std::unexpected(ErrorCode::IoError);
                }

                AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                break;
            }
        }

        return static_cast<size_t>(processed);
    }
}
