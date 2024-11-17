#include "AnemoneRuntime/Threading/UserSemaphore.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone
{
    UserSemaphore::~UserSemaphore()
    {
        AE_ASSERT(this->m_Count.load() == 0);
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
        else if (waiting < count)
        {
            // Releasing more threads than waiters.
            Platform::posix_FutexWakeAll(this->m_Count);
        }
        else
        {
            // Releasing fewer threads than waiters.
            for (int32_t i = 0; i < count; ++i)
            {
                Platform::posix_FutexWakeOne(this->m_Count);
            }
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
        this->m_Waiting.fetch_add(1);

        int32_t current = this->m_Count.load(std::memory_order::relaxed);

        if (current == 0)
        {
            Platform::posix_FutexWait(this->m_Count, 0);
        }

        this->m_Waiting.fetch_sub(1, std ::memory_order::relaxed);
    }
}
