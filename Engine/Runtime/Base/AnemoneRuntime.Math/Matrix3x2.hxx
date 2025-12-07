#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

namespace Anemone
{
    struct RectF;
    struct PointF;

    struct Matrix3x2F
    {
        float M11, M12;
        float M21, M22;
        float M31, M32; // OffsetX,Y

        [[nodiscard]] constexpr bool operator==(Matrix3x2F const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(Matrix3x2F const& other) const = default;

        [[nodiscard]] static constexpr Matrix3x2F FromRectToPoly(RectF r, PointF p1, PointF p2, PointF p3);

        [[nodiscard]] static constexpr Matrix3x2F CreateIdentity();
        [[nodiscard]] static constexpr Matrix3x2F CreateNaN();

        [[nodiscard]] static Matrix3x2F CreateTransform(float scaleX, float scaleY, float angle, float offsetX, float offsetY);

        [[nodiscard]] static Matrix3x2F CreateTranslation(float positionX, float positionY);

        [[nodiscard]] static Matrix3x2F CreateRotation(float radians);
        [[nodiscard]] static Matrix3x2F CreateRotation(float radians, PointF center);

        [[nodiscard]] static Matrix3x2F CreateScale(float scaleX, float scaleY);
        [[nodiscard]] static Matrix3x2F CreateScale(float scaleX, float scaleY, PointF center);
        [[nodiscard]] static Matrix3x2F CreateScale(float scale);
        [[nodiscard]] static Matrix3x2F CreateScale(float scale, PointF center);

        [[nodiscard]] static Matrix3x2F CreateSkew(float radiansX, float radiansY);
        [[nodiscard]] static Matrix3x2F CreateSkew(float radiansX, float radiansY, PointF center);
    };

    constexpr Matrix3x2F Multiply(Matrix3x2F const& left, Matrix3x2F const& right);
    constexpr float Determinant(Matrix3x2F matrix);
    bool Invert(Matrix3x2F& result, Matrix3x2F matrix);
    constexpr PointF Transform(Matrix3x2F matrix, PointF point);
}

#include "AnemoneRuntime.Math/Matrix3x2.inl"
