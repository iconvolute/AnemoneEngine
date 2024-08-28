#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone::Threading
{
    class RUNTIME_API ManualResetEventSlim final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        ManualResetEventSlim() = default;
        explicit ManualResetEventSlim(bool signaled);
        ManualResetEventSlim(ManualResetEventSlim const&) = delete;
        ManualResetEventSlim(ManualResetEventSlim&&) = delete;
        ManualResetEventSlim& operator=(ManualResetEventSlim const&) = delete;
        ManualResetEventSlim& operator=(ManualResetEventSlim&&) = delete;
        ~ManualResetEventSlim() = default;

    private:
        bool TryAcquire() const
        {
            return this->IsSignaled();
        }

    public:
        bool Reset();
        void Set();
        bool IsSignaled() const;
        void Wait();
    };
}
