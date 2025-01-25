#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    CriticalSection::CriticalSection()
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

        if (int const rc = pthread_mutex_init(&this->m_native.Inner, &attr); rc != 0)
        {
            AE_PANIC("pthread_mutex_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_destroy(&attr); rc != 0)
        {
            AE_PANIC("pthread_mutexattr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    CriticalSection::~CriticalSection()
    {
        if (int const rc = pthread_mutex_destroy(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_mutex_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void CriticalSection::Enter()
    {
        if (int const rc = pthread_mutex_lock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_mutex_lock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void CriticalSection::Leave()
    {
        if (int const rc = pthread_mutex_unlock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_mutex_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool CriticalSection::TryEnter()
    {
        if (int const rc = pthread_mutex_trylock(&this->m_native.Inner); rc != 0)
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
