#include "AnemoneRuntime/Threading/Windows/WindowsSemaphore.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone
{
    WindowsSemaphore::WindowsSemaphore(int32_t initialCount, int32_t maxCount)
    {
        AE_ASSERT(initialCount >= 0);
        AE_ASSERT(maxCount >= 1);

        this->m_native = CreateSemaphoreW(
            nullptr,
            initialCount,
            maxCount,
            nullptr);
    }

    WindowsSemaphore::~WindowsSemaphore()
    {
        CloseHandle(this->m_native);
    }

    void WindowsSemaphore::Acquire()
    {
        WaitForSingleObject(
            this->m_native,
            INFINITE);
    }

    bool WindowsSemaphore::TryAcquire(Duration const& timeout)
    {
        return WaitForSingleObject(
                   this->m_native,
                   Platform::win32_ValidateTimeoutDuration(timeout)) != WAIT_TIMEOUT;
    }

    bool WindowsSemaphore::TryAcquire()
    {
        return WaitForSingleObject(
                   this->m_native,
                   0) != WAIT_TIMEOUT;
    }

    void WindowsSemaphore::Release()
    {
        ReleaseSemaphore(
            this->m_native,
            1,
            nullptr);
    }
}
