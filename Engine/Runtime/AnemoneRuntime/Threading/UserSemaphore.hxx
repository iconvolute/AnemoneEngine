#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone
{
    class RUNTIME_API UserSemaphore final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit UserSemaphore(int32_t initial)
            : m_Count{initial}
        {
        }

        UserSemaphore(UserSemaphore const&) = delete;
        UserSemaphore(UserSemaphore&&) = delete;
        UserSemaphore& operator=(UserSemaphore const&) = delete;
        UserSemaphore& operator=(UserSemaphore&&) = delete;
        ~UserSemaphore();

        void Release(int32_t count = 1);

        void Acquire();

        bool TryAcquire();

    private:
        void Wait();
    };
}
