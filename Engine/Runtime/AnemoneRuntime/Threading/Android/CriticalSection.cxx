#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone::Threading
{
    CriticalSection::CriticalSection()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Create(this->_native);

        pthread_mutexattr_t attr{};

        if (int const rc = pthread_mutexattr_init(&attr); rc != 0)
        {
            AE_BUGCHECK("pthread_mutexattr_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); rc != 0)
        {
            AE_BUGCHECK("pthread_mutexattr_settype (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutex_init(&nativeThis.Inner, &attr); rc != 0)
        {
            AE_BUGCHECK("pthread_mutex_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_destroy(&attr); rc != 0)
        {
            AE_BUGCHECK("pthread_mutexattr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    CriticalSection::~CriticalSection()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_mutex_destroy(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_mutex_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        Platform::Destroy(this->_native);
    }

    void CriticalSection::Enter()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_mutex_lock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool CriticalSection::TryEnter()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_mutex_trylock(&nativeThis.Inner); rc != 0)
        {
            if (rc == EBUSY)
            {
                return false;
            }
            else
            {
                AE_BUGCHECK("pthread_mutex_trylock (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        return true;
    }

    void CriticalSection::Leave()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_mutex_unlock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_mutex_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
