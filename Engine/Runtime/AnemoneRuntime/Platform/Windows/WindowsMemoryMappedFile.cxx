#include "AnemoneRuntime/Platform/Windows/WindowsMemoryMappedFile.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Error.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Internal
{
    static DWORD ConvertFileMapAccess(MemoryMappedFileAccess access)
    {
        DWORD result = 0;

        switch (access)
        {
        case MemoryMappedFileAccess::ReadOnly:
            result = FILE_MAP_READ;
            break;

        case MemoryMappedFileAccess::ReadWrite:
            result = FILE_MAP_READ | FILE_MAP_WRITE;
            break;

        case MemoryMappedFileAccess::CopyOnWrite:
            result = FILE_MAP_COPY;
            break;

        case MemoryMappedFileAccess::ReadExecute:
            result = FILE_MAP_READ | FILE_MAP_EXECUTE;
            break;

        default:
            break;
        }

        return result;
    }

    static DWORD ConverPageAccess(MemoryMappedFileAccess access)
    {
        DWORD result = 0;
        switch (access)
        {
        case MemoryMappedFileAccess::ReadOnly:
            result = PAGE_READONLY;
            break;
        case MemoryMappedFileAccess::ReadWrite:
            result = PAGE_READWRITE;
            break;
        case MemoryMappedFileAccess::CopyOnWrite:
            result = PAGE_WRITECOPY;
            break;
        case MemoryMappedFileAccess::ReadExecute:
            result = PAGE_EXECUTE_READ;
            break;
        default:
            break;
        }

        return result;
    }
}

namespace Anemone
{
    auto WindowsMemoryMappedFile::OpenCore(
        const wchar_t* name,
        MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        DWORD const dwDesiredAccess = Internal::ConvertFileMapAccess(access);

        Interop::Win32SafeMemoryMappedFileHandle handle;
        handle.Attach(OpenFileMappingW(
            dwDesiredAccess,
            FALSE,
            name));

        if (handle)
        {
            return WindowsMemoryMappedFile{std::move(handle)};
        }

        return std::unexpected(Internal::TranslateErrorCodeWin32(GetLastError()));
    }

    auto WindowsMemoryMappedFile::CreateCore(
        Interop::Win32SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        DWORD dwPageAccess = Internal::ConverPageAccess(access);
        DWORD dwCapacityHigh = static_cast<DWORD>(capacity >> 32);
        DWORD dwCapacityLow = static_cast<DWORD>(capacity);

        HANDLE hFile = handle ? handle.Get() : INVALID_HANDLE_VALUE;

        Interop::Win32SafeMemoryMappedFileHandle result;
        result.Attach(CreateFileMappingW(hFile,
            &sa,
            dwPageAccess,
            dwCapacityHigh,
            dwCapacityLow,
            name));

        DWORD const dwError = GetLastError();

        if (result)
        {
            if (dwError == ERROR_ALREADY_EXISTS)
            {
                return std::unexpected(ErrorCode::AlreadyExists);
            }
        }
        else
        {
            return std::unexpected(Internal::TranslateErrorCodeWin32(dwError));
        }

        return WindowsMemoryMappedFile{std::move(result)};
    }

    auto WindowsMemoryMappedFile::OpenOrCreateCore(
        Interop::Win32SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        size_t retries = 20;

        HANDLE const hFile = handle ? handle.Get() : INVALID_HANDLE_VALUE;

        DWORD const dwPageAccess = Internal::ConverPageAccess(access);
        DWORD const dwDesiredAccess = Internal::ConvertFileMapAccess(access);
        DWORD const dwCapacityHigh = static_cast<DWORD>(capacity >> 32);
        DWORD const dwCapacityLow = static_cast<DWORD>(capacity);

        Interop::Win32SafeMemoryMappedFileHandle result;

        while (retries)
        {
            result.Attach(CreateFileMappingW(
                hFile,
                &sa,
                dwPageAccess,
                dwCapacityHigh,
                dwCapacityLow,
                name));

            if (result)
            {
                break;
            }

            if (DWORD const dwError = GetLastError(); dwError != ERROR_ACCESS_DENIED)
            {
                return std::unexpected(Internal::TranslateErrorCodeWin32(dwError));
            }

            result.Attach(OpenFileMappingW(dwDesiredAccess, FALSE, name));

            if (result)
            {
                break;
            }

            if (DWORD const dwError = GetLastError(); dwError != ERROR_FILE_NOT_FOUND)
            {
                return std::unexpected(Internal::TranslateErrorCodeWin32(dwError));
            }

            --retries;
            SleepEx(100, TRUE);
        }

        return WindowsMemoryMappedFile{std::move(result)};
    }

    // Anonymous, in-process memory mapped file.
    auto WindowsMemoryMappedFile::Create(int64_t capacity, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        return CreateCore(
            {},
            nullptr,
            access,
            capacity);
    }

    auto WindowsMemoryMappedFile::Create(WindowsFileHandle const& fileHandle, int64_t capacity, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        return CreateCore(
            fileHandle.GetNativeHandle(),
            nullptr,
            access,
            capacity);
    }

    // Named, create, fail if exists.
    auto WindowsMemoryMappedFile::Create(std::string_view name, int64_t capacity, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        Interop::win32_FilePathW wsName;
        if (not Interop::win32_WidenString(wsName, name))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return CreateCore(
            {},
            wsName.c_str(),
            access,
            capacity);
    }

    auto WindowsMemoryMappedFile::Create(WindowsFileHandle const& fileHandle, std::string_view name, int64_t capacity, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        Interop::win32_FilePathW wsName;
        if (not Interop::win32_WidenString(wsName, name))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return CreateCore(
            fileHandle.GetNativeHandle(),
            wsName.c_str(),
            access,
            capacity);
    }

    // Named, open, fail if doesn't exist.
    auto WindowsMemoryMappedFile::Open(std::string_view name, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        Interop::win32_FilePathW wsName;
        if (not Interop::win32_WidenString(wsName, name))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return OpenCore(
            wsName.c_str(),
            access);
    }

    // Named, create or open, fail for any other reason.
    auto WindowsMemoryMappedFile::OpenOrCreate(WindowsFileHandle const& fileHandle, std::string_view name, int64_t capacity, MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedFile, ErrorCode>
    {
        Interop::win32_FilePathW wsName;
        if (not Interop::win32_WidenString(wsName, name))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return OpenOrCreateCore(
            fileHandle.GetNativeHandle(),
            wsName.c_str(),
            access,
            capacity);
    }
}

namespace Anemone
{
    auto WindowsMemoryMappedView::Create(
        WindowsMemoryMappedFile const& fileHandle,
        MemoryMappedFileAccess access)
        -> std::expected<WindowsMemoryMappedView, ErrorCode>
    {
        DWORD const dwDesiredAccess = Internal::ConvertFileMapAccess(access);

        //
        // Map whole range of mapped file.
        //

        LPVOID data = MapViewOfFile(
            fileHandle.GetNativeHandle().Get(),
            dwDesiredAccess,
            0,
            0,
            0);

        size_t size = 0;

        if (data)
        {
            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery(data, &mbi, sizeof(mbi));

            size = mbi.RegionSize;
        }

        Interop::Win32SafeMemoryMappedViewHandle handle{data, size};

        if (handle)
        {
            return WindowsMemoryMappedView{std::move(handle)};
        }

        return std::unexpected(Internal::TranslateErrorCodeWin32(GetLastError()));
    }

    auto WindowsMemoryMappedView::Create(
        WindowsMemoryMappedFile const& fileHandle,
        MemoryMappedFileAccess access,
        int64_t offset,
        int64_t size)
        -> std::expected<WindowsMemoryMappedView, ErrorCode>
    {
        if (size < 0)
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        size_t const validSize = static_cast<size_t>(size);

        DWORD const dwDesiredAccess = Internal::ConvertFileMapAccess(access);
        DWORD const dwFileOffsetHigh = static_cast<DWORD>(offset >> 32);
        DWORD const dwFileOffsetLow = static_cast<DWORD>(offset);

        Interop::Win32SafeMemoryMappedViewHandle handle;
        handle.Attach(
            MapViewOfFile(
                fileHandle.GetNativeHandle().Get(),
                dwDesiredAccess,
                dwFileOffsetHigh,
                dwFileOffsetLow,
                validSize),
            validSize);

        if (handle)
        {
            return WindowsMemoryMappedView{std::move(handle)};
        }

        return std::unexpected(Internal::TranslateErrorCodeWin32(GetLastError()));
    }

    std::expected<void, ErrorCode> WindowsMemoryMappedView::Flush() const
    {
        AE_ASSERT(this->_handle);

        if (not FlushViewOfFile(this->_handle.GetData(), this->_handle.GetSize()))
        {
            return std::unexpected(Internal::TranslateErrorCodeWin32(GetLastError()));
        }

        return {};
    }

    std::expected<void, ErrorCode> WindowsMemoryMappedView::Flush(size_t offset, size_t size) const
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(offset + size <= this->_handle.GetSize());

        if (not FlushViewOfFile(static_cast<std::byte const*>(this->_handle.GetData()) + offset, size))
        {
            return std::unexpected(Internal::TranslateErrorCodeWin32(GetLastError()));
        }

        return {};
    }
}
