#pragma once
#include "AnemoneRuntime.Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime.Threading/Thread.hxx"

namespace Anemone
{
    class WindowsThread final : public Thread
    {
    private:
        Interop::Windows::SafeHandle _handle{};

    private:
        struct StartupContext final
        {
            std::atomic<WindowsThread*> thread{};
            ThreadStart const* start{};
            std::atomic<bool> initialized{false};
        };

    private:
        static DWORD WINAPI EntryPoint(LPVOID lpThreadParameter);

    public:
        WindowsThread() = default;

        ~WindowsThread() override;

    public:
        void Join() override;

        bool IsJoinable() const override;

        void Detach() override;

    public:
        static Reference<WindowsThread> Start(ThreadStart const& start);
    };
}
