#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"

namespace Anemone::Platform
{
    struct NativeIpAddress final
    {
        union
        {
            in_addr IPv4;
            in6_addr IPv6;
        } Address;
        uint32_t ScopeId;
        int AddressFamily;
    };

    struct NativeSocket final
    {
        SOCKET Inner;
    };

    struct NativeConditionVariable final
    {
        CONDITION_VARIABLE Inner;
    };

    struct NativeCriticalSection final
    {
        CRITICAL_SECTION Inner;
    };

    struct NativeReaderWriterLock final
    {
        SRWLOCK Inner;
    };

    struct NativeSemaphore final
    {
        HANDLE Inner;
    };

    struct NativeThread final
    {
        HANDLE Handle;
        DWORD Id;
    };

        struct NativeSharedLibrary final
    {
        HMODULE Handle;
    };

    struct NativeProcess final
    {
        HANDLE Handle;
        DWORD Id;
        bool Owned;
    };

    struct NativeNamedMutex final
    {
        HANDLE Handle;
    };

    struct NativeFileHandle final
    {
        HANDLE Handle;
    };

    struct NativeAsyncFileHandle final
    {
        HANDLE Handle;
    };

    struct NativeDirectoryEnumerator final
    {
        HANDLE Handle;
    };

    struct NativeMemoryMappedFileHandle final
    {
        HANDLE Handle;
    };
}
