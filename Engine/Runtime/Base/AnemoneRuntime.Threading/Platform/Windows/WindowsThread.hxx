#pragma once
#include "AnemoneRuntime.Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime.Threading/Thread.hxx"

#define ANEMONE_WINDOWS_USE_BEGINTHREADEX false

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
#if ANEMONE_WINDOWS_USE_BEGINTHREADEX
        static unsigned __stdcall EntryPoint(void* lpThreadParameter);
#else
        static DWORD WINAPI EntryPoint(LPVOID lpThreadParameter);
#endif

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
