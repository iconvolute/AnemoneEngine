#include "AnemoneRuntime/Storage/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

namespace Anemone
{
    WindowsFileHandle::WindowsFileHandle(Interop::Windows::SafeFileHandle handle)
        : _handle{std::move(handle)}
    {
    }

    WindowsFileHandle::~WindowsFileHandle() = default;

    std::expected<void, Error> WindowsFileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        if (not FlushFileBuffers(this->_handle.Get()))
        {
            return std::unexpected(Error::IoError);
        }

        return {};
    }

    std::expected<uint64_t, Error> WindowsFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER liLength{};

        if (not GetFileSizeEx(this->_handle.Get(), &liLength))
        {
            return std::unexpected(Error::IoError);
        }

        return std::bit_cast<uint64_t>(liLength);
    }

    std::expected<void, Error> WindowsFileHandle::SetLength(uint64_t length)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(length <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        LARGE_INTEGER const liLength = std::bit_cast<LARGE_INTEGER>(length);

        // FIXME: Setting length of the stream leaves cursor at that position.

        if (not SetFilePointerEx(this->_handle.Get(), liLength, nullptr, FILE_BEGIN))
        {
            return std::unexpected(Error::IoError);
        }

        if (not SetEndOfFile(this->_handle.Get()))
        {
            return std::unexpected(Error::IoError);
        }

        return {};
    }

    std::expected<uint64_t, Error> WindowsFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER liPosition{};
        LARGE_INTEGER liDistanceToMove{};

        if (not SetFilePointerEx(this->_handle.Get(), liDistanceToMove, &liPosition, FILE_CURRENT))
        {
            return std::unexpected(Error::IoError);
        }

        return std::bit_cast<uint64_t>(liPosition);
    }

    std::expected<void, Error> WindowsFileHandle::SetPosition(uint64_t position)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(position <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        LARGE_INTEGER const liPosition = std::bit_cast<LARGE_INTEGER>(position);

        // Set the file pointer to the specified position.
        if (not SetFilePointerEx(this->_handle.Get(), liPosition, nullptr, FILE_BEGIN))
        {
            return std::unexpected(Error::IoError);
        }

        return {};
    }

    std::expected<size_t, Error> WindowsFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD const dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not ReadFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            AE_ASSERT(dwProcessed == 0);
            DWORD const dwError = GetLastError();

            if ((dwError != ERROR_HANDLE_EOF) and (dwError != ERROR_BROKEN_PIPE) and (dwError != ERROR_NO_DATA))
            {
                return std::unexpected(Error::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Error> WindowsFileHandle::ReadAt(
        std::span<std::byte> buffer,
        uint64_t position)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(position <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        if (buffer.empty())
        {
            return 0;
        }

        DWORD const dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::Windows::GetOverlappedForPosition(position);

        if (not ReadFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            AE_ASSERT(dwProcessed == 0);
            DWORD const dwError = GetLastError();

            if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle.Get(), &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Error::IoError);
                }
            }
            else if ((dwError != ERROR_HANDLE_EOF) and (dwError != ERROR_BROKEN_PIPE) and (dwError != ERROR_NO_DATA))
            {
                return std::unexpected(Error::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Error> WindowsFileHandle::Write(
        std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD const dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not WriteFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            AE_ASSERT(dwProcessed == 0);
            DWORD const dwError = GetLastError();

            if ((dwError != ERROR_HANDLE_EOF) and (dwError != ERROR_BROKEN_PIPE) and (dwError != ERROR_NO_DATA))
            {
                return std::unexpected(Error::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Error> WindowsFileHandle::WriteAt(
        std::span<std::byte const> buffer,
        uint64_t position)
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(position <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));

        if (buffer.empty())
        {
            return 0;
        }

        DWORD const dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::Windows::GetOverlappedForPosition(position);

        if (not WriteFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            AE_ASSERT(dwProcessed == 0);
            DWORD const dwError = GetLastError();

            if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle.Get(), &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Error::IoError);
                }
            }

            if ((dwError != ERROR_HANDLE_EOF) and (dwError != ERROR_BROKEN_PIPE) and (dwError != ERROR_NO_DATA))
            {
                return std::unexpected(Error::IoError);
            }
        }

        return dwProcessed;
    }
}
