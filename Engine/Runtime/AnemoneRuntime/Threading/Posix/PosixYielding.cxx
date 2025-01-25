#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#if ANEMONE_ARCHITECTURE_X64
#include <xmmintrin.h>
#endif

namespace Anemone
{
    void YieldThread(ThreadYieldTarget target)
    {
        sched_yield();
        (void)target;
    }

    void YieldThread()
    {
        sched_yield();
    }

    void SleepThread(int32_t milliseconds)
    {
        usleep(static_cast<useconds_t>(milliseconds * 1'000));
    }

    void SleepThread(Duration const& timeout)
    {
        usleep(static_cast<useconds_t>(timeout.ToMicroseconds()));
    }

    void PauseThread()
    {
#if ANEMONE_ARCHITECTURE_X64
        _mm_pause();
        _mm_pause();
        _mm_pause();
        _mm_pause();
#elif ANEMONE_ARCHITECTURE_ARM64
#if ANEMONE_TOOLCHAIN_MSVC
        __wfe();
#else
        __asm__ __volatile__("wfe");
#endif
#else
        // nop
#endif
    }
}
