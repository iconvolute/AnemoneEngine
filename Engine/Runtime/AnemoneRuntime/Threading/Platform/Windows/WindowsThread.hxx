#pragma once
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

namespace Anemone
{
    class WindowsThread : public Thread
    {
    private:
        Interop::Windows::SafeHandle _handle{};
        DWORD _id{};

    public:
        WindowsThread() = default;

        ~WindowsThread() override;

    public:
        ThreadId Id() const override;

        void Join() override;

        bool IsJoinable() const override;

        void Detach() override;

    public:
        static Reference<WindowsThread> Start(ThreadStart const& start);

        static DWORD WINAPI EntryPoint(LPVOID lpThreadParameter);
    };
}
