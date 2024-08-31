#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <cassert>
#include <cerrno>
#include <string>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::System::Private
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

namespace Anemone::System
{
    FileHandle::FileHandle(Platform::NativeFileHandle const& native)
    {
        Platform::Create(this->_native, native);
    }

    FileHandle::FileHandle()
    {
        Platform::Create(this->_native, Platform::NativeFileHandle{-1});
    }

    FileHandle::FileHandle(FileHandle&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), Platform::NativeFileHandle{-1}));
    }

    FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
            Platform::NativeFileHandle& nativeOther = Platform::Get(other._native);

            if (nativeThis.Descriptor >= 0)
            {
                if (close(nativeThis.Descriptor))
                {
                    AE_TRACE(Error, "Failed to close file: fd = {}, errno = {}", nativeThis.Descriptor, errno);
                }
            }

            nativeThis = std::exchange(nativeOther, Platform::NativeFileHandle{-1});
        }

        return *this;
    }

    FileHandle::~FileHandle()
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Descriptor >= 0)
        {
            if (close(nativeThis.Descriptor))
            {
                AE_TRACE(Error, "Failed to close file: fd = {}, errno = {}", nativeThis.Descriptor, errno);
            }
        }

        Platform::Destroy(this->_native);
    }

    std::expected<FileHandle, ErrorCode> FileHandle::Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options)
    {
        int const flags = Private::TranslateToOpenFlags(mode, access, options, false);

        mode_t const fmode = Private::TranslateToOpenMode(options);

        int const fd = open(std::string{path}.c_str(), flags, fmode);

        if (fd != -1)
        {
            if (flock(fd, LOCK_EX | LOCK_NB) == -1)
            {
                int const error = errno;
                bool failed{false};

                if ((error == EAGAIN) or (error == EWOULDBLOCK))
                {
                    failed = true;
                }

                if (failed)
                {
                    close(fd);
                    return std::unexpected(Private::ErrorCodeFromErrno(error));
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(fd, 0) != 0)
                {
                    int error = errno;
                    close(fd);

                    return std::unexpected(Private::ErrorCodeFromErrno(error));
                }
            }

            return FileHandle{Platform::NativeFileHandle{fd}};
        }

        return std::unexpected(Private::ErrorCodeFromErrno(errno));
    }

    std::expected<void, ErrorCode> FileHandle::CreatePipe(FileHandle& read, FileHandle& write)
    {
        int fd[2];

        if (pipe2(fd, O_CLOEXEC | O_NONBLOCK))
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        read = FileHandle{Platform::NativeFileHandle{fd[0]}};
        write = FileHandle{Platform::NativeFileHandle{fd[1]}};

        return {};
    }

    std::expected<void, ErrorCode> FileHandle::Close()
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);

        int const descriptor = std::exchange(nativeThis.Descriptor, -1);
        if (descriptor >= 0)
        {
            if (close(descriptor))
            {
                return std::unexpected(Private::ErrorCodeFromErrno(errno));
            }
        }

        return {};
    }

    std::expected<void, ErrorCode> FileHandle::Flush()
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        if (fsync(nativeThis.Descriptor))
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> FileHandle::GetLength() const
    {
        Platform::NativeFileHandle const& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        // clang-format off
        struct stat64 st{};
        // clang-format on

        if (fstat64(nativeThis.Descriptor, &st))
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return st.st_size;
    }

    std::expected<void, ErrorCode> FileHandle::SetLength(int64_t length)
    {
        Platform::NativeFileHandle const& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        if (ftruncate64(nativeThis.Descriptor, length))
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> FileHandle::GetPosition() const
    {
        Platform::NativeFileHandle const& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        off64_t const position = lseek64(nativeThis.Descriptor, 0, SEEK_CUR);

        if (position < 0)
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return position;
    }

    std::expected<void, ErrorCode> FileHandle::SetPosition(int64_t position)
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        if (lseek64(nativeThis.Descriptor, position, SEEK_SET) < 0)
        {
            return std::unexpected(Private::ErrorCodeFromErrno(errno));
        }

        return {};
    }

    std::expected<size_t, ErrorCode> FileHandle::Read(std::span<std::byte> buffer, int64_t position)
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = std::min(buffer.size(), static_cast<size_t>(std::numeric_limits<std::int32_t>::max()));

            while (true)
            {
                processed = pread64(nativeThis.Descriptor, buffer.data(), requested, position);

                if (processed < 0)
                {
                    if (auto error = errno; error != EINTR)
                    {
                        return std::unexpected(Private::ErrorCodeFromErrno(error));
                    }
                }
                else
                {
                    AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                    break;
                }
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> FileHandle::Write(std::span<std::byte const> buffer, int64_t position)
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = std::min(buffer.size(), static_cast<size_t>(std::numeric_limits<std::int32_t>::max()));

            while (true)
            {
                processed = pwrite64(nativeThis.Descriptor, buffer.data(), requested, position);

                if (processed < 0)
                {
                    if (auto error = errno; error != EINTR)
                    {
                        return std::unexpected(Private::ErrorCodeFromErrno(error));
                    }
                }
                else
                {
                    AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                    break;
                }
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> FileHandle::Read(std::span<std::byte> buffer)
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = std::min(buffer.size(), static_cast<size_t>(std::numeric_limits<std::int32_t>::max()));

            while (true)
            {
                processed = read(nativeThis.Descriptor, buffer.data(), requested);

                if (processed < 0)
                {
                    auto error = errno;

                    if (error == EINTR)
                    {
                        continue;
                    }

                    if (error == EAGAIN)
                    {
                        // The file descriptor is non-blocking and there is no data available at the moment. Treat this as an EOF situation.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Private::ErrorCodeFromErrno(error));
                }
                else
                {
                    AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                    break;
                }
            }
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, ErrorCode> FileHandle::Write(std::span<std::byte const> buffer)
    {
        Platform::NativeFileHandle& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Descriptor >= 0);

        ssize_t processed = 0;

        if (not buffer.empty())
        {
            size_t const requested = std::min(buffer.size(), static_cast<size_t>(std::numeric_limits<std::int32_t>::max()));

            while (true)
            {
                processed = write(nativeThis.Descriptor, buffer.data(), requested);

                if (processed < 0)
                {
                    auto error = errno;

                    if (error == EINTR)
                    {
                        continue;
                    }

                    if (error == EAGAIN)
                    {
                        // The file descriptor is non-blocking and there is no data available at the moment. Treat this as an EOF situation.
                        processed = 0;
                        break;
                    }

                    return std::unexpected(Private::ErrorCodeFromErrno(error));
                }
                else
                {
                    AE_ASSERT((0 <= processed) and (processed <= static_cast<ssize_t>(requested)));
                    break;
                }
            }
        }

        return static_cast<size_t>(processed);
    }
}
