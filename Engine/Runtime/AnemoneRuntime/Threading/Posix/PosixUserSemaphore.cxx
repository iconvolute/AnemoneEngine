#include "AnemoneRuntime/Threading/UserSemaphore.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    UserSemaphore::~UserSemaphore()
    {
        AE_ASSERT(this->m_Count.load() == 0);
        AE_ASSERT(this->m_Waiting.load() == 0);
    }

    void UserSemaphore::Release(int32_t count)
    {
        if (count == 0)
        {
            return;
        }

        AE_ASSERT(count > 0);

        [[maybe_unused]] int32_t const previous = this->m_Count.fetch_add(count);
        AE_ASSERT(previous >= 0);

        int32_t const waiting = this->m_Waiting.load();

        if (waiting == 0)
        {
            // No waiting threads.
        }
        else if (waiting <= count)
        {
            // Releasing more threads than waiters.
            Internal::FutexApi::WakeAll(this->m_Count);
        }
        else
        {
            // Releasing fewer threads than waiters.
            Internal::FutexApi::WakeMany(this->m_Count, count);
        }
    }

    void UserSemaphore::Acquire()
    {
        int32_t current = this->m_Count.load(std::memory_order::relaxed);

        while (true)
        {
            while (current == 0)
            {
                this->Wait();

                current = this->m_Count.load(std::memory_order::relaxed);
            }

            AE_ASSERT(current > 0);

            if (this->m_Count.compare_exchange_weak(current, current - 1))
            {
                return;
            }
        }
    }

    bool UserSemaphore::TryAcquire()
    {
        int32_t current = this->m_Count.load();

        if (current == 0)
        {
            return false;
        }

        AE_ASSERT(current > 0);
        return this->m_Count.compare_exchange_weak(current, current - 1);
    }

    void UserSemaphore::Wait()
    {
        // Notify other threads that we are waiting.
        this->m_Waiting.fetch_add(1);

        // Try to acquire the semaphore again.
        int32_t current = this->m_Count.load();

        if (current == 0)
        {
            // Wait for the semaphore to be released.
            Internal::FutexApi::Wait(this->m_Count, 0);
        }

        // Release waiting counter.
        this->m_Waiting.fetch_sub(1);
    }
}
