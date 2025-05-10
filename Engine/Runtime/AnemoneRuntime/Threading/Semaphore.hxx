#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Platform/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class RUNTIME_API Semaphore final
    {
    private:
        Internal::PlatformSemaphore _inner;

    public:
        Semaphore(int32_t initial);
        Semaphore(Semaphore const&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore const&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;
        ~Semaphore();

    public:
        void Acquire();

        bool TryAcquire(Duration const& timeout);

        bool TryAcquire();

        void Release(int32_t count = 1);
    };
}
