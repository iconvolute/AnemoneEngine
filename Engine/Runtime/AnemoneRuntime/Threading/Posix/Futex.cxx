#include "AnemoneRuntime/Threading/Futex.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

// https://www.remlab.net/op/futex-condvar.shtml
// https://www.remlab.net/op/futex-misc.shtml

namespace Anemone::Threading
{
    void FutexWait(std::atomic<int>& futex, int expected)
    {
        while (true)
        {
            int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, expected, nullptr, nullptr, 0);

            if (rc == -1)
            {
                if (errno == EAGAIN)
                {
                    return;
                }
                else
                {
                    AE_BUGCHECK("FutexWait (rc: {}, '{}')", errno, strerror(errno));
                }
            }
            else if (rc == 0)
            {
                if (futex.load() != expected)
                {
                    return;
                }
            }
        }
    }

    bool FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        timespec tsTimeout = Platform::posix_FromDuration(timeout);
        timespec tsStart{};
        clock_gettime(CLOCK_MONOTONIC, &tsStart);

        timespec tsElapsed{};

        while (true)
        {
            if (not Platform::posix_CompareGreaterEqual(tsElapsed, tsTimeout))
            {
                return false;
            }

            timespec tsPartialTimeout = Platform::posix_TimespecDifference(tsTimeout, tsElapsed);

            int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, expected, &tsPartialTimeout, nullptr, 0);

            if (rc != 0)
            {
                int const error = errno;

                if (error == ETIMEDOUT)
                {
                    return false;
                }

                if (error != EAGAIN)
                {
                    AE_BUGCHECK("FutexWaitTimeout (rc: {}, '{}')", error, strerror(error));
                }
            }

            timespec tsCurrent{};
            clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

            tsElapsed = Platform::posix_TimespecDifference(tsCurrent, tsStart);

            if (futex.load() != expected)
            {
                return true;
            }
        }
    }

    void FutexWakeOne(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_BUGCHECK("FutexWakeOne (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    void FutexWakeAll(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT32_MAX, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_BUGCHECK("FutexWakeAll (rc: {}, '{}')", errno, strerror(errno));
        }
    }
}
