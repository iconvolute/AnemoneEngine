#pragma once
#include "AnemoneRuntime/Threading/Base/BaseThread.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone
{
    using ThreadId = DWORD;
    using ThreadHandle = Interop::Win32SafeHandle;
}

namespace Anemone
{
    class RUNTIME_API WindowsThread final
    {
    private:
        ThreadHandle _handle{};
        ThreadId _id{};

    public:
        WindowsThread() = default;
        WindowsThread(WindowsThread const&) = delete;
        WindowsThread(WindowsThread&& other) noexcept;
        WindowsThread& operator=(WindowsThread const&) = delete;
        WindowsThread& operator=(WindowsThread&& other) noexcept;
        explicit WindowsThread(ThreadStart const& start);
        ~WindowsThread();

    public:
        ThreadHandle const& GetHandle() const
        {
            return this->_handle;
        }

        ThreadId GetId() const
        {
            return this->_id;
        }

    public:
        void Join();
        bool IsJoinable() const;
        void Detach();
    };

    using Thread = WindowsThread;
}
