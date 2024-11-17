#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone
{
    class RUNTIME_API PosixUserManualResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        PosixUserManualResetEvent() = default;
        explicit PosixUserManualResetEvent(bool signaled);
        PosixUserManualResetEvent(PosixUserManualResetEvent const&) = delete;
        PosixUserManualResetEvent(PosixUserManualResetEvent&&) = delete;
        PosixUserManualResetEvent& operator=(PosixUserManualResetEvent const&) = delete;
        PosixUserManualResetEvent& operator=(PosixUserManualResetEvent&&) = delete;
        ~PosixUserManualResetEvent() = default;

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

    using UserManualResetEvent = PosixUserManualResetEvent;
}
