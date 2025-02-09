#pragma once
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone::Interop
{
    struct Win32SafeRegistryHandleTraits final
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
    using Win32SafeRegistryHandle = base_SafeHandle<HKEY, Win32SafeRegistryHandleTraits>;

    struct Win32SafeHandleTraits final
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
    using Win32SafeHandle = base_SafeHandle<HANDLE, Win32SafeHandleTraits>;

    struct Win32SafeFileHandleTraits final
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
    using Win32SafeFileHandle = base_SafeHandle<HANDLE, Win32SafeFileHandleTraits>;

    struct Win32SafeSharedLibraryHandleTraits final
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
    using Win32SafeSharedLibraryHandle = base_SafeHandle<HMODULE, Win32SafeSharedLibraryHandleTraits>;

    struct Win32SafeFindFileHandleTraits final
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
    using Win32SafeFindFileHandle = base_SafeHandle<HANDLE, Win32SafeFindFileHandleTraits>;
}
