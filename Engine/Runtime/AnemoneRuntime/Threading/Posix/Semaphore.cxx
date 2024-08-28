#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone::Threading
{
    Semaphore::Semaphore(int32_t initialCount, int32_t maxCount)
    {
        AE_ASSERT(initialCount >= 0);
        AE_ASSERT(maxCount >= 1);

        Platform::NativeSemaphore& nativeThis = Platform::Create(this->_native);

        if (pthread_mutex_init(&nativeThis.Mutex, nullptr))
        {
            AE_BUGCHECK("pthread_mutex_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        pthread_condattr_t attr;
        if (pthread_condattr_init(&attr))
        {
            AE_BUGCHECK("pthread_condattr_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC))
        {
            AE_BUGCHECK("pthread_condattr_setclock (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_cond_init(&nativeThis.Cond, &attr))
        {
            AE_BUGCHECK("pthread_cond_init (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_condattr_destroy(&attr))
        {
            AE_BUGCHECK("pthread_condattr_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    Semaphore::~Semaphore()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        if (pthread_cond_destroy(&nativeThis.Cond) != 0)
        {
            AE_BUGCHECK("pthread_cond_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_destroy(&nativeThis.Mutex) != 0)
        {
            AE_BUGCHECK("pthread_mutex_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }

        Platform::Destroy(this->_native);
    }

    void Semaphore::Acquire()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        if (pthread_mutex_lock(&nativeThis.Mutex))
        {
            AE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        while (nativeThis.Current < 1)
        {
            if (pthread_cond_wait(&nativeThis.Cond, &nativeThis.Mutex))
            {
                AE_BUGCHECK("pthread_cond_wait (rc: {}, `{}`)", errno, strerror(errno));
            }
        }

        --nativeThis.Current;

        if (pthread_mutex_unlock(&nativeThis.Mutex))
        {
            AE_BUGCHECK("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    bool Semaphore::TryAcquire(Duration const& timeout)
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        timespec ts{};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        Platform::posix_ValidateTimeout(ts, timeout);

        if (pthread_mutex_lock(&nativeThis.Mutex))
        {
            AE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        int rc = 0;

        while (nativeThis.Current < 1)
        {
            if ((rc = pthread_cond_timedwait(&nativeThis.Cond, &nativeThis.Mutex, &ts)))
            {
                if (rc == ETIMEDOUT)
                {
                    break;
                }

                AE_BUGCHECK("pthread_cond_timedwait (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        if (rc == 0)
        {
            --nativeThis.Current;
        }

        pthread_mutex_unlock(&nativeThis.Mutex);
        return rc == 0;
    }

    bool Semaphore::TryAcquire()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        return this->TryAcquire(Duration::FromMilliseconds(0));
    }

    void Semaphore::Release()
    {
        Platform::NativeSemaphore& nativeThis = Platform::Get(this->_native);

        if (pthread_mutex_lock(&nativeThis.Mutex))
        {
            AE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (nativeThis.Current < nativeThis.Max)
        {
            ++nativeThis.Current;
        }
        else
        {
            AE_BUGCHECK("semaphore overflow");
        }

        if (pthread_cond_signal(&nativeThis.Cond))
        {
            AE_BUGCHECK("pthread_cond_signal (rc: {}, `{}`)", errno, strerror(errno));
        }

        if (pthread_mutex_unlock(&nativeThis.Mutex))
        {
            AE_BUGCHECK("pthread_mutex_unlock (rc: {}, `{}`)", errno, strerror(errno));
        }
    }
}
