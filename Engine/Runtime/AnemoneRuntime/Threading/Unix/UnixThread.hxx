#pragma once
#include "AnemoneRuntime/Threading/Base/BaseThread.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

namespace Anemone
{
    using ThreadId = pid_t;
    using ThreadHandle = Interop::UnixPthreadThreadHandle;
}

namespace Anemone
{
    class RUNTIME_API UnixThread final
    {
    private:
        ThreadHandle _handle{};
        ThreadId _id{};

    public:
        UnixThread() = default;
        UnixThread(UnixThread const&) = delete;
        UnixThread(UnixThread&& other) noexcept;
        UnixThread& operator=(UnixThread const&) = delete;
        UnixThread& operator=(UnixThread&& other) noexcept;
        explicit UnixThread(ThreadStart const& start);
        ~UnixThread();

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

    using Thread = UnixThread;
}
