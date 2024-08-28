#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    CriticalSection::CriticalSection()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Create(this->_native);

        ::InitializeCriticalSectionEx(&nativeThis.Inner, 4096, CRITICAL_SECTION_NO_DEBUG_INFO);
    }

    CriticalSection::~CriticalSection()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        ::DeleteCriticalSection(&nativeThis.Inner);

        Platform::Destroy(this->_native);
    }

    void CriticalSection::Enter()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        ::EnterCriticalSection(&nativeThis.Inner);
    }

    void CriticalSection::Leave()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        ::LeaveCriticalSection(&nativeThis.Inner);
    }

    bool CriticalSection::TryEnter()
    {
        Platform::NativeCriticalSection& nativeThis = Platform::Get(this->_native);

        return !!::TryEnterCriticalSection(&nativeThis.Inner);
    }
}
