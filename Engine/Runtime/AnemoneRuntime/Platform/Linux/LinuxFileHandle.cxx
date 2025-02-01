#include "AnemoneRuntime/Platform/Linux/LinuxFileHandle.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <unistd.h>
#include <sys/file.h>

namespace Anemone
{
    namespace
    {
        constexpr mode_t TranslateToOpenMode(Flags<FileOptions> options)
        {
            mode_t result = S_IRUSR | S_IWUSR;

            if (options.Any(FileOptions::ShareDelete))
            {
                result |= 0;
            }

            if (options.Any(FileOptions::ShareRead))
            {
                result |= S_IRGRP | S_IROTH;
            }

            if (options.Any(FileOptions::ShareWrite))
            {
                result |= S_IWGRP | S_IWOTH;
            }

            return result;
        }

        constexpr int TranslateToOpenFlags(FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options, bool failForSymlinks)
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

            if (options.Any(FileOptions::WriteThrough))
            {
                result |= O_SYNC;
            }

            return result;
        }
    }
}

namespace Anemone
{
    LinuxFileHandle::LinuxFileHandle(int handle)
        : _handle{handle}
    {
    }

    LinuxFileHandle::LinuxFileHandle(LinuxFileHandle&& other) noexcept
        : _handle{std::exchange(other._handle, InvalidHandle)}
    {
    }

    LinuxFileHandle& LinuxFileHandle::operator=(LinuxFileHandle&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->IsValid())
            {
                if (close(this->_handle))
                {
                    AE_TRACE(Error, "Failed to close file: fd={}, errno={}", this->_handle, errno);
                }
            }

            this->_handle = std::exchange(other._handle, InvalidHandle);
        }

        return *this;
    }

    LinuxFileHandle::~LinuxFileHandle() noexcept
    {
        if (this->IsValid())
        {
            if (close(this->_handle))
            {
                AE_TRACE(Error, "Failed to close file: fd={}, errno={}", this->_handle, errno);
            }
        }
    }

    std::expected<LinuxFileHandle, ErrorCode> LinuxFileHandle::Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options)
    {
        int const flags = TranslateToOpenFlags(mode, access, options, false);
        mode_t const fmode = TranslateToOpenMode(options);

        Interop::string_buffer<char, 256> spath{path};
        int const fd = open(spath.c_str(), flags, fmode);

        if (fd >= 0)
        {
            if (flock(fd, LOCK_EX | LOCK_NB))
            {
                int const error = errno;
                bool failed = false;

                if ((error == EAGAIN) or (error == EWOULDBLOCK))
                {
                    failed = true;
                }

                if (failed)
                {
                    close(fd);

                    return std::unexpected(ErrorCode::InvalidOperation);
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(fd, 0))
                {
                    close(fd);
                    return std::unexpected(ErrorCode::InvalidOperation);
                }
            }

            return LinuxFileHandle{fd};
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

        read = LinuxFileHandle{fd[0]};
        write = LinuxFileHandle{fd[1]};

        return {};
    }

    std::expected<void, ErrorCode> LinuxFileHandle::Close()
    {
        AE_ASSERT(this->IsValid());

        int const handle = std::exchange(this->_handle, InvalidHandle);

        if (handle >= 0)
        {
            if (close(handle))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void, ErrorCode> LinuxFileHandle::Flush()
    {
        AE_ASSERT(this->IsValid());

        if (fsync(this->_handle))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> LinuxFileHandle::GetLength() const
    {
        AE_ASSERT(this->IsValid());

        struct stat64 st
        {
            0
        };

        if (fstat64(this->_handle, &st))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return st.st_size;
    }

    std::expected<void, ErrorCode> LinuxFileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->IsValid());

        if (ftruncate64(this->_handle, length))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> LinuxFileHandle::GetPosition() const
    {
        AE_ASSERT(this->IsValid());

        off64_t const position = lseek64(this->_handle, 0, SEEK_CUR);

        if (position < 0)
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return position;
    }

    std::expected<void, ErrorCode> LinuxFileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->IsValid());

        if (lseek64(this->_handle, position, SEEK_SET) < 0)
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<size_t, ErrorCode> LinuxFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->IsValid());

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = read(this->_handle, buffer.data(), requested);

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
        AE_ASSERT(this->IsValid());

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = pread64(this->_handle, buffer.data(), requested, position);

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
        AE_ASSERT(this->IsValid());

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = write(this->_handle, buffer.data(), requested);

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
        AE_ASSERT(this->IsValid());

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = Interop::unix_ValidateIoRequestLength(buffer.size());

            while (true)
            {
                processed = pwrite64(this->_handle, buffer.data(), requested, position);

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
