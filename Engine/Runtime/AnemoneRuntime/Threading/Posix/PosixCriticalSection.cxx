#include "AnemoneRuntime/Threading/Posix/PosixCriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone
{
    PosixCriticalSection::PosixCriticalSection()
    {
        pthread_mutexattr_t attr{};

        if (int const rc = pthread_mutexattr_init(&attr); rc != 0)
        {
            AE_PANIC("pthread_mutexattr_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); rc != 0)
        {
            AE_PANIC("pthread_mutexattr_settype (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutex_init(&this->m_native, &attr); rc != 0)
        {
            AE_PANIC("pthread_mutex_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_destroy(&attr); rc != 0)
        {
            AE_PANIC("pthread_mutexattr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    PosixCriticalSection::~PosixCriticalSection()
    {
        if (int const rc = pthread_mutex_destroy(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_mutex_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void PosixCriticalSection::Enter()
    {
        if (int const rc = pthread_mutex_lock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void PosixCriticalSection::Leave()
    {
        if (int const rc = pthread_mutex_unlock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool PosixCriticalSection::TryEnter()
    {
        if (int const rc = pthread_mutex_trylock(&this->m_native); rc != 0)
        {
            if (rc == EBUSY)
            {
                return false;
            }
            else
            {
                AE_PANIC("pthread_mutex_trylock (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        return true;
    }
}
