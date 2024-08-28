#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Threading/Posix/Native.hxx"

namespace Anemone::Threading
{
    ConditionVariable::ConditionVariable()
    {
        Private::NativeConditionVariable& nativeThis = Platform::Create(this->_native);

        if (int const rc = pthread_cond_init(&nativeThis.Inner, nullptr); rc != 0)
        {
            AE_BUGCHECK("pthread_cond_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ConditionVariable::~ConditionVariable()
    {
        Private::NativeConditionVariable& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_cond_destroy(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_cond_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        Platform::Destroy(this->_native);
    }

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        Private::NativeConditionVariable& nativeThis = Platform::Get(this->_native);
        Private::NativeCriticalSection& nativeLock = Platform::Get(lock._native);

        int const rc = pthread_cond_wait(
                &nativeThis.Inner,
                &nativeLock.Inner);

        if (rc != 0)
        {
            AE_BUGCHECK("pthread_cond_wait (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, Duration const& timeout)
    {
        Private::NativeConditionVariable& nativeThis = Platform::Get(this->_native);
        Private::NativeCriticalSection& nativeLock = Platform::Get(lock._native);

        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Platform::Private::posix_ValidateTimeout(ts, timeout);

        int const rc = pthread_cond_timedwait(
                &nativeThis.Inner,
                &nativeLock.Inner,
                &ts);

        if (rc == ETIMEDOUT)
        {
            return false;
        }
        else if (rc != 0)
        {
            AE_BUGCHECK("pthread_cond_timedwait (rc: {}, `{}`)", rc, strerror(rc));
        }

        return true;
    }

    void ConditionVariable::Notify()
    {
        Private::NativeConditionVariable& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_cond_signal(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
    void ConditionVariable::NotifyAll()
    {
        Private::NativeConditionVariable& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_cond_broadcast(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
