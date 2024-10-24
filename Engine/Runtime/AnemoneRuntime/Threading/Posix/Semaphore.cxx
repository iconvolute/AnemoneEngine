#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone::Threading
{
    Semaphore::Semaphore(int32_t initialCount, int32_t maxCount)
    {
        AE_ASSERT(initialCount >= 0);
        AE_ASSERT(maxCount >= 1);

        if (pthread_mutex_init(&this->m_native.Mutex, nullptr))
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

        if (pthread_cond_init(&this->m_native.Cond, &attr))
        {
            AE_PANIC("pthread_cond_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_condattr_destroy(&attr))
        {
            AE_PANIC("pthread_condattr_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    Semaphore::~Semaphore()
    {
        if (pthread_cond_destroy(&this->m_native.Cond) != 0)
        {
            AE_PANIC("pthread_cond_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_destroy(&this->m_native.Mutex) != 0)
        {
            AE_PANIC("pthread_mutex_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    void Semaphore::Acquire()
    {
        if (pthread_mutex_lock(&this->m_native.Mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        while (this->m_native.Current < 1)
        {
            if (pthread_cond_wait(&this->m_native.Cond, &this->m_native.Mutex))
            {
                AE_PANIC("pthread_cond_wait (rc: {}, `{}`)", errno, strerror(errno));
            }
        }

        --this->m_native.Current;

        if (pthread_mutex_unlock(&this->m_native.Mutex))
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    bool Semaphore::TryAcquire(Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        Platform::posix_ValidateTimeout(ts, timeout);

        if (pthread_mutex_lock(&this->m_native.Mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        int rc = 0;

        while (this->m_native.Current < 1)
        {
            if ((rc = pthread_cond_timedwait(&this->m_native.Cond, &this->m_native.Mutex, &ts)))
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
            --this->m_native.Current;
        }

        pthread_mutex_unlock(&this->m_native.Mutex);
        return rc == 0;
    }

    bool Semaphore::TryAcquire()
    {
        return this->TryAcquire(Duration::FromMilliseconds(0));
    }

    void Semaphore::Release()
    {
        if (pthread_mutex_lock(&this->m_native.Mutex))
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (this->m_native.Current < this->m_native.Max)
        {
            ++this->m_native.Current;
        }
        else
        {
            AE_PANIC("semaphore overflow");
        }

        if (pthread_cond_signal(&this->m_native.Cond))
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_unlock(&this->m_native.Mutex))
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }
}
