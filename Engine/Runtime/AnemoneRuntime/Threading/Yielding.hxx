#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone
{
    class SpinWait final
    {
    private:
        std::uint32_t m_Counter{};

        static constexpr std::uint32_t SpinYieldThreshold = 10;

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

        RUNTIME_API void Spin();

        bool NextSpinWillYield() const
        {
            return this->m_Counter >= SpinYieldThreshold;
        }
    };
}

namespace Anemone
{
    enum class ThreadYieldTarget
    {
        None,
        AnyThreadOnAnyProcessor,
        AnyThreadOnSameProcessor,
        SameOrHigherPriorityOnAnyProcessor,
    };

    RUNTIME_API void YieldThread(ThreadYieldTarget target);

    RUNTIME_API void YieldThread();

    RUNTIME_API void SleepThread(int32_t milliseconds);

    RUNTIME_API void SleepThread(Duration const& timeout);

    RUNTIME_API void PauseThread();

    inline void SpinThread(size_t spins)
    {
        SpinWait spinner{};

        while (spins != 0)
        {
            spinner.Spin();
            --spins;
        }
    }

    inline constexpr size_t YieldThreadThreshold = 0xFF;

    template <typename PredicateT = bool()>
    anemone_forceinline void WaitForCompletion(PredicateT predicate)
    {
        SpinWait spinner{};

        while (!predicate())
        {
            spinner.Spin();
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
                spinner.Spin();
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
                spinner.Spin();
            }
        }

        return false;
    }
}
