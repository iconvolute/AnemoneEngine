#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone::Threading
{
    RUNTIME_API void FutexWait(std::atomic<int>& futex, int expected);

    RUNTIME_API bool FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout);

    RUNTIME_API void FutexWakeOne(std::atomic<int>& futex);

    RUNTIME_API void FutexWakeAll(std::atomic<int>& futex);

    template <typename PredicateT>
    inline void FutexWaitCondition(std::atomic<int>& futex, PredicateT&& predicate)
    {
        int value = futex.load();

        while (not std::forward<PredicateT>(predicate)(value))
        {
            FutexWait(futex, value);
            value = futex.load();
        }
    }
}
