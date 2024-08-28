#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone::Threading
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

namespace Anemone::Threading
{
    enum class ThreadYieldTarget
    {
        None,
        AnyThreadOnAnyProcessor,
        AnyThreadOnSameProcessor,
        SameOrHigherPriorityOnAnyProcessor,
    };

    namespace ThisThread
    {
        RUNTIME_API void Yield(ThreadYieldTarget target);

        RUNTIME_API void Yield();

        RUNTIME_API void Sleep(int32_t milliseconds);

        RUNTIME_API void Sleep(Duration const& timeout);

        RUNTIME_API void Pause();

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

        template <typename PredicateT = bool()>
        [[deprecated("Do not use")]]
        anemone_forceinline bool TryWaitForCompletion(
            PredicateT predicate,
            size_t spins,
            size_t yields,
            ThreadYieldTarget target)
        {
            for (size_t i = 0; i < yields; ++i)
            {
                for (size_t j = 0; j < spins; ++j)
                {
                    if (predicate())
                    {
                        // Wait was successful.
                        return true;
                    }
                    else
                    {
                        // Pause CPU for some time.
                        Pause();
                    }
                }

                // Otherwise, yield.
                Yield(target);
            }

            return false;
        }
    }
}

