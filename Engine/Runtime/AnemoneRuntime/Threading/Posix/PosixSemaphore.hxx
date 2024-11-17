#pragma once
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <atomic>

namespace Anemone
{
    class RUNTIME_API PosixSemaphore final
    {
    private:
        std::atomic<int> m_current;
        int m_max;
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;

    public:
        PosixSemaphore(int32_t initialCount, int32_t maxCount);
        PosixSemaphore(PosixSemaphore const&) = delete;
        PosixSemaphore(PosixSemaphore&&) = delete;
        PosixSemaphore& operator=(PosixSemaphore const&) = delete;
        PosixSemaphore& operator=(PosixSemaphore&&) = delete;
        ~PosixSemaphore();

    public:
        void Acquire();
        bool TryAcquire(Duration const& timeout);
        bool TryAcquire();
        void Release();
    };

    using Semaphore = PosixSemaphore;
}
