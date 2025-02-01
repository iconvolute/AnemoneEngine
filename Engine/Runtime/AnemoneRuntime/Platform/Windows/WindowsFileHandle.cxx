#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{

    WindowsFileHandle::WindowsFileHandle(HANDLE handle)
        : _handle{handle}
    {
    }

    WindowsFileHandle::WindowsFileHandle(WindowsFileHandle&& other) noexcept
        : _handle{std::exchange(other._handle, nullptr)}
    {
    }

    WindowsFileHandle& WindowsFileHandle::operator=(WindowsFileHandle&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle != nullptr)
            {
                if (not CloseHandle(this->_handle))
                {
                    AE_TRACE(Error, "Failed to close file: handle={}, error={}", fmt::ptr(this->_handle), GetLastError());
                }
            }

            this->_handle = std::exchange(other._handle, nullptr);
        }

        return *this;
    }

    WindowsFileHandle::~WindowsFileHandle() noexcept
    {
        if (this->_handle != nullptr)
        {
            if (not CloseHandle(this->_handle))
            {
                AE_TRACE(Error, "Failed to close file: handle={}, error={}", fmt::ptr(this->_handle), GetLastError());
            }
        }
    }

    std::expected<WindowsFileHandle, ErrorCode> WindowsFileHandle::Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options)
    {
        DWORD dwCreationDisposition;

        switch (mode)
        {
        case FileMode::CreateAlways:
            dwCreationDisposition = CREATE_ALWAYS;
            break;

        case FileMode::CreateNew:
            dwCreationDisposition = CREATE_NEW;
            break;

        case FileMode::OpenAlways:
            dwCreationDisposition = OPEN_ALWAYS;
            break;

        case FileMode::OpenExisting:
            dwCreationDisposition = OPEN_EXISTING;
            break;

        case FileMode::TruncateExisting:
            dwCreationDisposition = TRUNCATE_EXISTING;
            break;

        default:
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD dwAccess;

        switch (access)
        {

        case FileAccess::Read:
            dwAccess = GENERIC_READ;
            break;

        case FileAccess::Write:
            dwAccess = GENERIC_WRITE;
            break;

        case FileAccess::ReadWrite:
            dwAccess = GENERIC_READ | GENERIC_WRITE;
            break;

        default:
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD dwShare = 0;
        DWORD dwFlags = 0;

        if (options.Has(FileOptions::ShareRead))
        {
            dwShare |= FILE_SHARE_READ;
        }

        if (options.Has(FileOptions::ShareWrite))
        {
            dwShare |= FILE_SHARE_WRITE;
        }

        if (options.Has(FileOptions::DeleteOnClose))
        {
            dwFlags |= FILE_FLAG_DELETE_ON_CLOSE;
        }

        if (options.Has(FileOptions::RandomAccess))
        {
            dwFlags |= FILE_FLAG_RANDOM_ACCESS;
        }

        if (options.Has(FileOptions::SequentialScan))
        {
            dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
        }

        if (options.Has(FileOptions::WriteThrough))
        {
            dwFlags |= FILE_FLAG_WRITE_THROUGH;
        }

        if (options.Has(FileOptions::NoBuffering))
        {
            dwFlags |= FILE_FLAG_NO_BUFFERING;
        }

        Interop::win32_FilePathW wpath{};
        if (Interop::win32_WidenString(wpath, path))
        {
            SECURITY_ATTRIBUTES sa{
                .nLength = sizeof(SECURITY_ATTRIBUTES),
                .lpSecurityDescriptor = nullptr,
                .bInheritHandle = options.Has(FileOptions::Inheritable),
            };

            CREATEFILE2_EXTENDED_PARAMETERS cf{
                .dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
                .dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
                .dwFileFlags = dwFlags,
                .dwSecurityQosFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                .lpSecurityAttributes = options.Has(FileOptions::Inheritable) ? &sa : nullptr,
                .hTemplateFile = nullptr,
            };

            HANDLE const result = CreateFile2(
                wpath.data(),
                dwAccess,
                dwShare,
                dwCreationDisposition,
                &cf);

            if (result != INVALID_HANDLE_VALUE)
            {
                return WindowsFileHandle{result};
            }

            return std::unexpected(ErrorCode{});
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> WindowsFileHandle::CreatePipe(WindowsFileHandle& read, WindowsFileHandle& write)
    {
        HANDLE hRead = nullptr;
        HANDLE hWrite = nullptr;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        if (::CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            read = WindowsFileHandle{hRead};
            write = WindowsFileHandle{hWrite};
            return {};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    std::expected<void, ErrorCode> WindowsFileHandle::Close()
    {
        AE_ASSERT(this->_handle != nullptr);

        HANDLE const handle = std::exchange(this->_handle, nullptr);

        if (handle != nullptr)
        {
            if (not CloseHandle(handle))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void, ErrorCode> WindowsFileHandle::Flush()
    {
        AE_ASSERT(this->_handle != nullptr);

        if (not FlushFileBuffers(this->_handle))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> WindowsFileHandle::GetLength() const
    {
        AE_ASSERT(this->_handle != nullptr);

        LARGE_INTEGER liLength{};

        if (not GetFileSizeEx(this->_handle, &liLength))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return liLength.QuadPart;
    }

    std::expected<void, ErrorCode> WindowsFileHandle::Truncate(int64_t length)
    {
        AE_ASSERT(this->_handle != nullptr);

        LARGE_INTEGER const liLength = std::bit_cast<LARGE_INTEGER>(length);

        // Set the file pointer to the specified position.
        if (not SetFilePointerEx(this->_handle, liLength, nullptr, FILE_BEGIN))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }


        // Truncate the file at the current position.
        if (not SetEndOfFile(this->_handle))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> WindowsFileHandle::GetPosition() const
    {
        AE_ASSERT(this->_handle != nullptr);

        LARGE_INTEGER liPosition{};
        LARGE_INTEGER liDistanceToMove{};

        if (not SetFilePointerEx(this->_handle, liDistanceToMove, &liPosition, FILE_CURRENT))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return std::bit_cast<int64_t>(liPosition);
    }

    std::expected<void, ErrorCode> WindowsFileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->_handle != nullptr);

        LARGE_INTEGER const liPosition = std::bit_cast<LARGE_INTEGER>(position);

        if (not SetFilePointerEx(this->_handle, liPosition, nullptr, FILE_BEGIN))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        return {};
    }

    std::expected<size_t, ErrorCode> WindowsFileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_handle != nullptr);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::win32_ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not ReadFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(ErrorCode::IoError);
            }
        }


        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> WindowsFileHandle::ReadAt(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle != nullptr);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::win32_ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::win32_GetOverlappedForPosition(position);

        if (not ReadFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle, &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(ErrorCode::IoError);
                }
            }
            else
            {
                return std::unexpected(ErrorCode::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> WindowsFileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->_handle != nullptr);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::win32_ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        if (not WriteFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(ErrorCode::IoError);
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> WindowsFileHandle::WriteAt(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->_handle != nullptr);

        if (buffer.empty())
        {
            return 0;
        }

        DWORD dwRequested = Interop::win32_ValidateIoRequestLength(buffer.size());
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::win32_GetOverlappedForPosition(position);

        if (not WriteFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();
            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->_handle, &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(ErrorCode::IoError);
                }
            }
            else
            {
                return std::unexpected(ErrorCode::IoError);
            }
        }

        return dwProcessed;
    }
}
