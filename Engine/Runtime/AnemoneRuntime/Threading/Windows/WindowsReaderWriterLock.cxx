#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

namespace Anemone
{
    ReaderWriterLock::ReaderWriterLock()
    {
        InitializeSRWLock(&this->m_native.Inner);
    }

    ReaderWriterLock::~ReaderWriterLock() = default;

    void ReaderWriterLock::EnterShared()
    {
        AcquireSRWLockShared(&this->m_native.Inner);
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        return TryAcquireSRWLockShared(&this->m_native.Inner) != FALSE;
    }

    void ReaderWriterLock::LeaveShared()
    {
        ReleaseSRWLockShared(&this->m_native.Inner);
    }

    void ReaderWriterLock::Enter()
    {
        AcquireSRWLockExclusive(&this->m_native.Inner);
    }

    bool ReaderWriterLock::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->m_native.Inner) != FALSE;
    }

    void ReaderWriterLock::Leave()
    {
        ReleaseSRWLockExclusive(&this->m_native.Inner);
    }
}
