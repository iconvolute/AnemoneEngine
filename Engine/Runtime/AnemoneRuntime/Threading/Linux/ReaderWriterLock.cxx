#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Threading/Posix/Native.hxx"

namespace Anemone::Threading
{
    ReaderWriterLock::ReaderWriterLock()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Create(this->_native);

        if (int const rc = pthread_rwlock_init(&nativeThis.Inner, nullptr); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ReaderWriterLock::~ReaderWriterLock()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_destroy(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        Platform::Destroy(this->_native);
    }

    void ReaderWriterLock::EnterShared()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_rdlock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_rdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

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
            AE_BUGCHECK("pthread_rwlock_tryrdlock (rc: {}, `{}`)", rc, strerror(rc));
            return false;
        }
    }

    void ReaderWriterLock::LeaveShared()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_unlock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::Enter()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc = pthread_rwlock_wrlock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_wrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnter()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

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
            AE_BUGCHECK("pthread_rwlock_trywrlock (rc: {}, `{}`)", rc, strerror(rc));
            return false;
        }
    }

    void ReaderWriterLock::Leave()
    {
        Private::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        if (int const rc  = pthread_rwlock_unlock(&nativeThis.Inner); rc != 0)
        {
            AE_BUGCHECK("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
