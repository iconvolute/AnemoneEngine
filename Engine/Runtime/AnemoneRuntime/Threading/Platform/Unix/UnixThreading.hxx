#pragma once
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"

#include <sys/syscall.h>
#include <linux/futex.h>

#include <atomic>

namespace Anemone::Internal
{
    using PlatformSemaphore = sem_t;
    using PlatformReaderWriterLock = pthread_rwlock_t;
    using PlatformCriticalSection = pthread_mutex_t;
    using PlatformRecursiveCriticalSection = pthread_mutex_t;
    using PlatformConditionVariable = pthread_cond_t;

    struct PlatformMonitor
    {
        pthread_mutex_t _lock;
        pthread_cond_t _condition;
    };
}

namespace Anemone::Internal::Futex
{
    inline void Wait(std::atomic<int32_t>& futex, int32_t expected)
    {
        syscall(
            SYS_futex,
            /* uaddr */ &futex,
            /* futex_op */ FUTEX_WAIT_PRIVATE,
            /* val */ expected,
            /* timeout */ nullptr);
    }

    inline void WaitSpurious(std::atomic<int32_t>& futex, int32_t expected)
    {
        while (true)
        {
            int const rc = syscall(
                SYS_futex,
                /* uaddr */ &futex,
                /* futex_op */ FUTEX_WAIT_PRIVATE,
                /* val */ expected,
                /* timeout */ nullptr);

            if (rc == 0)
            {
                // Wait succeeded.
                if (futex.load() != expected)
                {
                    break;
                }
            }
            else
            {
                // Wait failed.
                if (errno == EAGAIN)
                {
                    break;
                }
                else
                {
                    anemone_debugbreak();
                }
            }
        }
    }

    inline void WakeOne(std::atomic<int32_t>& futex)
    {
        syscall(
            SYS_futex,
            /* uaddr */ &futex,
            /* futex_op */ FUTEX_WAKE_PRIVATE,
            /* val */ 1);
    }

    inline void WakeMany(std::atomic<int32_t>& futex, int32_t count)
    {
        syscall(
            SYS_futex,
            /* uaddr */ &futex,
            /* futex_op */ FUTEX_WAKE_PRIVATE,
            /* val */ count);
    }

    inline void WakeAll(std::atomic<int32_t>& futex)
    {
        syscall(
            SYS_futex,
            /* uaddr */ &futex,
            /* futex_op */ FUTEX_WAKE_PRIVATE,
            /* val */ INT32_MAX);
    }
}
