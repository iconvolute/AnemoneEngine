#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    CriticalSection::CriticalSection()
    {
        InitializeCriticalSectionEx(&this->m_native.Inner, 4096, CRITICAL_SECTION_NO_DEBUG_INFO);
    }

    CriticalSection::~CriticalSection()
    {
        DeleteCriticalSection(&this->m_native.Inner);
    }

    void CriticalSection::Enter()
    {
        EnterCriticalSection(&this->m_native.Inner);
    }

    void CriticalSection::Leave()
    {
        LeaveCriticalSection(&this->m_native.Inner);
    }

    bool CriticalSection::TryEnter()
    {
        return !!TryEnterCriticalSection(&this->m_native.Inner);
    }
}
