#pragma once
#include <compare>
#include <span>
#include <optional>

namespace Anemone::UI
{
    struct RectangleF;
    struct Point2F;
    struct Point3F;
    struct Vector2F;
    struct Vector3F;

    enum class MatrixOrder
    {
        Prepend,
        Append,
    };

    struct MatrixF
    {
        float M11;
        float M12;
        float M21;
        float M22;
        float DX;
        float DY;

        constexpr MatrixF FromRectangleToPolygon(RectangleF const& r, Point2F const& p1, Point2F const& p2, Point2F const& p3);

        [[nodiscard]] constexpr bool operator==(MatrixF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(MatrixF const& other) const = default;

        static constexpr MatrixF Identity();

        constexpr bool IsIdentity() const;

        constexpr bool IsInvertible() const;

        constexpr float Determinant() const;

        constexpr Point2F Transform(Point2F const& value) const;
        constexpr Point3F Transform(Point3F const& value) const;

        constexpr Vector2F Transform(Vector2F const& value) const;
        constexpr Vector3F Transform(Vector3F const& value) const;

        constexpr void Transform(std::span<Point2F> values) const;
        constexpr void Transform(std::span<Point3F> values) const;

        constexpr void Transform(std::span<Vector2F> values) const;
        constexpr auto Transform(std::span<Vector3F> values) const -> void;

        constexpr std::optional<MatrixF> Inverse() const;

        static constexpr MatrixF Multiply(MatrixF const& left, MatrixF const& right);

        constexpr void Multiply(MatrixF const& other);
        constexpr auto Multiply(MatrixF const& other, MatrixOrder order) -> void;

        constexpr void Reset();

        void Rotate(float angle);
        void Rotate(float angle, MatrixOrder order);

        void RotateAt(float angle, Point2F const& point);
        void RotateAt(float angle, Point2F const& point, MatrixOrder order);

        constexpr void Translate(float x, float y);
        constexpr void Translate(float x, float y, MatrixOrder order);

        constexpr void Scale(float x, float y);
        constexpr void Scale(float x, float y, MatrixOrder order);

        constexpr void ScaleAt(float x, float y, Point2F const& point);
        constexpr void ScaleAt(float x, float y, Point2F const& point, MatrixOrder order);

        constexpr void Shear(float x, float y);
        constexpr void Shear(float x, float y, MatrixOrder order);

        constexpr void ShearAt(float x, float y, Point2F const& point);
        constexpr void ShearAt(float x, float y, Point2F const& point, MatrixOrder order);
    };
}

#include <AnemoneUI/PointF.hxx>
#include <AnemoneUI/VectorF.hxx>
#include <AnemoneUI/RectangleF.hxx>

namespace Anemone::UI
{
    constexpr MatrixF MatrixF::FromRectangleToPolygon(RectangleF const& r, Point2F const& p1, Point2F const& p2, Point2F const& p3)
    {
        float const v1x = p2.X - p1.X;
        float const v1y = p2.Y - p1.Y;
        float const v2x = p3.X - p1.X;
        float const v2y = p3.Y - p1.Y;

        float const rdx = r.X / r.Width;
        float const rdy = r.Y / r.Height;

        return MatrixF{
            .M11 = v1x / r.Width,
            .M12 = v1y / r.Width,
            .M21 = v2x / r.Height,
            .M22 = v2y / r.Height,
            .DX = p1.X - (rdx * v1x) - (rdy * v2x),
            .DY = p1.Y - (rdx * v1y) - (rdy * v2y),
        };
    }

    constexpr MatrixF MatrixF::Identity()
    {
        return MatrixF{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    }

    constexpr bool MatrixF::IsIdentity() const
    {
        return (*this) == MatrixF::Identity();
    }

    constexpr bool MatrixF::IsInvertible() const
    {
        return this->Determinant() != 0.0f;
    }

    constexpr float MatrixF::Determinant() const
    {
        return (this->M11 * this->M22) - (this->M12 * this->M21);
    }

    constexpr Point2F MatrixF::Transform(Point2F const& value) const
    {
        return Point2F{
            (value.X * this->M11) + (value.Y * this->M21) + this->DX,
            (value.X * this->M12) + (value.Y * this->M22) + this->DY,
        };
    }

    constexpr Point3F MatrixF::Transform(Point3F const& value) const
    {
        return Point3F{
            (value.X * this->M11) + (value.Y * this->M21) + this->DX,
            (value.X * this->M12) + (value.Y * this->M22) + this->DY,
            value.Z,
        };
    }

    constexpr Vector2F MatrixF::Transform(Vector2F const& value) const
    {
        return Vector2F{
            (value.X * this->M11) + (value.Y * this->M21),
            (value.X * this->M12) + (value.Y * this->M22),
        };
    }

    constexpr Vector3F MatrixF::Transform(Vector3F const& value) const
    {
        return Vector3F{
            (value.X * this->M11) + (value.Y * this->M21),
            (value.X * this->M12) + (value.Y * this->M22),
            value.Z,
        };
    }

    constexpr void MatrixF::Transform(std::span<Point2F> values) const
    {
        for (auto& value : values)
        {
            value = this->Transform(value);
        }
    }

    constexpr void MatrixF::Transform(std::span<Point3F> values) const
    {
        for (auto& value : values)
        {
            value = this->Transform(value);
        }
    }

    constexpr void MatrixF::Transform(std::span<Vector2F> values) const
    {
        for (auto& value : values)
        {
            value = this->Transform(value);
        }
    }

    constexpr void MatrixF::Transform(std::span<Vector3F> values) const
    {
        for (auto& value : values)
        {
            value = this->Transform(value);
        }
    }

    constexpr std::optional<MatrixF> MatrixF::Inverse() const
    {
        auto const det = this->Determinant();
        if (det == 0.0f)
        {
            return std::nullopt;
        }

        auto const inv_det = 1.0f / det;
        return MatrixF{
            this->M22 * inv_det,
            -this->M12 * inv_det,
            -this->M21 * inv_det,
            this->M11 * inv_det,
            ((this->M21 * this->DY) - (this->M22 * this->DX)) * inv_det,
            ((this->M12 * this->DX) - (this->M11 * this->DY)) * inv_det,
        };
    }

    constexpr MatrixF MatrixF::Multiply(MatrixF const& left, MatrixF const& right)
    {
        return MatrixF{
            (left.M11 * right.M11) + (left.M12 * right.M21),
            (left.M11 * right.M12) + (left.M12 * right.M22),
            (left.M21 * right.M11) + (left.M22 * right.M21),
            (left.M21 * right.M12) + (left.M22 * right.M22),
            (left.DX * right.M11) + (left.DY * right.M21) + right.DX,
            (left.DX * right.M12) + (left.DY * right.M22) + right.DY,
        };
    }

    constexpr void MatrixF::Multiply(MatrixF const& other)
    {
        *this = MatrixF::Multiply(*this, other);
    }

    constexpr void MatrixF::Multiply(MatrixF const& other, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            *this = MatrixF::Multiply(other, *this);
        }
        else
        {
            *this = MatrixF::Multiply(*this, other);
        }
    }

    constexpr void MatrixF::Reset()
    {
        *this = MatrixF::Identity();
    }

    inline void MatrixF::Rotate(float angle)
    {
        float const cos_angle = std::cos(angle);
        float const sin_angle = std::sin(angle);

        float const m11 = (cos_angle * this->M11) + (sin_angle * this->M21);
        float const m12 = (cos_angle * this->M12) + (sin_angle * this->M22);
        float const m21 = (cos_angle * this->M21) - (sin_angle * this->M11);
        float const m22 = (cos_angle * this->M22) - (sin_angle * this->M12);

        this->M11 = m11;
        this->M12 = m12;
        this->M21 = m21;
        this->M22 = m22;
    }

    inline void MatrixF::Rotate(float angle, MatrixOrder order)
    {
        float const cos_angle = std::cos(angle);
        float const sin_angle = std::sin(angle);

        if (order == MatrixOrder::Prepend)
        {
            float const m11 = (cos_angle * this->M11) + (sin_angle * this->M21);
            float const m12 = (cos_angle * this->M12) + (sin_angle * this->M22);
            float const m21 = (cos_angle * this->M21) - (sin_angle * this->M11);
            float const m22 = (cos_angle * this->M22) - (sin_angle * this->M12);

            this->M11 = m11;
            this->M12 = m12;
            this->M21 = m21;
            this->M22 = m22;
        }
        else
        {
            float const m11 = (cos_angle * this->M11) - (sin_angle * this->M12);
            float const m12 = (sin_angle * this->M11) + (cos_angle * this->M12);
            float const m21 = (cos_angle * this->M21) - (sin_angle * this->M22);
            float const m22 = (sin_angle * this->M21) + (cos_angle * this->M22);
            float const dx = (cos_angle * this->DX) - (sin_angle * this->DY);
            float const dy = (sin_angle * this->DX) + (cos_angle * this->DY);

            this->M11 = m11;
            this->M12 = m12;
            this->M21 = m21;
            this->M22 = m22;
            this->DX = dx;
            this->DY = dy;
        }
    }

    inline void MatrixF::RotateAt(float angle, Point2F const& point)
    {
        this->Translate(-point.X, -point.Y);
        this->Rotate(angle);
        this->Translate(point.X, point.Y);
    }

    inline void MatrixF::RotateAt(float angle, Point2F const& point, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            this->Translate(point.X, point.Y);
            this->Rotate(angle, MatrixOrder::Prepend);
            this->Translate(-point.X, -point.Y);
        }
        else
        {
            this->Translate(-point.X, -point.Y);
            this->Rotate(angle, MatrixOrder::Append);
            this->Translate(point.X, point.Y);
        }
    }

    constexpr void MatrixF::Translate(float x, float y)
    {
        this->DX += (x * this->M11) + (y * this->M21);
        this->DY += (x * this->M12) + (y * this->M22);
    }

    constexpr void MatrixF::Translate(float x, float y, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            this->DX += (x * this->M11) + (y * this->M21);
            this->DY += (x * this->M12) + (y * this->M22);
        }
        else
        {
            this->DX += x;
            this->DY += y;
        }
    }

    constexpr void MatrixF::Scale(float x, float y)
    {
        this->M11 *= x;
        this->M12 *= x;
        this->M21 *= y;
        this->M22 *= y;
    }

    constexpr void MatrixF::Scale(float x, float y, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            this->M11 *= x;
            this->M12 *= x;
            this->M21 *= y;
            this->M22 *= y;
        }
        else
        {
            this->M11 *= x;
            this->M12 *= y;
            this->M21 *= x;
            this->M22 *= y;
            this->DX *= x;
            this->DY *= y;
        }
    }

    constexpr void MatrixF::ScaleAt(float x, float y, Point2F const& point)
    {
        this->Translate(-point.X, -point.Y);
        this->Scale(x, y);
        this->Translate(point.X, point.Y);
    }

    constexpr void MatrixF::ScaleAt(float x, float y, Point2F const& point, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            this->Translate(point.X, point.Y);
            this->Scale(x, y, MatrixOrder::Prepend);
            this->Translate(-point.X, -point.Y);
        }
        else
        {
            this->Translate(-point.X, -point.Y);
            this->Scale(x, y, MatrixOrder::Append);
            this->Translate(point.X, point.Y);
        }
    }

    constexpr void MatrixF::Shear(float x, float y)
    {
        float const m11 = this->M11 + (y * this->M21);
        float const m12 = this->M12 + (y * this->M22);
        float const m21 = (x * this->M11) + this->M21;
        float const m22 = (y * this->M12) + this->M22;

        this->M11 = m11;
        this->M12 = m12;
        this->M21 = m21;
        this->M22 = m22;
    }

    constexpr void MatrixF::Shear(float x, float y, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            float const m11 = this->M11 + (y * this->M21);
            float const m12 = this->M12 + (y * this->M22);
            float const m21 = (x * this->M11) + this->M21;
            float const m22 = (y * this->M12) + this->M22;

            this->M11 = m11;
            this->M12 = m12;
            this->M21 = m21;
            this->M22 = m22;
        }
        else
        {
            float const m11 = this->M11 + (x * this->M12);
            float const m12 = (y * this->M11) + this->M12;
            float const m21 = this->M21 + (x * this->M22);
            float const m22 = (y * this->M21) + this->M22;
            float const dx = this->DX + (x * this->DY);
            float const dy = (y * this->DX) + this->DY;

            this->M11 = m11;
            this->M12 = m12;
            this->M21 = m21;
            this->M22 = m22;
            this->DX = dx;
            this->DY = dy;
        }
    }

    constexpr void MatrixF::ShearAt(float x, float y, Point2F const& point)
    {
        this->Translate(-point.X, -point.Y);
        this->Shear(x, y);
        this->Translate(point.X, point.Y);
    }

    constexpr void MatrixF::ShearAt(float x, float y, Point2F const& point, MatrixOrder order)
    {
        if (order == MatrixOrder::Prepend)
        {
            this->Translate(point.X, point.Y);
            this->Shear(x, y, MatrixOrder::Prepend);
            this->Translate(-point.X, -point.Y);
        }
        else
        {
            this->Translate(-point.X, -point.Y);
            this->Shear(x, y, MatrixOrder::Append);
            this->Translate(point.X, point.Y);
        }
    }
}
