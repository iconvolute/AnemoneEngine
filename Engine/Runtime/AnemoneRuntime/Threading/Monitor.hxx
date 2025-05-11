#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Platform/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class RUNTIME_API Monitor final
    {
    private:
        Internal::PlatformMonitor _inner;

    public:
        Monitor();
        Monitor(Monitor const&) = delete;
        Monitor(Monitor&&) = delete;
        Monitor& operator=(Monitor const&) = delete;
        Monitor& operator=(Monitor&&) = delete;
        ~Monitor();

    public:
        void Enter();
        void Leave();
        bool TryEnter();
        void Wait();
        bool TryWait(Duration const& timeout);
        void NotifyOne();
        void NotifyAll();
    };

    class MonitorLock final
    {
    private:
        Monitor* _lock;

    public:
        MonitorLock() = delete;
        MonitorLock(MonitorLock const&) = delete;
        MonitorLock(MonitorLock&&) = delete;
        MonitorLock& operator=(MonitorLock const&) = delete;
        MonitorLock& operator=(MonitorLock&&) = delete;

        explicit MonitorLock(Monitor& lock)
            : _lock{&lock}
        {
            this->_lock->Enter();
        }

        ~MonitorLock()
        {
            this->_lock->Leave();
        }

        void Wait()
        {
            this->_lock->Wait();
        }

        bool TryWait(Duration const& timeout)
        {
            return this->_lock->TryWait(timeout);
        }

        void NotifyOne()
        {
            this->_lock->NotifyOne();
        }

        void NotifyAll()
        {
            this->_lock->NotifyAll();
        }
    };
}
