#include "AnemoneRuntime/Threading/Windows/WindowsReaderWriterLock.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone
{
    WindowsReaderWriterLock::WindowsReaderWriterLock()
    {
        InitializeSRWLock(&this->m_native);
    }

    WindowsReaderWriterLock::~WindowsReaderWriterLock() = default;

    void WindowsReaderWriterLock::EnterShared()
    {
        AcquireSRWLockShared(&this->m_native);
    }

    bool WindowsReaderWriterLock::TryEnterShared()
    {
        return TryAcquireSRWLockShared(&this->m_native) != FALSE;
    }

    void WindowsReaderWriterLock::LeaveShared()
    {
        ReleaseSRWLockShared(&this->m_native);
    }

    void WindowsReaderWriterLock::Enter()
    {
        AcquireSRWLockExclusive(&this->m_native);
    }

    bool WindowsReaderWriterLock::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->m_native) != FALSE;
    }

    void WindowsReaderWriterLock::Leave()
    {
        ReleaseSRWLockExclusive(&this->m_native);
    }
}
