#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone::Threading
{
    class RUNTIME_API AutoResetEventSlim final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        AutoResetEventSlim() = default;
        explicit AutoResetEventSlim(bool signaled);
        AutoResetEventSlim(AutoResetEventSlim const&) = delete;
        AutoResetEventSlim(AutoResetEventSlim&&) = delete;
        AutoResetEventSlim& operator=(AutoResetEventSlim const&) = delete;
        AutoResetEventSlim& operator=(AutoResetEventSlim&&) = delete;
        ~AutoResetEventSlim() = default;

    private:
        bool TryAcquire()
        {
            return this->Reset();
        }

    public:
        bool Reset();
        void Set();
        bool IsSignaled() const;
        void Wait();
    };
}
