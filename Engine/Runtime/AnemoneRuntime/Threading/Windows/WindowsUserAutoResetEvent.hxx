#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone
{
    class RUNTIME_API WindowsUserAutoResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        WindowsUserAutoResetEvent() = default;
        explicit WindowsUserAutoResetEvent(bool signaled);
        WindowsUserAutoResetEvent(WindowsUserAutoResetEvent const&) = delete;
        WindowsUserAutoResetEvent(WindowsUserAutoResetEvent&&) = delete;
        WindowsUserAutoResetEvent& operator=(WindowsUserAutoResetEvent const&) = delete;
        WindowsUserAutoResetEvent& operator=(WindowsUserAutoResetEvent&&) = delete;
        ~WindowsUserAutoResetEvent() = default;

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

    using UserAutoResetEvent = WindowsUserAutoResetEvent;
}
