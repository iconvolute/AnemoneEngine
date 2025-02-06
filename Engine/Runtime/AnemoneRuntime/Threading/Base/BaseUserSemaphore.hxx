#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone
{
    template <typename FutexTraits>
    class BaseUserSemaphore final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit BaseUserSemaphore(int32_t initial)
            : m_Count{initial}
        {
        }

        BaseUserSemaphore(BaseUserSemaphore const&) = delete;
        BaseUserSemaphore(BaseUserSemaphore&&) = delete;
        BaseUserSemaphore& operator=(BaseUserSemaphore const&) = delete;
        BaseUserSemaphore& operator=(BaseUserSemaphore&&) = delete;

        ~BaseUserSemaphore()
        {
            AE_ASSERT(this->m_Waiting.load() == 0);
        }

    public:
        void Acquire()
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

        bool TryAcquire()
        {
            int32_t current = this->m_Count.load();

            if (current == 0)
            {
                return false;
            }

            AE_ASSERT(current > 0);
            return this->m_Count.compare_exchange_weak(current, current - 1);
        }

        void Release(int32_t count = 1)
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
                FutexTraits::WakeAll(this->m_Count);
            }
            else
            {
                // Releasing fewer threads than waiters.
                FutexTraits::WakeMany(this->m_Count, count);
            }
        }

    private:
        void Wait()
        {
            // Notify other threads that we are waiting.
            this->m_Waiting.fetch_add(1);

            // Try to acquire the semaphore again.
            int32_t current = this->m_Count.load();

            if (current == 0)
            {
                // Wait for the semaphore to be released.
                FutexTraits::Wait(this->m_Count, 0);
            }

            // Release waiting counter.
            this->m_Waiting.fetch_sub(1);
        }
    };
}
