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


    LinuxFileHandle::~LinuxFileHandle()
    {
    }


    std::expected<void, Status> LinuxFileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<int64_t, Status> LinuxFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<void, Status> LinuxFileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->_handle);

        (void)length;

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<int64_t, Status> LinuxFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<void, Status> LinuxFileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->_handle);

        (void)position;

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<size_t, Status> LinuxFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<size_t, Status> LinuxFileHandle::ReadAt(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        (void)position;

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<size_t, Status> LinuxFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        return std::unexpected(Status::NotImplemented);
    }

    std::expected<size_t, Status> LinuxFileHandle::WriteAt(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        (void)position;

        return std::unexpected(Status::NotImplemented);
    }
}
