#include "AnemoneRuntime/Threading/CurrentThread.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThreading.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <timeapi.h>

#if ANEMONE_ARCHITECTURE_X64
#include <xmmintrin.h>
#endif

namespace Anemone
{
    void CurrentThread::YieldAnyThreadOnAnyProcessor()
    {
        if (timeBeginPeriod(1) == TIMERR_NOERROR)
        {
            SleepEx(1, FALSE);
            (void)timeEndPeriod(1);
        }
    }

    void CurrentThread::YieldAnyThreadOnSameProcessor()
    {
        SwitchToThread();
    }

    void CurrentThread::YieldSameOrHigherPriorityThreadOnAnyProcessor()
    {
        SleepEx(0, FALSE);
    }

    void CurrentThread::Yield()
    {
        if (SwitchToThread() == FALSE)
        {
            SleepEx(0, FALSE);
        }
    }

    void CurrentThread::Sleep(int32_t milliseconds)
    {
        SleepEx(Interop::win32_ValidateTimeoutDuration(milliseconds), TRUE);
    }

    void CurrentThread::Sleep(Duration const& timeout)
    {
        SleepEx(Interop::win32_ValidateTimeoutDuration(timeout), TRUE);
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
        return ThreadId{GetCurrentThreadId()};
    }
}

#endif
