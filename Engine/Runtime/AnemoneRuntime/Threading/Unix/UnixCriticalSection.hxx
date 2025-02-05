#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone
{
    class UnixConditionVariable;

    class UnixCriticalSection
    {
        friend class UnixConditionVariable;

    private:
        pthread_mutex_t _inner;

    public:
        UnixCriticalSection()
        {
            pthread_mutexattr_t attr{};
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&this->_inner, &attr);
            pthread_mutexattr_destroy(&attr);
        }

        UnixCriticalSection(UnixCriticalSection const&) = delete;
        UnixCriticalSection(UnixCriticalSection&&) = delete;
        UnixCriticalSection& operator=(UnixCriticalSection const&) = delete;
        UnixCriticalSection& operator=(UnixCriticalSection&&) = delete;

        ~UnixCriticalSection()
        {
            pthread_mutex_destroy(&this->_inner);
        }

        void Enter()
        {
            pthread_mutex_lock(&this->_inner);
        }

        void Leave()
        {
            pthread_mutex_unlock(&this->_inner);
        }

        bool TryEnter()
        {
            // If the mutex is already locked, this will return EBUSY.
            return pthread_mutex_trylock(&this->_inner) == 0;
        }
    };

    using CriticalSection = UnixCriticalSection;
}
