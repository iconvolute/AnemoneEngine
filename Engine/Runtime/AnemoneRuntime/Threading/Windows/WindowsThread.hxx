#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"

namespace Anemone
{
    //! Represents a thread.
    class RUNTIME_API WindowsThread final
    {
    private:
        HANDLE m_handle{};
        DWORD m_id{};

    public:
        //! Creates a new thread object. Does not start the thread.
        WindowsThread();

        //! Starts a new thread.
        explicit WindowsThread(ThreadStart const& start);

        WindowsThread(WindowsThread const&) = delete;

        WindowsThread(WindowsThread&& other) noexcept;

        WindowsThread& operator=(WindowsThread const&) = delete;

        WindowsThread& operator=(WindowsThread&& other) noexcept;

        ~WindowsThread();

    public:
        void Join();
        [[nodiscard]] bool IsJoinable() const;
        [[nodiscard]] ThreadId GetId() const;
        void Detach();
    };

    RUNTIME_API ThreadId GetThisThreadId();

    using Thread = WindowsThread;
}
