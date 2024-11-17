#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if !ANEMONE_FEATURE_FUTEX
#error "Futex is required"
#else


#include <atomic>

namespace Anemone
{
    class RUNTIME_API UserAutoResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        UserAutoResetEvent() = default;
        explicit UserAutoResetEvent(bool signaled);
        UserAutoResetEvent(UserAutoResetEvent const&) = delete;
        UserAutoResetEvent(UserAutoResetEvent&&) = delete;
        UserAutoResetEvent& operator=(UserAutoResetEvent const&) = delete;
        UserAutoResetEvent& operator=(UserAutoResetEvent&&) = delete;
        ~UserAutoResetEvent() = default;

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

#endif
