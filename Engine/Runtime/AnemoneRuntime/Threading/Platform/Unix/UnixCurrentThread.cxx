#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Interop/Linux/Process.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

#include "AnemoneRuntime/Threading/CurrentThread.hxx"

#if ANEMONE_ARCHITECTURE_X64
#include <xmmintrin.h>
#endif

namespace Anemone
{
    void CurrentThread::YieldAnyThreadOnAnyProcessor()
    {
        sched_yield();
    }

    void CurrentThread::YieldAnyThreadOnSameProcessor()
    {
        sched_yield();
    }

    void CurrentThread::YieldSameOrHigherPriorityThreadOnAnyProcessor()
    {
        sched_yield();
    }

    void CurrentThread::Yield()
    {
        sched_yield();
    }

    void CurrentThread::Sleep(int32_t milliseconds)
    {
        // TODO: Validate timeout value
        usleep(static_cast<useconds_t>(milliseconds * 1'000));
    }

    void CurrentThread::Sleep(Duration const& timeout)
    {
        // TODO: Validate timeout value
        usleep(static_cast<useconds_t>(timeout.ToMicroseconds()));
    }

    void CurrentThread::Pause()
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

    ThreadId CurrentThread::Id()
    {
        return ThreadId{static_cast<uintptr_t>(Interop::Linux::GetThreadId())};
    }
}

#endif
