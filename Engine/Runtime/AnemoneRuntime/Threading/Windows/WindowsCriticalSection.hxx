#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone
{
    class WindowsConditionVariable;

    class WindowsCriticalSection final
    {
        friend class WindowsConditionVariable;

    private:
        CRITICAL_SECTION _inner;

    public:
        WindowsCriticalSection()
        {
            InitializeCriticalSection(&this->_inner);
        }

        WindowsCriticalSection(WindowsCriticalSection const&) = delete;
        WindowsCriticalSection(WindowsCriticalSection&&) = delete;
        WindowsCriticalSection& operator=(WindowsCriticalSection const&) = delete;
        WindowsCriticalSection& operator=(WindowsCriticalSection&&) = delete;

        ~WindowsCriticalSection()
        {
            DeleteCriticalSection(&this->_inner);
        }

        void Enter()
        {
            EnterCriticalSection(&this->_inner);
        }

        void Leave()
        {
            LeaveCriticalSection(&this->_inner);
        }

        bool TryEnter()
        {
            return TryEnterCriticalSection(&this->_inner);
        }
    };

    using CriticalSection = WindowsCriticalSection;
}
