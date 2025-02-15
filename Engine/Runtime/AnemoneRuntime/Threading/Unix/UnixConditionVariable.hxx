#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixCriticalSection.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone
{
    class UnixCriticalSection;

    class UnixConditionVariable final
    {
        friend class UnixCriticalSection;

    private:
        pthread_cond_t _inner;

    public:
        UnixConditionVariable()
        {
            pthread_cond_init(&this->_inner, nullptr);
        }

        UnixConditionVariable(UnixConditionVariable const&) = delete;
        UnixConditionVariable(UnixConditionVariable&&) = delete;
        UnixConditionVariable& operator=(UnixConditionVariable const&) = delete;
        UnixConditionVariable& operator=(UnixConditionVariable&&) = delete;

        ~UnixConditionVariable()
        {
            pthread_cond_destroy(&this->_inner);
        }

        void Wait(UnixCriticalSection& cs)
        {
            pthread_cond_wait(&this->_inner, &cs._inner);
        }

        template <typename Predicate>
        void Wait(UnixCriticalSection& cs, Predicate&& predicate)
        {
            while (!std::forward<Predicate>(predicate)())
            {
                pthread_cond_wait(&this->_inner, &cs._inner);
            }
        }

        bool TryWait(UnixCriticalSection& cs, Duration const& timeout)
        {
            timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            Interop::posix_ValidateTimeout(ts, timeout);

            // Should fail on ETIMEDOUT only.
            return pthread_cond_timedwait(&this->_inner, &cs._inner, &ts) == 0;
        }

        void Notify()
        {
            pthread_cond_signal(&this->_inner);
        }

        void NotifyAll()
        {
            pthread_cond_broadcast(&this->_inner);
        }
    };

    using ConditionVariable = UnixConditionVariable;
}
