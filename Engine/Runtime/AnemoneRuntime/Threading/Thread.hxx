#pragma once
#include "AnemoneRuntime/Threading/Runnable.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Platform/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

#include <optional>
#include <string_view>

namespace Anemone
{
    //! Available thread priorities.
    enum class ThreadPriority
    {
        TimeCritical,
        Highest,
        AboveNormal,
        Normal,
        BelowNormal,
        Lower,
        Lowest,
    };

    //! Represents a thread start parameters.
    struct ThreadStart final
    {
        //! The name of the thread.
        std::optional<std::string_view> Name;

        //! The size of the stack.
        std::optional<size_t> StackSize;

        //! The priority of the thread.
        std::optional<ThreadPriority> Priority;

        //! The callback to run.
        Runnable* Callback{};
    };
}

namespace Anemone
{
    class RUNTIME_API Thread final
    {
    private:
        ThreadHandle _handle{};
        ThreadId _id{};

    public:
        Thread() = default;
        Thread(Thread const&) = delete;
        Thread(Thread&& other) noexcept;
        Thread& operator=(Thread const&) = delete;
        Thread& operator=(Thread&& other) noexcept;
        explicit Thread(ThreadStart const& start);
        ~Thread();

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
}
