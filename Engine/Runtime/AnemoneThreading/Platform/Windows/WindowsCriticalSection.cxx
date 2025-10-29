#include "AnemoneThreading/CriticalSection.hxx"

#if ANEMONE_PLATFORM_WINDOWS

namespace Anemone
{
    CriticalSection::CriticalSection()
    {
        InitializeSRWLock(&this->_inner);
    }

    CriticalSection::~CriticalSection() = default;

    void CriticalSection::Enter()
    {
        AcquireSRWLockExclusive(&this->_inner);
    }

    void CriticalSection::Leave()
    {
        ReleaseSRWLockExclusive(&this->_inner);
    }

    bool CriticalSection::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->_inner);
    }
}

namespace Anemone
{
    RecursiveCriticalSection::RecursiveCriticalSection()
    {
        InitializeCriticalSection(&this->_inner);
    }

    RecursiveCriticalSection::~RecursiveCriticalSection()
    {
        DeleteCriticalSection(&this->_inner);
    }

    void RecursiveCriticalSection::Enter()
    {
        EnterCriticalSection(&this->_inner);
    }

    void RecursiveCriticalSection::Leave()
    {
        LeaveCriticalSection(&this->_inner);
    }

    bool RecursiveCriticalSection::TryEnter()
    {
        return TryEnterCriticalSection(&this->_inner);
    }
}

#endif
