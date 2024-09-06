#include "AnemoneRuntime/Threading/Futex.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    void FutexWait(std::atomic<int>& futex, int expected)
    {
        // Handle spurious wakes by checking the value after the wait.

        while (true)
        {
            WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);

            if (futex.load() != expected)
            {
                break;
            }
        }
    }

    bool FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        DWORD dwTimeout = Platform::win32_ValidateTimeoutDuration(timeout);

        UINT64 start;
        QueryUnbiasedInterruptTime(&start);

        UINT64 elapsed = 0;

        while (true)
        {
            if (elapsed >= dwTimeout)
            {
                return false;
            }

            DWORD const dwPartialTimeout = static_cast<DWORD>(dwTimeout - elapsed);

            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwPartialTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    return true;
                }
            }

            UINT64 current;
            QueryUnbiasedInterruptTime(&current);

            elapsed = (current - start) / 10'000;
        }
    }

    bool FutexWait(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        // Handle spurious wakes by checking the value after the wait.

        DWORD const dwTimeout = Platform::win32_ValidateTimeoutDuration(timeout);

        while (true)
        {
            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    // Wait timed out
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    // Value changed before timeout.
                    return true;
                }
            }
        }
    }

    void FutexWakeOne(std::atomic<int>& futex)
    {
        WakeByAddressSingle(&futex);
    }

    void FutexWakeAll(std::atomic<int>& futex)
    {
        WakeByAddressAll(&futex);
    }
}
