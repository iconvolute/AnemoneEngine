#pragma once
#include <cstdint>
#include <compare>
#include <cstddef>

namespace Anemone::VM
{
    struct Label
    {
        size_t Index;

        [[nodiscard]] auto constexpr operator<=>(Label const&) const = default;
    };
}
