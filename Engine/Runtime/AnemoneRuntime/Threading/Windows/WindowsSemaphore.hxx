#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone
{
    class RUNTIME_API WindowsSemaphore final
    {
    private:
        HANDLE m_native;

    public:
        WindowsSemaphore(int32_t initialCount, int32_t maxCount);
        WindowsSemaphore(WindowsSemaphore const&) = delete;
        WindowsSemaphore(WindowsSemaphore&&) = delete;
        WindowsSemaphore& operator=(WindowsSemaphore const&) = delete;
        WindowsSemaphore& operator=(WindowsSemaphore&&) = delete;
        ~WindowsSemaphore();

    public:
        void Acquire();
        bool TryAcquire(Duration const& timeout);
        bool TryAcquire();
        void Release();
    };

    using Semaphore = WindowsSemaphore;
}
