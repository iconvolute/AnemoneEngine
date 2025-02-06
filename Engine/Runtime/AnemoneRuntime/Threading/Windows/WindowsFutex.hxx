#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

#include <atomic>

namespace Anemone::Internal
{
    struct WindowsFutexTraits final
    {
        static void Wait(std::atomic<int32_t>& futex, int32_t expected)
        {
            WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);
        }

        static void WaitSpurious(std::atomic<int32_t>& futex, int32_t expected)
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

        static void WakeOne(std::atomic<int32_t>& futex)
        {
            WakeByAddressSingle(&futex);
        }

        static void WakeMany(std::atomic<int32_t>& futex, int32_t count)
        {
            for (; count != 0; --count)
            {
                WakeByAddressSingle(&futex);
            }
        }

        static void WakeAll(std::atomic<int32_t>& futex)
        {
            WakeByAddressAll(&futex);
        }
    };
}
