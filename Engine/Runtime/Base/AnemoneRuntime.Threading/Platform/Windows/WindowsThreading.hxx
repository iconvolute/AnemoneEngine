#pragma once
#include "AnemoneRuntime.Interop/Windows/SafeHandle.hxx"

#include <atomic>

namespace Anemone::Internal
{
    using PlatformSemaphore = Interop::Windows::SafeHandle;
    using PlatformReaderWriterLock = SRWLOCK;
    using PlatformCriticalSection = SRWLOCK;
    using PlatformRecursiveCriticalSection = CRITICAL_SECTION;
    using PlatformConditionVariable = CONDITION_VARIABLE;

    struct PlatformMonitor
    {
        SRWLOCK _lock;
        CONDITION_VARIABLE _condition;
    };
}

namespace Anemone::Internal::Futex
{
    inline void Wait(std::atomic<int32_t>& futex, int32_t expected)
    {
        WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);
    }

    inline void WaitSpurious(std::atomic<int32_t>& futex, int32_t expected)
    {
        while (true)
        {
            WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);

            if (futex.load() != expected)
            {
                break;
            }
        }
    }

    inline void WakeOne(std::atomic<int32_t>& futex)
    {
        WakeByAddressSingle(&futex);
    }

    inline void WakeMany(std::atomic<int32_t>& futex, int32_t count)
    {
        for (; count != 0; --count)
        {
            WakeByAddressSingle(&futex);
        }
    }

    inline void WakeAll(std::atomic<int32_t>& futex)
    {
        WakeByAddressAll(&futex);
    }
}
