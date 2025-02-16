#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

namespace Anemone
{
    CriticalSection::CriticalSection()
    {
        pthread_mutexattr_t attr{};
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&this->_inner, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    CriticalSection::~CriticalSection()
    {
        pthread_mutex_destroy(&this->_inner);
    }

    void CriticalSection::Enter()
    {
        pthread_mutex_lock(&this->_inner);
    }

    void CriticalSection::Leave()
    {
        pthread_mutex_unlock(&this->_inner);
    }

    bool CriticalSection::TryEnter()
    {
        // If the mutex is already locked, this will return EBUSY.
        return pthread_mutex_trylock(&this->_inner) == 0;
    }
}

#endif
