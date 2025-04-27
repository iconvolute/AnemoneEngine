#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

// TODO: This will be replaced with proper solution

namespace Anemone::Interop
{
    struct NativeSocketEndPoint final
    {
        sockaddr_storage Inner;
    };

    struct NativeIpEndPoint final
    {
        union
        {
            sockaddr Header;
            sockaddr_in V4;
            sockaddr_in6 V6;
        } Address;
    };

    struct NativeSocket final
    {
        SOCKET Inner;
    };


    struct NativeLocalEndPoint final
    {
        SOCKADDR_UN Inner;
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
