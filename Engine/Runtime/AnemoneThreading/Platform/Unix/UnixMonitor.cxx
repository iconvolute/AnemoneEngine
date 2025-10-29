#include "AnemoneThreading/Monitor.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

#include "AnemoneInterop/Linux/Threading.hxx"

namespace Anemone
{
    Monitor::Monitor()
    {
        pthread_mutexattr_t attr{};
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP);
        pthread_mutex_init(&this->_inner._lock, &attr);
        pthread_mutexattr_destroy(&attr);

        pthread_cond_init(&this->_inner._condition, nullptr);
    }

    Monitor::~Monitor()
    {
        pthread_cond_destroy(&this->_inner._condition);

        pthread_mutex_destroy(&this->_inner._lock);
    }

    void Monitor::Enter()
    {
        pthread_mutex_lock(&this->_inner._lock);
    }

    void Monitor::Leave()
    {
        pthread_mutex_unlock(&this->_inner._lock);
    }

    bool Monitor::TryEnter()
    {
        return pthread_mutex_trylock(&this->_inner._lock) == 0;
    }

    void Monitor::Wait()
    {
        pthread_cond_wait(&this->_inner._condition, &this->_inner._lock);
    }

    bool Monitor::TryWait(Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Interop::Linux::ValidateTimeout(ts, timeout);

        // Should fail on ETIMEDOUT only.
        return pthread_cond_timedwait(&this->_inner._condition, &this->_inner._lock, &ts) == 0;
    }

    void Monitor::NotifyOne()
    {
        pthread_cond_signal(&this->_inner._condition);
    }

    void Monitor::NotifyAll()
    {
        pthread_cond_broadcast(&this->_inner._condition);
    }
}

#endif
