#pragma once
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone
{
    class RUNTIME_API WindowsUserManualResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        WindowsUserManualResetEvent() = default;
        explicit WindowsUserManualResetEvent(bool signaled);
        WindowsUserManualResetEvent(WindowsUserManualResetEvent const&) = delete;
        WindowsUserManualResetEvent(WindowsUserManualResetEvent&&) = delete;
        WindowsUserManualResetEvent& operator=(WindowsUserManualResetEvent const&) = delete;
        WindowsUserManualResetEvent& operator=(WindowsUserManualResetEvent&&) = delete;
        ~WindowsUserManualResetEvent() = default;

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

    using UserManualResetEvent = WindowsUserManualResetEvent;
}
