#pragma once
#include "AnemoneRuntime/Platform/Posix/Types.hxx"

namespace Anemone::Detail
{
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
        int Limit;
        pthread_mutex_t Mutex;
        pthread_cond_t Cond;
    };

    struct NativeThread final
    {
        pthread_t Handle;
    };
}
