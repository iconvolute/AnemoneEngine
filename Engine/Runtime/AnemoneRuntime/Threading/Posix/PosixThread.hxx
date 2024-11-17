#pragma once
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"

namespace Anemone
{
    //! Represents a thread.
    class RUNTIME_API PosixThread final
    {
    private:
        pthread_t m_handle{};

    public:
        PosixThread();

        explicit PosixThread(ThreadStart const& start);

        PosixThread(PosixThread const&) = delete;

        PosixThread(PosixThread&& other) noexcept;

        PosixThread& operator=(PosixThread const&) = delete;

        PosixThread& operator=(PosixThread&& other) noexcept;

        ~PosixThread();

    public:
        void Join();
        [[nodiscard]] bool IsJoinable() const;
        [[nodiscard]] ThreadId GetId() const;
        void Detach();
    };

    RUNTIME_API ThreadId GetThisThreadId();

    using Thread = PosixThread;
}
