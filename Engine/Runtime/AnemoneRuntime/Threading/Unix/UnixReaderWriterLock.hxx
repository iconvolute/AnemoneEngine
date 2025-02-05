#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone
{
    class UnixReaderWriterLock final
    {
    private:
        pthread_rwlock_t _lock{};

    public:
        UnixReaderWriterLock()
        {
            pthread_rwlock_init(&this->_lock, nullptr);
        }

        UnixReaderWriterLock(UnixReaderWriterLock const&) = delete;
        UnixReaderWriterLock(UnixReaderWriterLock&&) = delete;
        UnixReaderWriterLock& operator=(UnixReaderWriterLock const&) = delete;
        UnixReaderWriterLock& operator=(UnixReaderWriterLock&&) = delete;

        ~UnixReaderWriterLock()
        {
            pthread_rwlock_destroy(&this->_lock);
        }

    public:
        void EnterShared()
        {
            pthread_rwlock_rdlock(&this->_lock);
        }

        bool TryEnterShared()
        {
            return pthread_rwlock_tryrdlock(&this->_lock) == 0;
        }

        void LeaveShared()
        {
            pthread_rwlock_unlock(&this->_lock);
        }

    public:
        void Enter()
        {
            pthread_rwlock_wrlock(&this->_lock);
        }

        bool TryEnter()
        {
            return pthread_rwlock_trywrlock(&this->_lock) == 0;
        }

        void Leave()
        {
            pthread_rwlock_unlock(&this->_lock);
        }
    };

    using ReaderWriterLock = UnixReaderWriterLock;
}
