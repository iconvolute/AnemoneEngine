#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone
{
    class RUNTIME_API PosixUserAutoResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        PosixUserAutoResetEvent() = default;
        explicit PosixUserAutoResetEvent(bool signaled);
        PosixUserAutoResetEvent(PosixUserAutoResetEvent const&) = delete;
        PosixUserAutoResetEvent(PosixUserAutoResetEvent&&) = delete;
        PosixUserAutoResetEvent& operator=(PosixUserAutoResetEvent const&) = delete;
        PosixUserAutoResetEvent& operator=(PosixUserAutoResetEvent&&) = delete;
        ~PosixUserAutoResetEvent() = default;

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

    using UserAutoResetEvent = PosixUserAutoResetEvent;
}
