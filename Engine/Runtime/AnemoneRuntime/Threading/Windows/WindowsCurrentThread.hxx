#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsThread.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <timeapi.h>

#if ANEMONE_ARCHITECTURE_X64
#include <xmmintrin.h>
#endif

namespace Anemone
{
    struct WindowsCurrentThread final
    {
        static void YieldAnyThreadOnAnyProcessor()
        {
            timeBeginPeriod(1);
            SleepEx(1, FALSE);
            timeEndPeriod(1);
        }

        static void YieldAnyThreadOnSameProcessor()
        {
            SwitchToThread();
        }

        static void YieldSameOrHigherPriorityThreadOnAnyProcessor()
        {
            SleepEx(0, FALSE);
        }

        static void Yield()
        {
            if (SwitchToThread() == FALSE)
            {
                SleepEx(0, FALSE);
            }
        }

        static void Sleep(int32_t milliseconds)
        {
            SleepEx(Interop::win32_ValidateTimeoutDuration(milliseconds), TRUE);
        }

        static void Sleep(Duration const& timeout)
        {
            SleepEx(Interop::win32_ValidateTimeoutDuration(timeout), TRUE);
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
            return ThreadId{GetCurrentThreadId()};
        }
    };

    using CurrentThread = WindowsCurrentThread;
}
