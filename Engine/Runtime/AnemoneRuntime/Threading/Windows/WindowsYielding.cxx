#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <utility>

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <timeapi.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone
{
    void YieldThread(ThreadYieldTarget target)
    {
        switch (target)
        {
        case ThreadYieldTarget::None:
            {
                break;
            }

        case ThreadYieldTarget::AnyThreadOnAnyProcessor:
            {
                timeBeginPeriod(1);
                SleepEx(1, FALSE);
                timeEndPeriod(1);
                break;
            }
        case ThreadYieldTarget::SameOrHigherPriorityOnAnyProcessor:
            {
                SleepEx(0, FALSE);
                break;
            }

        case ThreadYieldTarget::AnyThreadOnSameProcessor:
            {
                SwitchToThread();
                break;
            }

        default: // NOLINT(clang-diagnostic-covered-switch-default)
            {
                std::unreachable();
            }
        }
    }

    void YieldThread()
    {
        if (SwitchToThread() == FALSE)
        {
            SleepEx(0, FALSE);
        }
    }

    void SleepThread(int32_t milliseconds)
    {
        SleepEx(Interop::win32_ValidateTimeoutDuration(milliseconds), TRUE);
    }

    void SleepThread(Duration const& timeout)
    {
        SleepEx(Interop::win32_ValidateTimeoutDuration(timeout), TRUE);
    }

    void PauseThread()
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
}
