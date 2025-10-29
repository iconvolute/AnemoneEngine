#pragma once
#include "AnemoneInterop/Linux/SafeHandle.hxx"
#include "AnemoneThreading/Thread.hxx"

namespace Anemone
{
    class UnixThread : public Thread
    {
    private:
        Interop::Linux::SafePthreadThreadHandle _handle;

    private:
        struct StartupContext final
        {
            std::atomic<UnixThread*> thread{};
            ThreadStart const* start{};
            std::atomic<bool> initialized{false};
        };

    private:
        static void* EntryPoint(void* context);

    public:
        UnixThread() = default;
        
        ~UnixThread() override;

    public:
        void Join() override;

        bool IsJoinable() const override;

        void Detach() override;

    public:
        static Reference<UnixThread> Start(ThreadStart const& start);
    };
}
