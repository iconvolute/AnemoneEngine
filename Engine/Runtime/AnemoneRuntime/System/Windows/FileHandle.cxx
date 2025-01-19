// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System
{
    FileHandle::FileHandle(Interop::NativeFileHandle const& native)
        : m_native{native}
    {
    }

    FileHandle::FileHandle()
        : m_native{INVALID_HANDLE_VALUE}
    {
    }

    FileHandle::FileHandle(FileHandle&& other) noexcept
        : m_native{std::exchange(other.m_native, {INVALID_HANDLE_VALUE})}
    {
    }

    FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->m_native.Handle != INVALID_HANDLE_VALUE)
            {
                if (not CloseHandle(this->m_native.Handle))
                {
                    AE_TRACE(Error, "Failed to close file: handle = {}, error = {}", fmt::ptr(this->m_native.Handle), GetLastError());
                }
            }

            this->m_native = std::exchange(other.m_native, Interop::NativeFileHandle{INVALID_HANDLE_VALUE});
        }

        return *this;
    }

    FileHandle::~FileHandle()
    {
        if (this->m_native.Handle != INVALID_HANDLE_VALUE)
        {
            if (not CloseHandle(this->m_native.Handle))
            {
                AE_TRACE(Error, "Failed to close file: handle = {}, error = {}", fmt::ptr(this->m_native.Handle), GetLastError());
            }
        }
    }

    std::expected<FileHandle, ErrorCode> FileHandle::Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options)
    {
        DWORD dwMode = 0;
        DWORD dwShare = 0;
        DWORD dwAccess = 0;
        DWORD dwFlags = 0;

        switch (mode)
        {
        case FileMode::CreateNew:
            dwMode = CREATE_NEW;
            break;

        case FileMode::CreateAlways:
            dwMode = CREATE_ALWAYS;
            break;

        case FileMode::OpenExisting:
            dwMode = OPEN_EXISTING;
            break;

        case FileMode::OpenAlways:
            dwMode = OPEN_ALWAYS;
            break;

        case FileMode::TruncateExisting:
            dwMode = TRUNCATE_EXISTING;
            break;

        default:
            AE_PANIC("Unknown FileMode: {}", std::to_underlying(mode));
        }

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
            AE_PANIC("Unknown FileMode: {}", std::to_underlying(mode));
        }

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
        else if (options.Has(FileOptions::SequentialScan))
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
                .bInheritHandle = TRUE,
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
                dwMode,
                &cf);

            if (result != INVALID_HANDLE_VALUE)
            {
                return FileHandle{Interop::NativeFileHandle{result}};
            }

            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> FileHandle::CreatePipe(FileHandle& read, FileHandle& write)
    {
        Interop::NativeFileHandle nativeRead{INVALID_HANDLE_VALUE};
        Interop::NativeFileHandle nativeWrite{INVALID_HANDLE_VALUE};

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        if (::CreatePipe(&nativeRead.Handle, &nativeWrite.Handle, &sa, 0))
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        read = FileHandle{nativeRead};
        write = FileHandle{nativeWrite};
        return {};
    }

    std::expected<void, ErrorCode> FileHandle::Close()
    {
        HANDLE const handle = std::exchange(this->m_native.Handle, INVALID_HANDLE_VALUE);
        if (handle != INVALID_HANDLE_VALUE)
        {
            if (not CloseHandle(handle))
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
            }

            this->m_native.Handle = INVALID_HANDLE_VALUE;
        }

        return {};
    }

    std::expected<void, ErrorCode> FileHandle::Flush()
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        if (not FlushFileBuffers(this->m_native.Handle))
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> FileHandle::GetLength() const
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        LARGE_INTEGER liLength{};

        if (not GetFileSizeEx(this->m_native.Handle, &liLength))
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return liLength.QuadPart;
    }

    std::expected<void, ErrorCode> FileHandle::SetLength(int64_t length)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        LARGE_INTEGER const liLength = std::bit_cast<LARGE_INTEGER>(length);

        BOOL result = SetFilePointerEx(this->m_native.Handle, liLength, nullptr, FILE_BEGIN);

        if (result != FALSE)
        {
            result = SetEndOfFile(this->m_native.Handle);
        }

        if (result == FALSE)
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return {};
    }

    std::expected<int64_t, ErrorCode> FileHandle::GetPosition() const
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        LARGE_INTEGER liPosition{};
        LARGE_INTEGER liDistanceToMove{};

        if (not SetFilePointerEx(this->m_native.Handle, liDistanceToMove, &liPosition, SEEK_CUR))
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return std::bit_cast<int64_t>(liPosition);
    }

    std::expected<void, ErrorCode> FileHandle::SetPosition(int64_t position)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        if (not SetFilePointerEx(this->m_native.Handle, std::bit_cast<LARGE_INTEGER>(position), nullptr, FILE_BEGIN))
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
        }

        return {};
    }

    std::expected<size_t, ErrorCode> FileHandle::Read(std::span<std::byte> buffer, int64_t position)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        DWORD dwRequested = static_cast<DWORD>(std::min<size_t>(buffer.size(), std::numeric_limits<DWORD>::max()));
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::win32_GetOverlappedForPosition(position);

        if (not ReadFile(this->m_native.Handle, buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->m_native.Handle, &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
                }
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> FileHandle::Write(std::span<std::byte const> buffer, int64_t position)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        DWORD dwRequested = static_cast<DWORD>(std::min<size_t>(buffer.size(), std::numeric_limits<DWORD>::max()));
        DWORD dwProcessed = 0;

        OVERLAPPED overlapped = Interop::win32_GetOverlappedForPosition(position);

        if (not WriteFile(this->m_native.Handle, buffer.data(), dwRequested, &dwProcessed, &overlapped))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else if (dwError == ERROR_IO_PENDING)
            {
                if (not GetOverlappedResult(this->m_native.Handle, &overlapped, &dwProcessed, TRUE))
                {
                    return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
                }
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> FileHandle::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        DWORD dwRequested = static_cast<DWORD>(std::min<size_t>(buffer.size(), std::numeric_limits<DWORD>::max()));
        DWORD dwProcessed = 0;

        if (not ReadFile(this->m_native.Handle, buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
            }
        }

        return dwProcessed;
    }

    std::expected<size_t, ErrorCode> FileHandle::Write(std::span<std::byte const> buffer)
    {
        AE_ASSERT(this->m_native.Handle != INVALID_HANDLE_VALUE);

        DWORD dwRequested = static_cast<DWORD>(std::min<size_t>(buffer.size(), std::numeric_limits<DWORD>::max()));
        DWORD dwProcessed = 0;

        if (not WriteFile(this->m_native.Handle, buffer.data(), dwRequested, &dwProcessed, nullptr))
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_HANDLE_EOF) or (dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
            {
                AE_ASSERT(dwProcessed == 0);
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
            }
        }

        return dwProcessed;
    }
}
