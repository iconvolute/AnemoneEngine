#pragma once
#include "AnemoneRuntime.Interop/SafeHandle.hxx"
#include "AnemoneRuntime.Interop/Windows/Headers.hxx"

#include <combaseapi.h>

namespace Anemone::Interop::Windows
{
    struct SafeRegistryHandleTraits final
    {
        static HKEY Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HKEY value)
        {
            return value != nullptr;
        }

        static bool Reset(HKEY value)
        {
            return RegCloseKey(value) == ERROR_SUCCESS;
        }
    };
    using SafeRegistryHandle = SafeHandleT<HKEY, SafeRegistryHandleTraits>;

    struct SafeHandleTraits final
    {
        static HANDLE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HANDLE value)
        {
            return (value != nullptr) and (value != INVALID_HANDLE_VALUE);
        }

        static bool Reset(HANDLE value)
        {
            return CloseHandle(value);
        }
    };
    using SafeHandle = SafeHandleT<HANDLE, SafeHandleTraits>;

    struct SafeFileHandleTraits final
    {
        static HANDLE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HANDLE value)
        {
            return (value != nullptr) and (value != INVALID_HANDLE_VALUE);
        }

        static bool Reset(HANDLE value)
        {
            return CloseHandle(value);
        }
    };
    using SafeFileHandle = SafeHandleT<HANDLE, SafeFileHandleTraits>;

    struct SafeSharedLibraryHandleTraits final
    {
        static HMODULE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HMODULE value)
        {
            return value != nullptr;
        }

        static bool Reset(HMODULE value)
        {
            return FreeLibrary(value);
        }
    };
    using SafeSharedLibraryHandle = SafeHandleT<HMODULE, SafeSharedLibraryHandleTraits>;

    struct SafeFindFileHandleTraits final
    {
        static HANDLE Invalid()
        {
            return INVALID_HANDLE_VALUE;
        }

        static bool IsValid(HANDLE value)
        {
            return value != INVALID_HANDLE_VALUE;
        }

        static bool Reset(HANDLE value)
        {
            return FindClose(value);
        }
    };
    using SafeFindFileHandle = SafeHandleT<HANDLE, SafeFindFileHandleTraits>;

    struct SafeMemoryMappedFileHandleTraits final
    {
        static HANDLE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HANDLE value)
        {
            return value != nullptr;
        }

        static bool Reset(HANDLE value)
        {
            return CloseHandle(value);
        }
    };
    using SafeMemoryMappedFileHandle = SafeHandleT<HANDLE, SafeMemoryMappedFileHandleTraits>;

    struct SafeMemoryMappedViewHandleTraits final
    {
        static void* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(void* value)
        {
            return value != nullptr;
        }

        static bool Reset(void* value, size_t size)
        {
            (void)size;
            return UnmapViewOfFile(value);
        }
    };
    using SafeMemoryMappedViewHandle = SafeBufferT<void, SafeMemoryMappedViewHandleTraits>;

    struct SafeComTaskMemoryBufferTraits final
    {
        static void* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(void* value)
        {
            return value != nullptr;
        }

        static bool Reset(void* value, size_t size)
        {
            (void)size;
            CoTaskMemFree(value);
            return true;
        }
    };
    using SafeComTaskMemoryBuffer = SafeBufferT<void, SafeComTaskMemoryBufferTraits>;

    template <typename T>
    struct SafeComTaskMemoryHandleTraits final
    {
        static T* Invalid()
        {
            return nullptr;
        }
        static bool IsValid(T* value)
        {
            return value != nullptr;
        }
        static bool Reset(T* value)
        {
            CoTaskMemFree(value);
            return true;
        }
    };

    template <typename T>
    using SafeComTaskMemoryHandle = SafeHandleT<T*, SafeComTaskMemoryHandleTraits<T>>;
}
