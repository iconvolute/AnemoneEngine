#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

namespace Anemone
{
    ReaderWriterLock::ReaderWriterLock()
    {
        if (int const rc = pthread_rwlock_init(&this->m_native.Inner, nullptr); rc != 0)
        {
            AE_PANIC("pthread_rwlock_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ReaderWriterLock::~ReaderWriterLock()
    {
        if (int const rc = pthread_rwlock_destroy(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::EnterShared()
    {
        if (int const rc = pthread_rwlock_rdlock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_rdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        if (int const rc = pthread_rwlock_tryrdlock(&this->m_native.Inner); rc == 0)
        {
            return true;
        }
        else if (rc == EBUSY)
        {
            return false;
        }
        else
        {
            AE_PANIC("pthread_rwlock_tryrdlock (rc: {}, `{}`)", rc, strerror(rc));
            return false;
        }
    }

    void ReaderWriterLock::LeaveShared()
    {
        if (int const rc = pthread_rwlock_unlock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::Enter()
    {
        if (int const rc = pthread_rwlock_wrlock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_wrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnter()
    {
        if (int const rc = pthread_rwlock_trywrlock(&this->m_native.Inner); rc == 0)
        {
            return true;
        }
        else if (rc == EBUSY)
        {
            return false;
        }
        else
        {
            AE_PANIC("pthread_rwlock_trywrlock (rc: {}, `{}`)", rc, strerror(rc));
            return false;
        }
    }

    void ReaderWriterLock::Leave()
    {
        if (int const rc = pthread_rwlock_unlock(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
