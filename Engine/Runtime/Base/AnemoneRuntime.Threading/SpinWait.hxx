#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <atomic>

namespace Anemone
{
    class SpinWait final
    {
    private:
        uint32_t m_Counter{};

        static constexpr uint32_t SpinYieldThreshold = 10;

    public:
        SpinWait() = default;
        SpinWait(SpinWait const&) = delete;
        SpinWait(SpinWait&&) = delete;
        SpinWait& operator=(SpinWait const&) = delete;
        SpinWait& operator=(SpinWait&&) = delete;
        ~SpinWait() = default;

    public:
        void Reset()
        {
            this->m_Counter = 0;
        }

        ANEMONE_RUNTIME_BASE_API void SpinOnce();

        bool NextSpinWillYield() const
        {
            return this->m_Counter >= SpinYieldThreshold;
        }
    };
}

namespace Anemone
{
    anemone_forceinline void SpinThread(size_t spins)
    {
        SpinWait spinner{};

        while (spins != 0)
        {
            spinner.SpinOnce();
            --spins;
        }
    }

    inline constexpr size_t YieldThreadThreshold = 0xFF;

    anemone_forceinline void WaitForCompletion(std::atomic_flag const& flag)
    {
        SpinWait spinner{};

        while (flag.test(std::memory_order::acquire))
        {
            spinner.SpinOnce();
        }
    }

    anemone_forceinline void WaitForCompletion(std::atomic<bool> const& value)
    {
        SpinWait spinner{};

        while (not value.load(std::memory_order::acquire))
        {
            spinner.SpinOnce();
        }
    }

    template <typename PredicateT = bool()>
    anemone_forceinline void WaitForCompletion(PredicateT predicate)
    {
        SpinWait spinner{};

        while (!predicate())
        {
            spinner.SpinOnce();
        }
    }

    template <typename PredicateT = bool(), typename ActionT = bool()>
    anemone_forceinline void WaitForCompletion(PredicateT predicate, ActionT action)
    {
        SpinWait spinner{};

        while (!predicate())
        {
            if (action())
            {
                // Restart spinning here.
                spinner.Reset();
            }
            else
            {
                spinner.SpinOnce();
            }
        }
    }

    template <typename PredicateT = bool()>
    anemone_forceinline bool TryWaitForCompletion(PredicateT predicate, size_t spins)
    {
        SpinWait spinner{};

        for (size_t i = 0; i < spins; ++i)
        {
            if (predicate())
            {
                return true;
            }
            else
            {
                spinner.SpinOnce();
            }
        }

        return false;
    }
}
