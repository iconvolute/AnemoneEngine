#include "AnemoneRuntime/Threading/Semaphore.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    Semaphore::Semaphore(int32_t initial)
    {
        AE_ASSERT(initial >= 0);

        this->_inner.Attach(CreateSemaphoreW(
            nullptr,
            initial,
            LONG_MAX,
            nullptr));
    }

    Semaphore::~Semaphore() = default;

    void Semaphore::Acquire()
    {
        WaitForSingleObject(
            this->_inner.Get(),
            INFINITE);
    }

    bool Semaphore::TryAcquire(Duration const& timeout)
    {
        return WaitForSingleObject(
                   this->_inner.Get(),
                   Interop::win32_ValidateTimeoutDuration(timeout)) != WAIT_TIMEOUT;
    }

    bool Semaphore::TryAcquire()
    {
        return WaitForSingleObject(
                   this->_inner.Get(),
                   0) != WAIT_TIMEOUT;
    }

    void Semaphore::Release(int32_t count)
    {
        if (count == 0)
        {
            return;
        }

        AE_ASSERT(count > 0);

        ReleaseSemaphore(
            this->_inner.Get(),
            count,
            nullptr);
    }
}

#endif
