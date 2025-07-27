#include "AnemoneRuntime/Storage/Platform/Linux/FileHandle.hxx"

#include "FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"

namespace Anemone
{
    LinuxFileHandle::LinuxFileHandle(Interop::Linux::SafeFdHandle handle)
        : _handle{std::move(handle)}
    {
    }


    LinuxFileHandle::~LinuxFileHandle() = default;


    void LinuxFileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        int result{};

        do
        {
            result = fsync(this->_handle.Get());
        } while ((result < 0) and (errno == EINTR));

        if (result < 0)
        {
            AE_VERIFY_ERRNO(errno);
        }
    }

    uint64_t LinuxFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        struct stat64 st{0};

        if (fstat64(this->_handle.Get(), &st) < 0)
        {
            AE_VERIFY_ERRNO(errno);
            return 0;
        }

        return static_cast<uint64_t>(st.st_size);
    }

    void LinuxFileHandle::SetLength(uint64_t length)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(length <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        // TODO: make this pattern an Interop function helper
        int result = 0;
        do
        {
            result = ftruncate64(this->_handle.Get(), static_cast<off64_t>(length));
        } while ((result < 0) and (errno == EINTR));

        if (result < 0)
        {
            AE_VERIFY_ERRNO(errno);
        }
    }

    uint64_t LinuxFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        off64_t result;

        do
        {
            result = lseek64(this->_handle.Get(), 0, SEEK_CUR);
        } while ((result < 0) and (errno == EINTR));

        if (result < 0)
        {
            AE_VERIFY_ERRNO(errno);
            return 0;
        }

        return static_cast<uint64_t>(result);
    }

    void LinuxFileHandle::SetPosition(uint64_t position)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(position <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        off64_t result;

        do
        {
            result = lseek64(this->_handle.Get(), static_cast<off64_t>(position), SEEK_SET);
        } while ((result < 0) and (errno == EINTR));

        if (result < 0)
        {
            AE_VERIFY_ERRNO(errno);
        }
    }

    size_t LinuxFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                ssize_t const processed = read(this->_handle.Get(), buffer.data(), requested);

                if (processed >= 0)
                {
                    return static_cast<size_t>(processed);
                }

                if (errno == EINTR)
                {
                    // The call was interrupted by a signal before any data was read.
                    continue;
                }

                if ((errno == EAGAIN) or (errno == EWOULDBLOCK))
                {
                    // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the read would block.
                    return 0;
                }

                AE_VERIFY_ERRNO(errno);
                return 0;
            }
        }

        return 0;
    }

    size_t LinuxFileHandle::ReadAt(std::span<std::byte> buffer, uint64_t position)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(position <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());
            loff_t const offset = static_cast<loff_t>(position);

            while (true)
            {
                ssize_t const processed = pread64(this->_handle.Get(), buffer.data(), requested, offset);

                if (processed >= 0)
                {
                    return static_cast<size_t>(processed);
                }

                if (errno == EINTR)
                {
                    // The call was interrupted by a signal before any data was read.
                    continue;
                }

                if ((errno == EAGAIN) or (errno == EWOULDBLOCK))
                {
                    // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the read would block.
                    return 0;
                }

                AE_VERIFY_ERRNO(errno);
                return 0;
            }
        }

        return 0;
    }

    size_t LinuxFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());

            while (true)
            {
                ssize_t const processed = write(this->_handle.Get(), buffer.data(), requested);

                if (processed >= 0)
                {
                    return static_cast<size_t>(processed);
                }

                if (errno == EINTR)
                {
                    // The call was interrupted by a signal before any data was written.
                    continue;
                }

                if ((errno == EAGAIN) or (errno == EWOULDBLOCK))
                {
                    // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the write would block.
                    return 0;
                }

                AE_VERIFY_ERRNO(errno);
                return 0;
            }
        }

        return 0;
    }

    size_t LinuxFileHandle::WriteAt(std::span<std::byte const> buffer, uint64_t position)
    {
        AE_ASSERT(this->_handle);

        if (not buffer.empty())
        {
            size_t const requested = Interop::Linux::ValidateIoRequestLength(buffer.size());
            loff_t const offset = static_cast<loff_t>(position);

            while (true)
            {
                ssize_t const processed = pwrite64(this->_handle.Get(), buffer.data(), requested, offset);

                if (processed >= 0)
                {
                    return static_cast<size_t>(processed);
                }

                if (errno == EINTR)
                {
                    // The call was interrupted by a signal before any data was written.
                    continue;
                }

                if ((errno == EAGAIN) or (errno == EWOULDBLOCK))
                {
                    // The file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the write would block.
                    return 0;
                }

                AE_VERIFY_ERRNO(errno);
                return 0;
            }
        }

        return 0;
    }
}
