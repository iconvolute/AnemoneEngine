#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone::Threading
{
    ReaderWriterLock::ReaderWriterLock()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Create(this->_native);

        if (int const rc = pthread_rwlock_init(&nativeThis.Inner, nullptr); rc != 0)
        {
            AE_PANIC("pthread_rwlock_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ReaderWriterLock::~ReaderWriterLock()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_destroy(&nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        Platform::Destroy(this->_native);
    }

    void ReaderWriterLock::EnterShared()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_rdlock(&nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_rdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_tryrdlock(&nativeThis.Inner); rc == 0)
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
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_unlock(&nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::Enter()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_wrlock(&nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_wrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnter()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_trywrlock(&nativeThis.Inner); rc == 0)
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
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc  = pthread_rwlock_unlock(&nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
