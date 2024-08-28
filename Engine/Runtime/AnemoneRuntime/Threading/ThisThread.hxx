#pragma once
#include <cstdint>
#include <cstddef>

namespace Anemone::Threading
{
    //! Represents a thread identifier.
    struct ThreadId final
    {
        uintptr_t Value;

        auto operator<=>(ThreadId const&) const = default;

        static constexpr ThreadId Invalid()
        {
            return {};
        };
    };

    namespace ThisThread
    {
        RUNTIME_API ThreadId Id();
    }
}
