#pragma once
#include "AnemoneRuntime.Math/Matrix3x2.hxx"
#include "AnemoneRuntime.Math/Rect.hxx"
#include "AnemoneRuntime.Math/Point.hxx"
#include "AnemoneRuntime.Math/Functions.hxx"

namespace Anemone
{
    constexpr Matrix3x2F Matrix3x2F::FromRectToPoly(RectF r, PointF p1, PointF p2, PointF p3)
    {
        float const v1x = p2.X - p1.X;
        float const v1y = p2.Y - p1.Y;
        float const v2x = p3.X - p1.X;
        float const v2y = p3.Y - p1.Y;

        float const rdx = r.X / r.Width;
        float const rdy = r.Y / r.Height;

        return Matrix3x2F{
            v1x / r.Width,
            v1y / r.Width,
            v2x / r.Height,
            v2y / r.Height,
            p1.X - (rdx * v1x) - (rdy * v2x),
            p1.Y - (rdx * v1y) - (rdy * v2y),
        };
    }

    constexpr Matrix3x2F Matrix3x2F::CreateIdentity()
    {
        return Matrix3x2F{
            1.0f, 0.0f,
            0.0f, 1.0f,
            0.0f, 0.0f};
    }

    constexpr Matrix3x2F Matrix3x2F::CreateNaN()
    {
        float const nan = std::numeric_limits<float>::quiet_NaN();

        return Matrix3x2F{
            nan, nan,
            nan, nan,
            nan, nan};
    }

    inline Matrix3x2F Matrix3x2F::CreateTransform(float scaleX, float scaleY, float angle, float offsetX, float offsetY)
    {
        float const s = Sin(angle);
        float const c = Cos(angle);

        return Matrix3x2F{
            c * scaleX, s * scaleY,
            -s * scaleX, c * scaleY,
            offsetX, offsetY
        };
    }

    inline Matrix3x2F Matrix3x2F::CreateTranslation(float positionX, float positionY)
    {
        return Matrix3x2F{
            1.0f, 0.0f,
            0.0f, 1.0f,
            positionX, positionY};
    }

    inline Matrix3x2F Matrix3x2F::CreateRotation(float radians)
    {
        float const s = Sin(radians);
        float const c = Cos(radians);

        return Matrix3x2F{
            c, s,
            -s, c,
            0.0f, 0.0f};
    }

    inline Matrix3x2F Matrix3x2F::CreateRotation(float radians, PointF center)
    {
        float const s = Sin(radians);
        float const c = Cos(radians);
        float const x = (center.X * (1.0f - c)) + (center.Y * s);
        float const y = (center.Y * (1.0f - c)) - (center.X * s);

        return Matrix3x2F{
            c, s,
            -s, c,
            x, y};
    }

    inline Matrix3x2F Matrix3x2F::CreateScale(float scaleX, float scaleY)
    {
        return Matrix3x2F{
            scaleX, 0.0f,
            0.0f, scaleY,
            0.0f, 0.0f};
    }

    inline Matrix3x2F Matrix3x2F::CreateScale(float scaleX, float scaleY, PointF center)
    {
        float const x = center.X * (1.0f - scaleX);
        float const y = center.Y * (1.0f - scaleY);

        return Matrix3x2F{
            scaleX, 0.0f,
            0.0f, scaleY,
            x, y};
    }

    inline Matrix3x2F Matrix3x2F::CreateScale(float scale)
    {
        return Matrix3x2F{
            scale, 0.0f,
            0.0f, scale,
            0.0f, 0.0f};
    }

    inline Matrix3x2F Matrix3x2F::CreateScale(float scale, PointF center)
    {
        float const x = center.X * (1.0f - scale);
        float const y = center.Y * (1.0f - scale);

        return Matrix3x2F{
            scale, 0.0f,
            0.0f, scale,
            x, y};
    }

    inline Matrix3x2F Matrix3x2F::CreateSkew(float radiansX, float radiansY)
    {
        float const xTan = Tan(radiansX);
        float const yTan = Tan(radiansY);

        return Matrix3x2F{
            1.0f, yTan,
            xTan, 1.0f,
            0.0f, 0.0f};
    }

    inline Matrix3x2F Matrix3x2F::CreateSkew(float radiansX, float radiansY, PointF center)
    {
        float const xTan = Tan(radiansX);
        float const yTan = Tan(radiansY);

        float const tx = -center.Y * xTan;
        float const ty = -center.X * yTan;

        return Matrix3x2F{
            1.0f, yTan,
            xTan, 1.0f,
            tx, ty};
    }

    constexpr float Determinant(Matrix3x2F matrix)
    {
        return (matrix.M11 * matrix.M22) - (matrix.M12 * matrix.M21);
    }

    inline bool Invert(Matrix3x2F& result, Matrix3x2F matrix)
    {
        float const det = (matrix.M11 * matrix.M22) - (matrix.M12 * matrix.M21);

        if (IsNearZero(det))
        {
            result = Matrix3x2F::CreateNaN();
            return false;
        }

        float const invDet = 1.0f / det;

        result.M11 = +matrix.M22 * invDet;
        result.M12 = -matrix.M12 * invDet;

        result.M21 = -matrix.M21 * invDet;
        result.M22 = +matrix.M11 * invDet;

        result.M31 = ((matrix.M21 * matrix.M32) - (matrix.M31 * matrix.M22)) * invDet;
        result.M32 = ((matrix.M31 * matrix.M12) - (matrix.M11 * matrix.M32)) * invDet;
        return true;
    }

    constexpr PointF Transform(Matrix3x2F matrix, PointF point)
    {
        return PointF{
            (point.X * matrix.M11) + (point.Y * matrix.M21) + matrix.M31,
            (point.X * matrix.M12) + (point.Y * matrix.M22) + matrix.M32};
    }

    // TODO: Use Vector2F when available
    constexpr PointF TransformVector(Matrix3x2F matrix, PointF vector)
    {
        return PointF{
            (vector.X * matrix.M11) + (vector.Y * matrix.M21),
            (vector.X * matrix.M12) + (vector.Y * matrix.M22)};
    }

    constexpr Matrix3x2F Multiply(Matrix3x2F const& left, Matrix3x2F const& right)
    {
        return Matrix3x2F{
            (left.M11 * right.M11) + (left.M12 * right.M21),
            (left.M11 * right.M12) + (left.M12 * right.M22),
            (left.M21 * right.M11) + (left.M22 * right.M21),
            (left.M21 * right.M12) + (left.M22 * right.M22),
            (left.M31 * right.M11) + (left.M32 * right.M21) + right.M31,
            (left.M31 * right.M12) + (left.M32 * right.M22) + right.M32,
        };
    }
}
