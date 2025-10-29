#pragma once
#include <compare>

namespace Anemone::Math
{
    struct SizeF
    {
        float Width;
        float Height;

        [[nodiscard]] constexpr bool operator==(SizeF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(SizeF const& other) const = default;

        [[nodiscard]] static constexpr SizeF CreateUniform(float uniform);
        [[nodiscard]] static constexpr SizeF Create(float width, float height);

        [[nodiscard]] static constexpr SizeF Zero();

        [[nodiscard]] constexpr bool IsEmpty() const;

        [[nodiscard]] constexpr float AspectRatio() const;
        [[nodiscard]] constexpr float InverseAspectRatio() const;
    };
}

#include "AnemoneMath/Size.inl"
