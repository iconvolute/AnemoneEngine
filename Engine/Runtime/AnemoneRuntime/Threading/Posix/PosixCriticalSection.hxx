#pragma once
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>

namespace Anemone
{
    class PosixConditionVariable;

    class RUNTIME_API PosixCriticalSection final
    {
        friend PosixConditionVariable;

    private:
        pthread_mutex_t m_native;

    public:
        PosixCriticalSection();

        PosixCriticalSection(PosixCriticalSection const&) = delete;
        PosixCriticalSection(PosixCriticalSection&&) = delete;
        PosixCriticalSection& operator=(PosixCriticalSection const&) = delete;
        PosixCriticalSection& operator=(PosixCriticalSection&&) = delete;

        ~PosixCriticalSection();

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

    using CriticalSection = PosixCriticalSection;
}
