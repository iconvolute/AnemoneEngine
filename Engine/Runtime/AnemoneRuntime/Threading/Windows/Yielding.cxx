#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <utility>

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <timeapi.h>

ANEMONE_EXTERNAL_HEADERS_END

// DDDDD

#include "AnemoneRuntime/Profiler/Profiler.hxx"

AE_DECLARE_PROFILE(YieldAnyThreadOnAnyProcessor);
AE_DECLARE_PROFILE(YieldSameOrHigherPriorityOnAnyProcessor);
AE_DECLARE_PROFILE(YieldAnyThreadOnSameProcessor);

// DDDDD

namespace Anemone::Threading::ThisThread
{
    void Yield(ThreadYieldTarget target)
    {
        switch (target)
        {
        case ThreadYieldTarget::None:
            {
                break;
            }

        case ThreadYieldTarget::AnyThreadOnAnyProcessor:
            {
                AE_PROFILE_SCOPE(YieldAnyThreadOnAnyProcessor);
                timeBeginPeriod(1);
                SleepEx(1, FALSE);
                timeEndPeriod(1);
                break;
            }
        case ThreadYieldTarget::SameOrHigherPriorityOnAnyProcessor:
            {
                AE_PROFILE_SCOPE(YieldSameOrHigherPriorityOnAnyProcessor);
                SleepEx(0, FALSE);
                break;
            }

        case ThreadYieldTarget::AnyThreadOnSameProcessor:
            {
                AE_PROFILE_SCOPE(YieldAnyThreadOnSameProcessor);
                SwitchToThread();
                break;
            }

        default: // NOLINT(clang-diagnostic-covered-switch-default)
            {
                std::unreachable();
            }
        }
    }

    void Yield()
    {
        if (SwitchToThread() == FALSE)
        {
            SleepEx(0, FALSE);
        }
    }

    void Sleep(int32_t milliseconds)
    {
        SleepEx(Platform::win32_ValidateTimeoutDuration(milliseconds), TRUE);
    }

    void Sleep(Duration const& timeout)
    {
        SleepEx(Platform::win32_ValidateTimeoutDuration(timeout), TRUE);
    }

    void Pause()
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
