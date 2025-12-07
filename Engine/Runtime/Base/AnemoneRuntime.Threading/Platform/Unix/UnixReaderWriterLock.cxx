#include "AnemoneRuntime.Threading/ReaderWriterLock.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

namespace Anemone
{
    ReaderWriterLock::ReaderWriterLock()
    {
        pthread_rwlock_init(&this->_inner, nullptr);
    }

    ReaderWriterLock::~ReaderWriterLock()
    {
        pthread_rwlock_destroy(&this->_inner);
    }

    void ReaderWriterLock::EnterShared()
    {
        pthread_rwlock_rdlock(&this->_inner);
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        return pthread_rwlock_tryrdlock(&this->_inner) == 0;
    }

    void ReaderWriterLock::LeaveShared()
    {
        pthread_rwlock_unlock(&this->_inner);
    }

    void ReaderWriterLock::Enter()
    {
        pthread_rwlock_wrlock(&this->_inner);
    }

    bool ReaderWriterLock::TryEnter()
    {
        return pthread_rwlock_trywrlock(&this->_inner) == 0;
    }

    void ReaderWriterLock::Leave()
    {
        pthread_rwlock_unlock(&this->_inner);
    }
}

#endif
