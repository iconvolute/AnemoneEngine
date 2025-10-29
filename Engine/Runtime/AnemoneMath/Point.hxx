#pragma once
#include <compare>

namespace Anemone::Math
{
    struct PointF
    {
        float X;
        float Y;

        [[nodiscard]] constexpr bool operator==(PointF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(PointF const& other) const = default;
    };
}
