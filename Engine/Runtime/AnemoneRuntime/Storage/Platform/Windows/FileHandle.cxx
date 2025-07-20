#include "AnemoneRuntime/Storage/Platform/Windows/FileHandle.hxx"

#include "FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

namespace Anemone
{
    WindowsFileHandle::WindowsFileHandle(Interop::Windows::SafeFileHandle handle, WindowsFileSystem* owner)
        : _handle{std::move(handle)}
        , _owner{owner}
    {
        if (this->_owner)
        {
            this->_owner->RegisterHandle(*this);
        }
    }


    WindowsFileHandle::~WindowsFileHandle()
    {
        if (this->_owner)
        {
            this->_owner->UnregisterHandle(*this);
        }
    }


    std::expected<void, Status> WindowsFileHandle::Flush()
    {
        AE_ASSERT(this->_handle);

        if (not FlushFileBuffers(this->_handle.Get()))
        {
            return std::unexpected(Status::IoError);
        }

        return {};
    }

    std::expected<int64_t, Status> WindowsFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER liLength{};

        if (not GetFileSizeEx(this->_handle.Get(), &liLength))
        {
            return std::unexpected(Status::IoError);
        }

        return std::bit_cast<int64_t>(liLength);
    }

    std::expected<void, Status> WindowsFileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER const liLength = std::bit_cast<LARGE_INTEGER>(length);

        // Set file pointer to the specified position.
        if (not SetFilePointerEx(this->_handle.Get(), liLength, nullptr, FILE_BEGIN))
        {
            return std::unexpected(Status::IoError);
        }

        // Truncate the file at the current position.
        if (not SetEndOfFile(this->_handle.Get()))
        {
            return std::unexpected(Status::IoError);
        }

        return {};
    }

    std::expected<int64_t, Status> WindowsFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER liPosition{};
        LARGE_INTEGER liDistanceToMove{};

        // Obtain the current file position.
        if (not SetFilePointerEx(this->_handle.Get(), liDistanceToMove, &liPosition, FILE_CURRENT))
        {
            return std::unexpected(Status::IoError);
        }

        return std::bit_cast<int64_t>(liPosition);
    }

    std::expected<void, Status> WindowsFileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->_handle);

        LARGE_INTEGER const liPosition = std::bit_cast<LARGE_INTEGER>(position);

        // Set the file pointer to the specified position.
        if (not SetFilePointerEx(this->_handle.Get(), liPosition, nullptr, FILE_BEGIN))
        {
            return std::unexpected(Status::IoError);
        }

        return {};
    }

    std::expected<size_t, Status> WindowsFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not ReadFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(Status::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Status> WindowsFileHandle::ReadAt(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::Windows::GetOverlappedForPosition(position);

        if (not ReadFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle.Get(), &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Status::IoError);
                }
            }
            else
            {
                return std::unexpected(Status::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Status> WindowsFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not WriteFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(Status::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, Status> WindowsFileHandle::WriteAt(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::Windows::ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::Windows::GetOverlappedForPosition(position);

        if (not WriteFile(this->_handle.Get(), buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle.Get(), &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Status::IoError);
                }
            }
            else
            {
                return std::unexpected(Status::IoError);
            }
        }

        return dwProcessed;
    }
}
