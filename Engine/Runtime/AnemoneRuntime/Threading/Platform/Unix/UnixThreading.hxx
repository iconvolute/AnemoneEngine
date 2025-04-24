#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

#include <sys/syscall.h>
#include <linux/futex.h>

#include <atomic>

namespace Anemone
{
    using ThreadId = pid_t;
    using ThreadHandle = Interop::UnixPthreadThreadHandle;
}

namespace Anemone::Internal
{
    using PlatformSemaphore = sem_t;
    using PlatformReaderWriterLock = pthread_rwlock_t;
    using PlatformCriticalSection = pthread_mutex_t;
    using PlatformRecursiveCriticalSection = pthread_mutex_t;
    using PlatformConditionVariable = pthread_cond_t;
}

namespace Anemone::Internal
{
    struct Futex final
    {
        static void Wait(std::atomic<int32_t>& futex, int32_t expected)
        {
            syscall(
                SYS_futex,
                /* uaddr */ &futex,
                /* futex_op */ FUTEX_WAIT_PRIVATE,
                /* val */ expected,
                /* timeout */ nullptr);
        }

        static void WaitSpurious(std::atomic<int32_t>& futex, int32_t expected)
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

        static void WakeOne(std::atomic<int32_t>& futex)
        {
            syscall(
                SYS_futex,
                /* uaddr */ &futex,
                /* futex_op */ FUTEX_WAKE_PRIVATE,
                /* val */ 1);
        }

        static void WakeMany(std::atomic<int32_t>& futex, int32_t count)
        {
#if false
            for (; count != 0; --count)
            {
                syscall(
                    SYS_futex,
                    /* uaddr */ &futex,
                    /* futex_op */ FUTEX_WAKE_PRIVATE,
                    /* val */ 1);
            }
#else
            syscall(
                SYS_futex,
                /* uaddr */ &futex,
                /* futex_op */ FUTEX_WAKE_PRIVATE,
                /* val */ count);
#endif
        }

        static void WakeAll(std::atomic<int32_t>& futex)
        {
            syscall(
                SYS_futex,
                /* uaddr */ &futex,
                /* futex_op */ FUTEX_WAKE_PRIVATE,
                /* val */ INT32_MAX);
        }
    };
}
