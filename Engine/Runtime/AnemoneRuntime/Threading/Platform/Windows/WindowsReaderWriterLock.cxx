#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

namespace Anemone
{
    ReaderWriterLock::ReaderWriterLock()
    {
        InitializeSRWLock(&this->_inner);
    }

    ReaderWriterLock::~ReaderWriterLock() = default;

    void ReaderWriterLock::EnterShared()
    {
        AcquireSRWLockShared(&this->_inner);
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        return TryAcquireSRWLockShared(&this->_inner);
    }

    void ReaderWriterLock::LeaveShared()
    {
        ReleaseSRWLockShared(&this->_inner);
    }

    void ReaderWriterLock::Enter()
    {
        AcquireSRWLockExclusive(&this->_inner);
    }

    bool ReaderWriterLock::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->_inner);
    }

    void ReaderWriterLock::Leave()
    {
        ReleaseSRWLockExclusive(&this->_inner);
    }
}

#endif
