#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone
{
    class WindowsReaderWriterLock final
    {
    private:
        SRWLOCK _inner;

    public:
        WindowsReaderWriterLock()
        {
            InitializeSRWLock(&this->_inner);
        }

        WindowsReaderWriterLock(WindowsReaderWriterLock const&) = delete;
        WindowsReaderWriterLock(WindowsReaderWriterLock&&) = delete;
        WindowsReaderWriterLock& operator=(WindowsReaderWriterLock const&) = delete;
        WindowsReaderWriterLock& operator=(WindowsReaderWriterLock&&) = delete;
        ~WindowsReaderWriterLock() = default;

    public:
        void EnterShared()
        {
            AcquireSRWLockShared(&this->_inner);
        }

        bool TryEnterShared()
        {
            return TryAcquireSRWLockShared(&this->_inner);
        }

        void LeaveShared()
        {
            ReleaseSRWLockShared(&this->_inner);
        }

    public:
        void Enter()
        {
            AcquireSRWLockExclusive(&this->_inner);
        }

        bool TryEnter()
        {
            return TryAcquireSRWLockExclusive(&this->_inner);
        }

        void Leave()
        {
            ReleaseSRWLockExclusive(&this->_inner);
        }
    };

    using ReaderWriterLock = WindowsReaderWriterLock;
}
