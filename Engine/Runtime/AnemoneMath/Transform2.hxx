#pragma once
#include "AnemoneMath/Matrix3x2.hxx"

namespace Anemone::Math
{
    struct ScaleTransform2F final
    {
        float X;
        float Y;

        [[nodiscard]] static ScaleTransform2F Identity() noexcept
        {
            return {1.0f, 1.0f};
        }

        [[nodiscard]] static ScaleTransform2F Uniform(float scale) noexcept
        {
            return {scale, scale};
        }
    };

    struct SkewTransform2F final
    {
        float RadiansX;
        float RadiansY;

        [[nodiscard]] static SkewTransform2F Identity() noexcept
        {
            return {0.0f, 0.0f};
        }
    };

    struct TranslateTransform2F final
    {
        float X;
        float Y;

        [[nodiscard]] static TranslateTransform2F Identity() noexcept
        {
            return {0.0f, 0.0f};
        }
    };

    struct RotateTransform2F final
    {
        float Radians;

        [[nodiscard]] static RotateTransform2F Identity() noexcept
        {
            return {0.0f};
        }
    };

    struct Transform2F final
    {
        TranslateTransform2F Translate;
        RotateTransform2F Rotate;
        ScaleTransform2F Scale;
        SkewTransform2F Skew;

        [[nodiscard]] static Transform2F Identity() noexcept
        {
            return {
                TranslateTransform2F::Identity(),
                RotateTransform2F::Identity(),
                ScaleTransform2F::Identity(),
                SkewTransform2F::Identity()};
        }
    };

    [[nodiscard]] Matrix3x2F ToMatrix3x2(Transform2F const& transform);
    [[nodiscard]] Matrix3x2F ToMatrix3x2(Transform2F const& transform, PointF origin);
}

#include "AnemoneMath/Transform2.inl"
