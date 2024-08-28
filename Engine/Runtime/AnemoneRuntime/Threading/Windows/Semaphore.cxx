#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    Semaphore::Semaphore(int32_t initialCount, int32_t maxCount)
    {
        AE_ASSERT(initialCount >= 0);
        AE_ASSERT(maxCount >= 1);

        Platform::NativeSemaphore& nativeThis = Platform::Create(this->_native);

        nativeThis.Inner = CreateSemaphoreW(
                nullptr,
                initialCount,
                maxCount,
                nullptr);
    }

    Semaphore::~Semaphore()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        CloseHandle(nativeThis.Inner);

        Platform::Destroy(this->_native);
    }

    void Semaphore::Acquire()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        WaitForSingleObject(
            nativeThis.Inner,
            INFINITE);
    }

    bool Semaphore::TryAcquire(Duration const& timeout)
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        return WaitForSingleObject(
                   nativeThis.Inner,
                   Platform::win32_ValidateTimeoutDuration(timeout)) != WAIT_TIMEOUT;
    }

    bool Semaphore::TryAcquire()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        return WaitForSingleObject(
                   nativeThis.Inner,
                   0) != WAIT_TIMEOUT;
    }

    void Semaphore::Release()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        ReleaseSemaphore(
            nativeThis.Inner,
            1,
            nullptr);
    }
}
