#pragma once
#include "AnemoneRuntime/Threading/Runnable.hxx"

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
