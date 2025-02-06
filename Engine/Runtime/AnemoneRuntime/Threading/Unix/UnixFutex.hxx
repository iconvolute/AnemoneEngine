#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <atomic>

namespace Anemone::Internal
{
    struct LinuxFutexTraits final
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
                        AE_TRACE(Error, "Futex wait failed: errno={}, '{}'", errno, strerror(errno));
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
