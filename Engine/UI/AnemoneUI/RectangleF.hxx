#pragma once
#include <span>

namespace Anemone::UI
{
    struct Point2F;
    struct Vector2F;
    struct Size2F;
    struct ThicknessF;

    struct RectangleF
    {
    public:
        float X;
        float Y;
        float Width;
        float Height;

    public:
        [[nodiscard]] constexpr bool operator==(RectangleF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(RectangleF const& other) const = default;

    public:
        static constexpr RectangleF FromLocationSpan(Point2F const& origin, Vector2F const& span);
        static constexpr RectangleF FromLocationSize(Point2F const& origin, Size2F const& size);
        static constexpr RectangleF FromLocationSize(float x, float y, float width, float height);
        static constexpr RectangleF FromLocationSize(float x, float y, float size);
        static constexpr RectangleF FromBounds(float left, float top, float right, float bottom);
        static constexpr RectangleF FromSize(Size2F const& size);
        static constexpr RectangleF FromCenterSize(Point2F const& center, Size2F const& size);
        static constexpr RectangleF FromCentered(Size2F const& size);
        static constexpr RectangleF FromCentered(ThicknessF const& margins);
        static constexpr RectangleF FromPoints(Point2F const& p1, Point2F const& p2);
        static constexpr RectangleF FromPoints(std::span<Point2F> points);

    public:
        constexpr float GetLeft() const;
        constexpr float GetTop() const;
        constexpr float GetRight() const;
        constexpr float GetBottom() const;

        constexpr Point2F GetLocation() const;
        constexpr void SetLocation(Point2F const& location);

        constexpr Size2F GetSize() const;
        constexpr void SetSize(Size2F const& size);

        constexpr Point2F GetCenter() const;
        constexpr void SetCenter(Point2F const& center);

        constexpr Point2F GetTopLeft() const;
        constexpr Point2F GetTopRight() const;
        constexpr Point2F GetBottomLeft() const;
        constexpr Point2F GetBottomRight() const;

        constexpr bool IsEmpty() const;

    public:
        constexpr RectangleF& Scale(float factor);
        constexpr RectangleF& Scale(float x, float y);

        constexpr RectangleF& ScaleAtCenter(float factor);
        constexpr RectangleF& ScaleAtCenter(float x, float y);

        constexpr RectangleF& Inflate(float value);
        constexpr RectangleF& Inflate(float horizontal, float vertical);
        constexpr RectangleF& Inflate(float left, float top, float right, float bottom);
        constexpr RectangleF& Inflate(ThicknessF const& value);

        constexpr RectangleF& Deflate(float value);
        constexpr RectangleF& Deflate(float horizontal, float vertical);
        constexpr RectangleF& Deflate(float left, float top, float right, float bottom);
        constexpr RectangleF& Deflate(ThicknessF const& value);

        constexpr RectangleF& Translate(float x, float y);
        constexpr RectangleF& Translate(Vector2F const& vector);
    };
}


#include <AnemoneUI/PointF.hxx>
#include <AnemoneUI/ThicknessF.hxx>
#include <AnemoneUI/SizeF.hxx>
#include <AnemoneUI/VectorF.hxx>

namespace Anemone::UI
{
    constexpr RectangleF RectangleF::FromLocationSpan(Point2F const& origin, Vector2F const& span)
    {
        return RectangleF{
            .X = origin.X,
            .Y = origin.Y,
            .Width = span.X,
            .Height = span.Y,
        };
    }

    constexpr RectangleF RectangleF::FromLocationSize(Point2F const& origin, Size2F const& size)
    {
        return RectangleF{
            .X = origin.X,
            .Y = origin.Y,
            .Width = size.Width,
            .Height = size.Height,
        };
    }

    constexpr RectangleF RectangleF::FromLocationSize(float x, float y, float width, float height)
    {
        return RectangleF{
            .X = x,
            .Y = y,
            .Width = width,
            .Height = height,
        };
    }

    constexpr RectangleF RectangleF::FromLocationSize(float x, float y, float size)
    {
        return RectangleF{
            .X = x,
            .Y = y,
            .Width = size,
            .Height = size,
        };
    }

    constexpr RectangleF RectangleF::FromBounds(float left, float top, float right, float bottom)
    {
        return RectangleF{
            .X = left,
            .Y = top,
            .Width = right - left,
            .Height = bottom - top,
        };
    }

    constexpr RectangleF RectangleF::FromSize(Size2F const& size)
    {
        return RectangleF{
            .X = 0.0f,
            .Y = 0.0f,
            .Width = size.Width,
            .Height = size.Height,
        };
    }

    constexpr RectangleF RectangleF::FromCenterSize(Point2F const& center, Size2F const& size)
    {
        return RectangleF{
            .X = center.X - (size.Width * 0.5f),
            .Y = center.Y - (size.Height * 0.5f),
            .Width = size.Width,
            .Height = size.Height,
        };
    }

    constexpr RectangleF RectangleF::FromCentered(Size2F const& size)
    {
        return RectangleF{
            .X = -size.Width * 0.5f,
            .Y = -size.Height * 0.5f,
            .Width = size.Width,
            .Height = size.Height,
        };
    }

    constexpr RectangleF RectangleF::FromCentered(ThicknessF const& margins)
    {
        float const width = margins.Left + margins.Right;
        float const height = margins.Top + margins.Bottom;

        return RectangleF{
            .X = -width * 0.5f,
            .Y = -height * 0.5f,
            .Width = width,
            .Height = height,
        };
    }

    constexpr RectangleF RectangleF::FromPoints(Point2F const& p1, Point2F const& p2)
    {
        float const minX = std::min(p1.X, p2.X);
        float const minY = std::min(p1.Y, p2.Y);
        float const maxX = std::max(p1.X, p2.X);
        float const maxY = std::max(p1.Y, p2.Y);

        return RectangleF{
            .X = minX,
            .Y = minY,
            .Width = maxX - minX,
            .Height = maxY - minY,
        };
    }

    constexpr RectangleF RectangleF::FromPoints(std::span<Point2F> points)
    {
        if (points.empty())
        {
            return RectangleF{};
        }

        if (points.size() == 1)
        {
            return RectangleF{
                .X = points[0].X,
                .Y = points[0].Y,
                .Width = 0.0f,
                .Height = 0.0f,
            };
        }

        float minX = points[0].X;
        float minY = points[0].Y;
        float maxX = points[0].X;
        float maxY = points[0].Y;

        for (size_t i = 1; i < points.size(); ++i)
        {
            minX = std::min(minX, points[i].X);
            minY = std::min(minY, points[i].Y);
            maxX = std::max(maxX, points[i].X);
            maxY = std::max(maxY, points[i].Y);
        }

        return RectangleF{
            .X = minX,
            .Y = minY,
            .Width = maxX - minX,
            .Height = maxY - minY,
        };
    }

    constexpr float RectangleF::GetLeft() const
    {
        return this->X;
    }

    constexpr float RectangleF::GetTop() const
    {
        return this->Y;
    }

    constexpr float RectangleF::GetRight() const
    {
        return this->X + this->Width;
    }

    constexpr float RectangleF::GetBottom() const
    {
        return this->Y + this->Height;
    }

    constexpr Point2F RectangleF::GetLocation() const
    {
        return Point2F{
            .X = this->X,
            .Y = this->Y,
        };
    }

    constexpr void RectangleF::SetLocation(Point2F const& location)
    {
        this->X = location.X;
        this->Y = location.Y;
    }

    constexpr Size2F RectangleF::GetSize() const
    {
        return Size2F{
            .Width = this->Width,
            .Height = this->Height,
        };
    }

    constexpr void RectangleF::SetSize(Size2F const& size)
    {
        this->Width = size.Width;
        this->Height = size.Height;
    }

    constexpr Point2F RectangleF::GetCenter() const
    {
        return Point2F{
            .X = this->X + (this->Width * 0.5f),
            .Y = this->Y + (this->Height * 0.5f),
        };
    }

    constexpr void RectangleF::SetCenter(Point2F const& center)
    {
        this->X = center.X - (this->Width * 0.5f);
        this->Y = center.Y - (this->Height * 0.5f);
    }

    constexpr Point2F RectangleF::GetTopLeft() const
    {
        return Point2F{
            .X = this->X,
            .Y = this->Y,
        };
    }

    constexpr Point2F RectangleF::GetTopRight() const
    {
        return Point2F{
            .X = this->X + this->Width,
            .Y = this->Y,
        };
    }

    constexpr Point2F RectangleF::GetBottomLeft() const
    {
        return Point2F{
            .X = this->X,
            .Y = this->Y + this->Height,
        };
    }

    constexpr Point2F RectangleF::GetBottomRight() const
    {
        return Point2F{
            .X = this->X + this->Width,
            .Y = this->Y + this->Height,
        };
    }

    constexpr bool RectangleF::IsEmpty() const
    {
        return (this->Width <= 0.0f) or (this->Height <= 0.0f);
    }

    constexpr RectangleF& RectangleF::Scale(float factor)
    {
        // Scale rectangle size.
        this->Width *= factor;
        this->Height *= factor;

        return *this;
    }

    constexpr RectangleF& RectangleF::Scale(float x, float y)
    {
        // Scale rectangle size.
        this->Width *= x;
        this->Height *= y;

        return *this;
    }

    constexpr RectangleF& RectangleF::ScaleAtCenter(float factor)
    {
        // Get center of the rectangle.
        float const centerX = this->X + (this->Width * 0.5f);
        float const centerY = this->Y + (this->Height * 0.5f);

        // Scale it's size.
        this->Width *= factor;
        this->Height *= factor;

        // Move the rectangle back to its original position.
        this->X = centerX - (this->Width * 0.5f);
        this->Y = centerY - (this->Height * 0.5f);

        return *this;
    }

    constexpr RectangleF& RectangleF::ScaleAtCenter(float x, float y)
    {
        // Get center of the rectangle.
        float const centerX = this->X + (this->Width * 0.5f);
        float const centerY = this->Y + (this->Height * 0.5f);

        // Scale it's size.
        this->Width *= x;
        this->Height *= y;

        // Move the rectangle back to its original position.
        this->X = centerX - (this->Width * 0.5f);
        this->Y = centerY - (this->Height * 0.5f);

        return *this;
    }

    constexpr RectangleF& RectangleF::Inflate(float value)
    {
        this->X -= value;
        this->Y -= value;
        this->Width += value * 2.0f;
        this->Height += value * 2.0f;

        return *this;
    }

    constexpr RectangleF& RectangleF::Inflate(float horizontal, float vertical)
    {
        this->X -= horizontal;
        this->Y -= vertical;
        this->Width += horizontal * 2.0f;
        this->Height += vertical * 2.0f;

        return *this;
    }

    constexpr RectangleF& RectangleF::Inflate(float left, float top, float right, float bottom)
    {
        this->X -= left;
        this->Y -= top;
        this->Width += left + right;
        this->Height += top + bottom;

        return *this;
    }

    constexpr RectangleF& RectangleF::Inflate(ThicknessF const& value)
    {
        this->X -= value.Left;
        this->Y -= value.Top;
        this->Width += value.Left + value.Right;
        this->Height += value.Top + value.Bottom;

        return *this;
    }

    constexpr RectangleF& RectangleF::Deflate(float value)
    {
        this->X += value;
        this->Y += value;
        this->Width -= value * 2.0f;
        this->Height -= value * 2.0f;

        return *this;
    }

    constexpr RectangleF& RectangleF::Deflate(float horizontal, float vertical)
    {
        this->X += horizontal;
        this->Y += vertical;
        this->Width -= horizontal * 2.0f;
        this->Height -= vertical * 2.0f;

        return *this;
    }

    constexpr RectangleF& RectangleF::Deflate(float left, float top, float right, float bottom)
    {
        this->X += left;
        this->Y += top;
        this->Width -= left + right;
        this->Height -= top + bottom;

        return *this;
    }

    constexpr RectangleF& RectangleF::Deflate(ThicknessF const& value)
    {
        this->X += value.Left;
        this->Y += value.Top;
        this->Width -= value.Left + value.Right;
        this->Height -= value.Top + value.Bottom;

        return *this;
    }

    constexpr RectangleF& RectangleF::Translate(float x, float y)
    {
        this->X += x;
        this->Y += y;

        return *this;
    }

    constexpr RectangleF& RectangleF::Translate(Vector2F const& vector)
    {
        this->X += vector.X;
        this->Y += vector.Y;

        return *this;
    }
}

namespace Anemone::UI
{
    constexpr RectangleF Transposed(RectangleF const& self)
    {
        return RectangleF{
            .X = self.Y,
            .Y = self.X,
            .Width = self.Height,
            .Height = self.Width,
        };
    }

    constexpr RectangleF CenterTransposed(RectangleF const& self)
    {
        float const centerX = self.X + (self.Width * 0.5f);
        float const centerY = self.Y + (self.Height * 0.5f);
        float const width = self.Height;
        float const height = self.Width;

        return RectangleF{
            .X = centerX - (width * 0.5f),
            .Y = centerY - (height * 0.5f),
            .Width = width,
            .Height = height,
        };
    }

    constexpr bool Contains(RectangleF const& self, RectangleF const& other)
    {
        return (self.GetLeft() <= other.GetLeft()) and (self.GetTop() <= other.GetTop()) and (other.GetRight() <= self.GetRight()) and (other.GetBottom() <= self.GetBottom());
    }

    constexpr bool Contains(RectangleF const& self, float x, float y)
    {
        float const minX = self.X;
        float const minY = self.Y;
        float const maxX = self.X + self.Width;
        float const maxY = self.Y + self.Height;

        return ((minX <= x) and (x <= maxX)) and ((minY <= y) and (y <= maxY));
    }

    constexpr bool Contains(RectangleF const& self, Point2F const& point)
    {
        return Contains(self, point.X, point.Y);
    }

    constexpr bool Intersects(RectangleF const& self, RectangleF const& other)
    {
        return (self.GetLeft() < other.GetRight()) and (self.GetRight() > other.GetLeft()) and (self.GetTop() < other.GetBottom()) and (self.GetBottom() > other.GetTop());
    }

    constexpr RectangleF UnionWith(RectangleF const& self, RectangleF const& other)
    {
        float const x = std::min(self.X, other.X);
        float const y = std::min(self.Y, other.Y);

        return RectangleF{
            .X = x,
            .Y = y,
            .Width = std::max(self.X + self.Width, other.X + other.Width) - x,
            .Height = std::max(self.Y + self.Height, other.Y + other.Height) - y,
        };
    }

    constexpr RectangleF UnionWith(RectangleF const& self, Point2F const& other)
    {
        float const x = std::min(self.X, other.X);
        float const y = std::min(self.Y, other.Y);

        return RectangleF{
            .X = x,
            .Y = y,
            .Width = std::max(self.X + self.Width, other.X) - x,
            .Height = std::max(self.Y + self.Height, other.Y) - y,
        };
    }

    constexpr RectangleF IntersectionWith(RectangleF const& self, RectangleF const& other)
    {
        float const x = std::max(self.X, other.X);
        float const y = std::max(self.Y, other.Y);
        float const width = std::min(self.X + self.Width, other.X + other.Width) - x;
        float const height = std::min(self.Y + self.Height, other.Y + other.Height) - y;

        if (width <= 0.0f or height <= 0.0f)
        {
            return RectangleF{
                .X = std::numeric_limits<float>::infinity(),
                .Y = std::numeric_limits<float>::infinity(),
                .Width = std::numeric_limits<float>::infinity(),
                .Height = std::numeric_limits<float>::infinity(),
            };
        }

        return RectangleF{
            .X = x,
            .Y = y,
            .Width = width,
            .Height = height,
        };
    }

    constexpr Point2F Clip(RectangleF const& self, Point2F const& point)
    {
        float const minX = self.X;
        float const minY = self.Y;
        float const maxX = self.X + self.Width;
        float const maxY = self.Y + self.Height;

        return Point2F{
            .X = std::min(std::max(point.X, minX), maxX),
            .Y = std::min(std::max(point.Y, minY), maxY),
        };
    }
}
