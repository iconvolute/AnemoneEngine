#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#if ANEMONE_PLATFORM_WINDOWS

namespace Anemone
{
    CriticalSection::CriticalSection()
    {
        InitializeCriticalSection(&this->_inner);
    }

    CriticalSection::~CriticalSection()
    {
        DeleteCriticalSection(&this->_inner);
    }

    void CriticalSection::Enter()
    {
        EnterCriticalSection(&this->_inner);
    }

    void CriticalSection::Leave()
    {
        LeaveCriticalSection(&this->_inner);
    }

    bool CriticalSection::TryEnter()
    {
        return TryEnterCriticalSection(&this->_inner);
    }
}

#endif
