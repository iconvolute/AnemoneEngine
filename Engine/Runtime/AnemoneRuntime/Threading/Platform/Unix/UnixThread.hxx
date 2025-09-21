#pragma once
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"

namespace Anemone
{
    class UnixThread : public Thread
    {
    private:
        Interop::Linux::SafePthreadThreadHandle _handle;
        ThreadId _id{};

    private:
        struct StartupContext
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
        ThreadId Id() const override;

        void Join() override;

        bool IsJoinable() const override;

        void Detach() override;

    public:
        static Reference<UnixThread> Start(ThreadStart const& start);
    };
}