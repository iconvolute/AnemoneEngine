#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>

namespace Anemone
{
    class WindowsConditionVariable;

    class RUNTIME_API WindowsCriticalSection
    {
        friend WindowsConditionVariable;

    private:
        CRITICAL_SECTION m_native;

    public:
        WindowsCriticalSection();

        WindowsCriticalSection(WindowsCriticalSection const&) = delete;
        WindowsCriticalSection(WindowsCriticalSection&&) = delete;
        WindowsCriticalSection& operator=(WindowsCriticalSection const&) = delete;
        WindowsCriticalSection& operator=(WindowsCriticalSection&&) = delete;

        ~WindowsCriticalSection();

    public:
        void Enter();

        void Leave();

        bool TryEnter();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock _{*this};
            return std::forward<F>(f)();
        }
    };

    using CriticalSection = WindowsCriticalSection;
}
