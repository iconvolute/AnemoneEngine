#pragma once
#include <compare>

namespace Anemone::Math
{
    struct MarginsF
    {
        float Left;
        float Top;
        float Right;
        float Bottom;

        [[nodiscard]] constexpr bool operator==(MarginsF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(MarginsF const& other) const = default;

        [[nodiscard]] static constexpr MarginsF CreateUniform(float uniform);
        [[nodiscard]] static constexpr MarginsF Create(float left, float top, float right, float bottom);

        [[nodiscard]] static constexpr MarginsF Zero();

        [[nodiscard]] constexpr float Horizontal() const;
        [[nodiscard]] constexpr float Vertical() const;
    };
}

#include "AnemoneRuntime/Math/Margins.inl"
