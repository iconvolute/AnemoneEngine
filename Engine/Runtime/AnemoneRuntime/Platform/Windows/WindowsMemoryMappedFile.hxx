#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

#include <span>

namespace Anemone::Interop
{
    enum class MemoryMappedFileAccess : uint8_t
    {
        ReadOnly,
        ReadWrite,
        CopyOnWrite,
        ReadExecute,
    };

    constexpr static DWORD win32_ConvertFileMapAccess(MemoryMappedFileAccess access)
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

    constexpr DWORD win32_ConvertPageAccess(MemoryMappedFileAccess access)
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

namespace Anemone::Interop
{
    inline auto win32_MemoryMappedFileHandle_OpenCore(
        const wchar_t* name,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        DWORD const dwDesiredAccess = win32_ConvertFileMapAccess(access);

        return Win32SafeMemoryMappedFileHandle{OpenFileMappingW(dwDesiredAccess, FALSE, name)};
    }

    inline auto win32_MemoryMappedFileHandle_CreateCore(
        Win32SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> Win32SafeMemoryMappedFileHandle
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        DWORD const dwPageAccess = win32_ConvertPageAccess(access);
        DWORD const dwCapacityHigh = static_cast<DWORD>(capacity >> 32);
        DWORD const dwCapacityLow = static_cast<DWORD>(capacity);

        HANDLE const hFile = handle.Get();

        Interop::Win32SafeMemoryMappedFileHandle result;
        result.Attach(CreateFileMappingW(
            hFile,
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
                return {};
            }
        }
        else
        {
            return {};
        }

        return result;
    }

    inline auto win32_MemoryMappedFileHandle_OpenOrCreateCore(
        Win32SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> Win32SafeMemoryMappedFileHandle
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        size_t retries = 20;

        HANDLE const hFile = handle.Get();

        DWORD const dwPageAccess = win32_ConvertPageAccess(access);
        DWORD const dwDesiredAccess = win32_ConvertFileMapAccess(access);
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
                return {};
            }

            result.Attach(OpenFileMappingW(dwDesiredAccess, FALSE, name));

            if (result)
            {
                break;
            }

            if (DWORD const dwError = GetLastError(); dwError != ERROR_FILE_NOT_FOUND)
            {
                return {};
            }

            --retries;
            SleepEx(100, TRUE);
        }

        return result;
    }

    inline auto win32_CreateMemoryMappedFile(
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_CreateCore(
            {},
            nullptr,
            access,
            capacity);
    }

    inline auto win32_CreateMemoryMappedFile(
        Win32SafeFileHandle const& fileHandle,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_CreateCore(
            fileHandle,
            nullptr,
            access,
            capacity);
    }

    // Named, create, fail if exists
    inline auto win32_CreateMemoryMappedFile(
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_CreateCore(
            {},
            name,
            access,
            capacity);
    }

    inline auto win32_CreateMemoryMappedFile(
        Win32SafeFileHandle const& fileHandle,
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_CreateCore(
            fileHandle,
            name,
            access,
            capacity);
    }

    inline auto win32_OpenMemoryMappedFile(
        const wchar_t* name,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_OpenCore(
            name,
            access);
    }

    inline auto win32_OpenMemoryMappedFile(
        Win32SafeFileHandle const& fileHandle,
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedFileHandle
    {
        return win32_MemoryMappedFileHandle_OpenOrCreateCore(
            fileHandle,
            name,
            access,
            capacity);
    }
}

namespace Anemone::Interop
{
    inline auto win32_CreateMemoryMappedView(
        Win32SafeMemoryMappedFileHandle const& handle,
        MemoryMappedFileAccess access)
        -> Win32SafeMemoryMappedViewHandle
    {
        DWORD const dwDesiredAccess = win32_ConvertFileMapAccess(access);

        LPVOID data = MapViewOfFile(
            handle.Get(),
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

        return Win32SafeMemoryMappedViewHandle{data, size};
    }

    inline auto win32_CreateMemoryMappedView(
        Win32SafeMemoryMappedFileHandle const& handle,
        MemoryMappedFileAccess access,
        int64_t offset,
        int64_t size)
        -> Win32SafeMemoryMappedViewHandle
    {
        AE_ASSERT(size > 0);
        size_t const validSize = static_cast<size_t>(size);

        DWORD const dwDesiredAccess = win32_ConvertFileMapAccess(access);
        DWORD const dwFileOffsetHigh = static_cast<DWORD>(offset >> 32);
        DWORD const dwFileOffsetLow = static_cast<DWORD>(offset);

        LPVOID data = MapViewOfFile(
            handle.Get(),
            dwDesiredAccess,
            dwFileOffsetHigh,
            dwFileOffsetLow,
            validSize);

        return Win32SafeMemoryMappedViewHandle{data, validSize};
    }

    inline auto win32_FlushMemoryMappedView(
        Win32SafeMemoryMappedViewHandle const& handle)
        -> bool
    {
        AE_ASSERT(handle);

        return FlushViewOfFile(handle.GetData(), handle.GetSize());
    }

    inline auto win32_FlushMemoryMappedView(
        Win32SafeMemoryMappedViewHandle const& handle,
        size_t offset,
        size_t size)
        -> bool
    {
        AE_ASSERT(handle);
        AE_ASSERT((offset + size) <= handle.GetSize());

        return FlushViewOfFile(
            static_cast<std::byte const*>(handle.GetData()) + offset,
            size);
    }
}
