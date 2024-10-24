#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>
#include <atomic>

namespace Anemone::Threading
{
    class ConditionVariable;

    class RUNTIME_API CriticalSection final
    {
        friend class ConditionVariable;

    private:
        Platform::NativeCriticalSection m_native;

    public:
        CriticalSection();
        CriticalSection(CriticalSection const&) = delete;
        CriticalSection(CriticalSection&&) = delete;
        CriticalSection& operator=(CriticalSection const&) = delete;
        CriticalSection& operator=(CriticalSection&&) = delete;
        ~CriticalSection();

    public:
        void Enter();
        bool TryEnter();
        void Leave();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock lock{*this};
            return std::forward<F>(f)();
        }
    };
}
