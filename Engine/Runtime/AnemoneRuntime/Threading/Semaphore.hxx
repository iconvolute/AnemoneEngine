#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone::Platform
{
    struct NativeSemaphore;

#if ANEMONE_PLATFORM_WINDOWS
    using NativeSemaphoreStorage = UninitializedStorage<NativeSemaphore, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeSemaphoreStorage = UninitializedStorage<NativeSemaphore, 104, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeSemaphoreStorage = UninitializedStorage<NativeSemaphore, 96, 4>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Threading
{
    class RUNTIME_API Semaphore final
    {
    private:
        Platform::NativeSemaphoreStorage _native;

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
