#pragma once
#include <compare>

namespace Anemone
{
    struct ThicknessF
    {
        float Left;
        float Top;
        float Right;
        float Bottom;

        [[nodiscard]] constexpr bool operator==(ThicknessF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(ThicknessF const& other) const = default;

        [[nodiscard]] static constexpr ThicknessF CreateUniform(float uniform);
        [[nodiscard]] static constexpr ThicknessF Create(float left, float top, float right, float bottom);

        [[nodiscard]] static constexpr ThicknessF Zero();

        [[nodiscard]] constexpr float Horizontal() const;
        [[nodiscard]] constexpr float Vertical() const;
    };
}

#include "AnemoneRuntime.Math/Thickness.inl"
