#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"
#else
#error Not implemented
#endif

#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone
{
    class UserSemaphore final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit UserSemaphore(int32_t initial)
            : m_Count{initial}
        {
        }

        UserSemaphore(UserSemaphore const&) = delete;
        UserSemaphore(UserSemaphore&&) = delete;
        UserSemaphore& operator=(UserSemaphore const&) = delete;
        UserSemaphore& operator=(UserSemaphore&&) = delete;

        ~UserSemaphore()
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
                Internal::Futex::WakeAll(this->m_Count);
            }
            else
            {
                // Releasing fewer threads than waiters.
                Internal::Futex::WakeMany(this->m_Count, count);
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
                Internal::Futex::Wait(this->m_Count, 0);
            }

            // Release waiting counter.
            this->m_Waiting.fetch_sub(1);
        }
    };
}

#else

#include "AnemoneRuntime/Threading/Semaphore.hxx"

namespace Anemone
{
    using UserSemaphore = Semaphore;
}

#endif
