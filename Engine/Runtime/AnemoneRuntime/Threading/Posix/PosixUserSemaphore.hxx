#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone
{
    class RUNTIME_API PosixUserSemaphore final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit PosixUserSemaphore(int32_t initial)
            : m_Count{initial}
        {
        }

        PosixUserSemaphore(PosixUserSemaphore const&) = delete;
        PosixUserSemaphore(PosixUserSemaphore&&) = delete;
        PosixUserSemaphore& operator=(PosixUserSemaphore const&) = delete;
        PosixUserSemaphore& operator=(PosixUserSemaphore&&) = delete;
        ~PosixUserSemaphore();

        void Release(int32_t count = 1);

        void Acquire();

        bool TryAcquire();

    private:
        void Wait();
    };

    using UserSemaphore = PosixUserSemaphore;
}
