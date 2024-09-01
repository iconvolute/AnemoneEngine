#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Threading/Runnable.hxx"

#include <optional>
#include <string_view>

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeThreadStorage = UninitializedStorage<struct NativeThread, 16, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeThreadStorage = UninitializedStorage<struct NativeThread, 8, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeThreadStorage = UninitializedStorage<struct NativeThread, 8, 8>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Threading
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
        uintptr_t Value;

        auto operator<=>(ThreadId const&) const = default;

        static constexpr ThreadId Invalid()
        {
            return {};
        }
    };

    //! Represents a thread.
    class RUNTIME_API Thread final
    {
    private:
        Platform::NativeThreadStorage _native;

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
        void Join();
        [[nodiscard]] bool IsJoinable() const;
        [[nodiscard]] ThreadId GetId() const;
        void Detach();
    };

    RUNTIME_API ThreadId GetThisThreadId();
}
