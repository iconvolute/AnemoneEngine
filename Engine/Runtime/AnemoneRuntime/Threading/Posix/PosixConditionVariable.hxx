#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"

namespace Anemone
{
    class PosixCriticalSection;

    class RUNTIME_API PosixConditionVariable final
    {
        friend PosixCriticalSection;

    private:
        pthread_cond_t m_native;

    public:
        PosixConditionVariable();
        PosixConditionVariable(PosixConditionVariable const&) = delete;
        PosixConditionVariable(PosixConditionVariable&&) = delete;
        PosixConditionVariable& operator=(PosixConditionVariable const&) = delete;
        PosixConditionVariable& operator=(PosixConditionVariable&&) = delete;
        ~PosixConditionVariable();

    public:
        void Wait(PosixCriticalSection& lock);
        bool TryWait(PosixCriticalSection& lock, Duration const& timeout);
        void Notify();
        void NotifyAll();
    };

    using ConditionVariable = PosixConditionVariable;
}
