#include "AnemoneRuntime/Threading/Windows/WindowsCriticalSection.hxx"

namespace Anemone
{
    WindowsCriticalSection::WindowsCriticalSection()
    {
        InitializeCriticalSectionEx(&this->m_native, 4096, CRITICAL_SECTION_NO_DEBUG_INFO);
    }

    WindowsCriticalSection::~WindowsCriticalSection()
    {
        DeleteCriticalSection(&this->m_native);
    }

    void WindowsCriticalSection::Enter()
    {
        EnterCriticalSection(&this->m_native);
    }

    void WindowsCriticalSection::Leave()
    {
        LeaveCriticalSection(&this->m_native);
    }

    bool WindowsCriticalSection::TryEnter()
    {
        return TryEnterCriticalSection(&this->m_native);
    }
}
