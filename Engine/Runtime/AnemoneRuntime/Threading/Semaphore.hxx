#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsNative.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixNative.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class RUNTIME_API Semaphore final
    {
    private:
        Detail::NativeSemaphore m_native;

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
