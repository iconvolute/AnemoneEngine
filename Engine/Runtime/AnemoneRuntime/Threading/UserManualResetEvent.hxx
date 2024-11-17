#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if !ANEMONE_FEATURE_FUTEX
#error "Futex is required"
#else

#include <atomic>

namespace Anemone
{
    class RUNTIME_API UserManualResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        UserManualResetEvent() = default;
        explicit UserManualResetEvent(bool signaled);
        UserManualResetEvent(UserManualResetEvent const&) = delete;
        UserManualResetEvent(UserManualResetEvent&&) = delete;
        UserManualResetEvent& operator=(UserManualResetEvent const&) = delete;
        UserManualResetEvent& operator=(UserManualResetEvent&&) = delete;
        ~UserManualResetEvent() = default;

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

#endif
