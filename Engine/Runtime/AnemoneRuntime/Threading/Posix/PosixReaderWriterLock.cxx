#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    PosixReaderWriterLock::PosixReaderWriterLock()
    {
        if (int const rc = pthread_rwlock_init(&this->m_native, nullptr); rc != 0)
        {
            AE_PANIC("pthread_rwlock_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    PosixReaderWriterLock::~PosixReaderWriterLock()
    {
        if (int const rc = pthread_rwlock_destroy(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_rwlock_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void PosixReaderWriterLock::EnterShared()
    {
        if (int const rc = pthread_rwlock_rdlock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_rwlock_rdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool PosixReaderWriterLock::TryEnterShared()
    {
        if (int const rc = pthread_rwlock_tryrdlock(&this->m_native); rc == 0)
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

    void PosixReaderWriterLock::LeaveShared()
    {
        if (int const rc = pthread_rwlock_unlock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void PosixReaderWriterLock::Enter()
    {
        if (int const rc = pthread_rwlock_wrlock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_rwlock_wrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool PosixReaderWriterLock::TryEnter()
    {
        if (int const rc = pthread_rwlock_trywrlock(&this->m_native); rc == 0)
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

    void PosixReaderWriterLock::Leave()
    {
        if (int const rc = pthread_rwlock_unlock(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
