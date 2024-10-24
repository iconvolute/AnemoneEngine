#pragma once
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"

#include <sys/socket.h>
#include <sys/un.h>

#include <atomic>

namespace Anemone::Platform
{
    struct NativeIpEndPoint final
    {
        union
        {
            sockaddr Header;
            sockaddr_in V4;
            sockaddr_in6 V6;
        } Address;
    };

    struct NativeLocalEndPoint final
    {
        sockaddr_un Inner;
    };

    struct NativeSocket final
    {
        int Inner;
    };

    struct NativeConditionVariable final
    {
        pthread_cond_t Inner;
    };

    struct NativeCriticalSection final
    {
        pthread_mutex_t Inner;
    };

    struct NativeReaderWriterLock final
    {
        pthread_rwlock_t Inner;
    };

    struct NativeSemaphore final
    {
        std::atomic<int> Current;
        int Max;
        pthread_mutex_t Mutex;
        pthread_cond_t Cond;
    };

    struct NativeThread final
    {
        pthread_t Inner;
    };

    struct NativeSharedLibrary final
    {
        void* Handle;
    };

    struct NativeProcess final
    {
        pid_t Pid;
    };

    struct NativeNamedMutex final
    {
        sem_t* Handle;
    };

    struct NativeFileHandle final
    {
        int Descriptor;
    };

    struct NativeAsyncFileHandle final
    {
        int Descriptor;
    };

    struct NativeDirectoryEnumerator final
    {
        DIR* Handle;
    };

    struct NativeMemoryMappedFileHandle final
    {
        int Descriptor;
    };
}
