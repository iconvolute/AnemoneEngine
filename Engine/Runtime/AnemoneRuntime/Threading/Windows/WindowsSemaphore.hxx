#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    class WindowsSemaphore final
    {
    private:
        Interop::Win32SafeHandle _handle;

    public:
        WindowsSemaphore(int32_t initial)
        {
            AE_ASSERT(initial >= 0);

            this->_handle.Attach(CreateSemaphoreW(
                nullptr,
                initial,
                LONG_MAX,
                nullptr));
        }

        WindowsSemaphore(WindowsSemaphore const&) = delete;
        WindowsSemaphore(WindowsSemaphore&&) = delete;
        WindowsSemaphore& operator=(WindowsSemaphore const&) = delete;
        WindowsSemaphore& operator=(WindowsSemaphore&&) = delete;

        ~WindowsSemaphore() = default;

    public:
        void Acquire()
        {
            WaitForSingleObject(
                this->_handle.Get(),
                INFINITE);
        }

        bool TryAcquire(Duration const& timeout)
        {
            return WaitForSingleObject(
                       this->_handle.Get(),
                       Interop::win32_ValidateTimeoutDuration(timeout)) != WAIT_TIMEOUT;
        }

        bool TryAcquire()
        {
            return WaitForSingleObject(
                       this->_handle.Get(),
                       0) != WAIT_TIMEOUT;
        }

        void Release(int32_t count = 1)
        {
            if (count == 0)
            {
                return;
            }

            AE_ASSERT(count > 0);

            ReleaseSemaphore(
                this->_handle.Get(),
                count,
                nullptr);
        }
    };

    using Semaphore = WindowsSemaphore;
}
