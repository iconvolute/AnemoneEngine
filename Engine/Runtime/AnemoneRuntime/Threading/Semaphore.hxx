#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone::Threading
{
    class RUNTIME_API Semaphore final
    {
    private:
        Platform::NativeSemaphore m_native;

    public:
        Semaphore(int32_t initialCount, int32_t maxCount);
        Semaphore(Semaphore const&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore const&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;
        ~Semaphore();

    public:
        void Acquire();
        bool TryAcquire(Duration const& timeout);
        bool TryAcquire();
        void Release();
    };
}
