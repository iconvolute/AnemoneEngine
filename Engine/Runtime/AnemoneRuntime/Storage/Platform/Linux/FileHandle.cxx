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
    }

    uint64_t LinuxFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        return 0;
    }

    void LinuxFileHandle::SetLength(uint64_t length)
    {
        AE_ASSERT(this->_handle);

        (void)length;
    }

    uint64_t LinuxFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);
        return 0;
    }

    void LinuxFileHandle::SetPosition(uint64_t position)
    {
        AE_ASSERT(this->_handle);

        (void)position;
    }

    size_t LinuxFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        return 0;
    }

    size_t LinuxFileHandle::ReadAt(std::span<std::byte> buffer, uint64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        (void)position;

        return 0;
    }

    size_t LinuxFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        return 0;
    }

    size_t LinuxFileHandle::WriteAt(std::span<std::byte const> buffer, uint64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        (void)position;

        return 0;
    }
}
