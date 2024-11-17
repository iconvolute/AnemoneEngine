#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone
{
    class RUNTIME_API WindowsUserSemaphore final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit WindowsUserSemaphore(int32_t initial)
            : m_Count{initial}
        {
        }

        WindowsUserSemaphore(WindowsUserSemaphore const&) = delete;
        WindowsUserSemaphore(WindowsUserSemaphore&&) = delete;
        WindowsUserSemaphore& operator=(WindowsUserSemaphore const&) = delete;
        WindowsUserSemaphore& operator=(WindowsUserSemaphore&&) = delete;
        ~WindowsUserSemaphore();

        void Release(int32_t count = 1);

        void Acquire();

        bool TryAcquire();

    private:
        void Wait();
    };

    using UserSemaphore = WindowsUserSemaphore;
}
