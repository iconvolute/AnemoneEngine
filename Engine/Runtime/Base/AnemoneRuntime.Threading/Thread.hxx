#pragma once
#include "AnemoneRuntime.Threading/Runnable.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"

#include <optional>
#include <compare>
#include <string_view>
#include <fmt/format.h>

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
        Reference<Runnable> Callback{};
    };

    struct ThreadId
    {
        uintptr_t Inner{};

        [[nodiscard]] constexpr auto operator<=>(ThreadId const&) const = default;

        [[nodiscard]] constexpr explicit operator bool() const
        {
            return this->Inner != 0;
        }

        [[nodiscard]] static constexpr ThreadId Invalid()
        {
            return ThreadId{0};
        }
    };

}

namespace Anemone
{
    class Thread : public ReferenceCounted<Thread>
    {
    protected:
        Reference<Runnable> _runnable{};
        ThreadId _id{};

    protected:
        Thread() = default;

    public:
        Thread(Thread const&) = delete;

        Thread(Thread&&) = delete;

        Thread& operator=(Thread const&) = delete;

        Thread& operator=(Thread&&) = delete;

        virtual ~Thread();

    public:
        ThreadId Id() const
        {
            return this->_id;
        }

        virtual void Join() = 0;

        virtual bool IsJoinable() const = 0;

        virtual void Detach() = 0;

    public:
        ANEMONE_RUNTIME_BASE_API static Reference<Thread> Start(ThreadStart const& threadStart);
    };
}

template <>
struct fmt::formatter<Anemone::ThreadId> : fmt::formatter<uintptr_t>
{
    constexpr auto format(
        Anemone::ThreadId const& value,
        auto& context) const
    {
        return fmt::formatter<uintptr_t>::format(value.Inner, context);
    }
};
