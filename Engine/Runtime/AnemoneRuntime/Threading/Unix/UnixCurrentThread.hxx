#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixThread.hxx"
#include "AnemoneRuntime/Duration.hxx"

#if ANEMONE_ARCHITECTURE_X64
#include <xmmintrin.h>
#endif

namespace Anemone
{
    struct UnixCurrentThread final
    {
        static void YieldAnyThreadOnAnyProcessor()
        {
            sched_yield();
        }

        static void YieldAnyThreadOnSameProcessor()
        {
            sched_yield();
        }

        static void YieldSameOrHigherPriorityThreadOnAnyProcessor()
        {
            sched_yield();
        }

        static void Yield()
        {
            sched_yield();
        }

        static void Sleep(int32_t milliseconds)
        {
            // TODO: Validate timeout value
            usleep(static_cast<useconds_t>(milliseconds * 1'000));
        }

        static void Sleep(Duration const& timeout)
        {
            // TODO: Validate timeout value
            usleep(static_cast<useconds_t>(timeout.ToMicroseconds()));
        }

        static void Pause()
        {
#if ANEMONE_ARCHITECTURE_X64
            _mm_pause();
            _mm_pause();
            _mm_pause();
            _mm_pause();
#elif ANEMONE_ARCHITECTURE_ARM64
#if ANEMONE_COMPILER_MSVC
            __wfe();
#else
            __asm__ __volatile__("wfe");
#endif
#else
            // nop
#endif
        }

        static ThreadId Id()
        {
            return ThreadId{Interop::posix_GetThreadId()};
        }
    };

    using CurrentThread = UnixCurrentThread;
}
