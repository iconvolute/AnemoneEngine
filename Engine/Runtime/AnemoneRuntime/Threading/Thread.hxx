#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Threading/Runnable.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsNative.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixNative.hxx"
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

    struct ThreadId final
    {
        Internal::NativeThreadId Value;

        constexpr auto operator<=>(ThreadId const& other) const = default;

        static constexpr ThreadId Invalid()
        {
            return {};
        }
    };

    //! Represents a thread.
    class RUNTIME_API Thread final
    {
    private:
        Internal::NativeThreadHandle _handle{};
        Internal::NativeThreadId _id{};

    public:
        //! Creates a new thread object. Does not start the thread.
        Thread();

        //! Starts a new thread.
        explicit Thread(ThreadStart const& start);

        Thread(Thread const&) = delete;

        Thread(Thread&& other) noexcept;

        Thread& operator=(Thread const&) = delete;

        Thread& operator=(Thread&& other) noexcept;

        ~Thread();

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::NativeThreadHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

        [[nodiscard]] ThreadId GetId() const
        {
            return ThreadId{this->_id};
        }

    public:
        void Join();
        [[nodiscard]] bool IsJoinable() const;
        void Detach();
    };

    RUNTIME_API ThreadId GetThisThreadId();
}
