#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"

namespace Anemone::Platform
{
    struct NativeIpEndPoint final
    {
        union
        {
            SOCKADDR Address;
            SOCKADDR_IN AddressV4;
            SOCKADDR_IN6 AddressV6;
        } Inner;
    };

    struct NativeLocalEndPoint final
    {
        SOCKADDR_UN Inner;
    };

    struct NativeSocket final
    {
        SOCKET Inner;
    };

    // todo: remove this
    struct NativeUnixDomainSocketEndPoint final
    {
        SOCKADDR_UN Inner;
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
