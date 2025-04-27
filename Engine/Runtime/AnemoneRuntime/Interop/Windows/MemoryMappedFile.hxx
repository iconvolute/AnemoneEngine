#pragma once
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"

#include <span>

namespace Anemone::Interop::Windows
{
    enum class MemoryMappedFileAccess : uint8_t
    {
        ReadOnly,
        ReadWrite,
        CopyOnWrite,
        ReadExecute,
    };

    constexpr static DWORD ConvertFileMapAccess(MemoryMappedFileAccess access)
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

    constexpr DWORD ConvertPageAccess(MemoryMappedFileAccess access)
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

namespace Anemone::Interop::Windows
{
    inline auto MemoryMappedFileHandle_OpenCore(
        const wchar_t* name,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        DWORD const dwDesiredAccess = ConvertFileMapAccess(access);

        return SafeMemoryMappedFileHandle{OpenFileMappingW(dwDesiredAccess, FALSE, name)};
    }

    inline auto MemoryMappedFileHandle_CreateCore(
        Windows::SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> SafeMemoryMappedFileHandle
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        DWORD const dwPageAccess = ConvertPageAccess(access);
        DWORD const dwCapacityHigh = static_cast<DWORD>(capacity >> 32);
        DWORD const dwCapacityLow = static_cast<DWORD>(capacity);

        HANDLE const hFile = handle.Get();

        SafeMemoryMappedFileHandle result;
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

    inline auto MemoryMappedFileHandle_OpenOrCreateCore(
        SafeFileHandle const& handle,
        const wchar_t* name,
        MemoryMappedFileAccess access,
        int64_t capacity)
        -> SafeMemoryMappedFileHandle
    {
        AE_ASSERT(capacity > 0);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = FALSE,
        };

        size_t retries = 20;

        HANDLE const hFile = handle.Get();

        DWORD const dwPageAccess = ConvertPageAccess(access);
        DWORD const dwDesiredAccess = ConvertFileMapAccess(access);
        DWORD const dwCapacityHigh = static_cast<DWORD>(capacity >> 32);
        DWORD const dwCapacityLow = static_cast<DWORD>(capacity);

        SafeMemoryMappedFileHandle result;

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

    inline auto CreateMemoryMappedFile(
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_CreateCore(
            {},
            nullptr,
            access,
            capacity);
    }

    inline auto CreateMemoryMappedFile(
        SafeFileHandle const& fileHandle,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_CreateCore(
            fileHandle,
            nullptr,
            access,
            capacity);
    }

    // Named, create, fail if exists
    inline auto CreateMemoryMappedFile(
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_CreateCore(
            {},
            name,
            access,
            capacity);
    }

    inline auto CreateMemoryMappedFile(
        SafeFileHandle const& fileHandle,
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_CreateCore(
            fileHandle,
            name,
            access,
            capacity);
    }

    inline auto OpenMemoryMappedFile(
        const wchar_t* name,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_OpenCore(
            name,
            access);
    }

    inline auto OpenMemoryMappedFile(
        SafeFileHandle const& fileHandle,
        const wchar_t* name,
        int64_t capacity,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedFileHandle
    {
        return MemoryMappedFileHandle_OpenOrCreateCore(
            fileHandle,
            name,
            access,
            capacity);
    }
}

namespace Anemone::Interop::Windows
{
    inline auto CreateMemoryMappedView(
        SafeMemoryMappedFileHandle const& handle,
        MemoryMappedFileAccess access)
        -> SafeMemoryMappedViewHandle
    {
        DWORD const dwDesiredAccess = ConvertFileMapAccess(access);

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

        return SafeMemoryMappedViewHandle{data, size};
    }

    inline auto CreateMemoryMappedView(
        SafeMemoryMappedFileHandle const& handle,
        MemoryMappedFileAccess access,
        int64_t offset,
        int64_t size)
        -> SafeMemoryMappedViewHandle
    {
        AE_ASSERT(size > 0);
        size_t const validSize = static_cast<size_t>(size);

        DWORD const dwDesiredAccess = ConvertFileMapAccess(access);
        DWORD const dwFileOffsetHigh = static_cast<DWORD>(offset >> 32);
        DWORD const dwFileOffsetLow = static_cast<DWORD>(offset);

        LPVOID data = MapViewOfFile(
            handle.Get(),
            dwDesiredAccess,
            dwFileOffsetHigh,
            dwFileOffsetLow,
            validSize);

        return SafeMemoryMappedViewHandle{data, validSize};
    }

    inline auto FlushMemoryMappedView(
        SafeMemoryMappedViewHandle const& handle)
        -> bool
    {
        AE_ASSERT(handle);

        return FlushViewOfFile(handle.GetData(), handle.GetSize());
    }

    inline auto FlushMemoryMappedView(
        SafeMemoryMappedViewHandle const& handle,
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
