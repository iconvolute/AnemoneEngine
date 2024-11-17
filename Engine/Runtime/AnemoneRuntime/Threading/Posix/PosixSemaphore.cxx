#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    PosixSemaphore::PosixSemaphore(int32_t initialCount, int32_t maxCount)
    {
        AE_ASSERT(initialCount >= 0);
        AE_ASSERT(maxCount >= 1);

        this->m_max = maxCount;
        this->m_current = initialCount;

        if (pthread_mutex_init(&this->m_mutex, nullptr))
        {
            AE_PANIC("pthread_mutex_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        pthread_condattr_t attr;
        if (pthread_condattr_init(&attr))
        {
            AE_PANIC("pthread_condattr_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC))
        {
            AE_PANIC("pthread_condattr_setclock (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_cond_init(&this->m_cond, &attr))
        {
            AE_PANIC("pthread_cond_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_condattr_destroy(&attr))
        {
            AE_PANIC("pthread_condattr_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    PosixSemaphore::~PosixSemaphore()
    {
        if (pthread_cond_destroy(&this->m_cond) != 0)
        {
            AE_PANIC("pthread_cond_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_destroy(&this->m_mutex) != 0)
        {
            AE_PANIC("pthread_mutex_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    void PosixSemaphore::Acquire()
    {
        if (pthread_mutex_lock(&this->m_mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        while (this->m_current < 1)
        {
            if (pthread_cond_wait(&this->m_cond, &this->m_mutex))
            {
                AE_PANIC("pthread_cond_wait (rc: {}, `{}`)", errno, strerror(errno));
            }
        }

        --this->m_current;

        if (pthread_mutex_unlock(&this->m_mutex))
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    bool PosixSemaphore::TryAcquire(Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        Platform::posix_ValidateTimeout(ts, timeout);

        if (pthread_mutex_lock(&this->m_mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        int rc = 0;

        while (this->m_current < 1)
        {
            if ((rc = pthread_cond_timedwait(&this->m_cond, &this->m_mutex, &ts)))
            {
                if (rc == ETIMEDOUT)
                {
                    break;
                }

                AE_PANIC("pthread_cond_timedwait (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        if (rc == 0)
        {
            --this->m_current;
        }

        pthread_mutex_unlock(&this->m_mutex);
        return rc == 0;
    }

    bool PosixSemaphore::TryAcquire()
    {
        return this->TryAcquire(Duration::FromMilliseconds(0));
    }

    void PosixSemaphore::Release()
    {
        if (pthread_mutex_lock(&this->m_mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (this->m_current < this->m_max)
        {
            ++this->m_current;
        }
        else
        {
            AE_PANIC("semaphore overflow");
        }

        if (pthread_cond_signal(&this->m_cond))
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_unlock(&this->m_mutex))
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }
}
