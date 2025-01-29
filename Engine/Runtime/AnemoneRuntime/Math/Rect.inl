#pragma once
#include <algorithm>

#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Thickness.hxx"
#include "AnemoneRuntime/Math/Functions.hxx"

namespace Anemone::Math
{
    constexpr RectF RectF::FromLocationSize(float x, float y, float width, float height)
    {
        return {x, y, width, height};
    }

    constexpr RectF RectF::FromLocationSize(float x, float y, float size)
    {
        return {x, y, size, size};
    }

    constexpr RectF RectF::FromBounds(float left, float top, float right, float bottom)
    {
        return {left, top, right - left, bottom - top};
    }

    constexpr RectF RectF::FromPoints(PointF p1, PointF p2)
    {
        return {p1.X, p1.Y, p2.X - p1.X, p2.Y - p1.Y};
    }

    constexpr RectF RectF::FromLocationSize(PointF location, SizeF size)
    {
        return {location.X, location.Y, size.Width, size.Height};
    }

    constexpr RectF RectF::FromCenterSize(PointF location, SizeF size)
    {
        return {location.X - (size.Width / 2.0f), location.Y - (size.Height / 2.0f), size.Width, size.Height};
    }

    constexpr RectF RectF::Empty()
    {
        return {};
    }

    constexpr RectF RectF::NaN()
    {
        float const nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan, nan, nan};
    }

    constexpr RectF RectF::FromPoints(std::span<const PointF> points)
    {
        if (points.empty())
        {
            return RectF::CreateNaN();
        }

        float minX = points[0].X;
        float minY = points[0].Y;

        float maxX = minX;
        float maxY = minY;

        for (size_t i = 1; i < points.size(); ++i)
        {
            minX = std::min(points[i].X, minX);
            minY = std::min(points[i].Y, minY);
            maxX = std::max(points[i].X, maxX);
            maxY = std::max(points[i].Y, maxY);
        }

        return RectF::CreateBounds(minX, minY, maxX, maxY);
    }

    constexpr float RectF::Left() const
    {
        return this->X;
    }

    constexpr float RectF::Top() const
    {
        return this->Y;
    }

    constexpr float RectF::Right() const
    {
        return this->X + this->Width;
    }

    constexpr float RectF::Bottom() const
    {
        return this->Y + this->Height;
    }

    constexpr PointF RectF::TopLeft() const
    {
        return {this->X, this->Y};
    }

    constexpr PointF RectF::TopRight() const
    {
        return {this->X + this->Width, this->Y};
    }

    constexpr PointF RectF::BottomLeft() const
    {
        return {this->X, this->Y + this->Height};
    }

    constexpr PointF RectF::BottomRight() const
    {
        return {this->X + this->Width, this->Y + this->Height};
    }

    constexpr SizeF RectF::Size() const
    {
        return {this->Width, this->Height};
    }

    constexpr PointF RectF::Location() const
    {
        return {this->X, this->Y};
    }

    constexpr PointF RectF::Center() const
    {
        return {this->X + (this->Width / 2.0f), this->Y + (this->Height / 2.0f)};
    }

    constexpr bool RectF::IsEmpty() const
    {
        return (this->Width <= 0.0f) || (this->Height <= 0.0f);
    }

    inline bool RectF::IsNaN() const
    {
        return std::isnan(this->X) || std::isnan(this->Y) || std::isnan(this->Width) || std::isnan(this->Height);
    }
}

namespace Anemone::Math
{
    constexpr RectF Inflate(RectF self, float value)
    {
        return {self.X - value, self.Y - value, self.Width + (2.0f * value), self.Height + (2.0f * value)};
    }

    constexpr RectF Inflate(RectF self, float horizontal, float vertical)
    {
        return {self.X - horizontal, self.Y - vertical, self.Width + (2.0f * horizontal), self.Height + (2.0f * vertical)};
    }

    constexpr RectF Inflate(RectF self, float left, float top, float right, float bottom)
    {
        return {self.X - left, self.Y - top, self.Width + left + right, self.Height + top + bottom};
    }

    constexpr RectF Inflate(RectF self, ThicknessF border)
    {
        return Inflate(self, border.Left, border.Top, border.Right, border.Bottom);
    }

    constexpr RectF Deflate(RectF self, float value)
    {
        return {self.X + value, self.Y + value, self.Width - (2.0f * value), self.Height - (2.0f * value)};
    }

    constexpr RectF Deflate(RectF self, float horizontal, float vertical)
    {
        return {self.X + horizontal, self.Y + vertical, self.Width - (2.0f * horizontal), self.Height - (2.0f * vertical)};
    }

    constexpr RectF Deflate(RectF self, float left, float top, float right, float bottom)
    {
        return {self.X + left, self.Y + top, self.Width - left - right, self.Height - top - bottom};
    }

    constexpr RectF Deflate(RectF self, ThicknessF border)
    {
        return Deflate(self, border.Left, border.Top, border.Right, border.Bottom);
    }

    constexpr RectF Translate(RectF self, float x, float y)
    {
        return {self.X + x, self.Y + y, self.Width, self.Height};
    }

    constexpr bool Contains(RectF self, float x, float y)
    {
        return (self.X <= x) && (x < (self.X + self.Width)) && (self.Y <= y) && (y < (self.Y + self.Height));
    }

    constexpr bool Contains(RectF self, PointF point)
    {
        return Contains(self, point.X, point.Y);
    }

    constexpr bool Contains(RectF self, RectF other)
    {
        return (self.X <= other.X) && ((other.X + other.Width) <= (self.X + self.Width)) && (self.Y <= other.Y) && ((other.Y + other.Height) <= (self.Y + self.Height));
    }

    constexpr RectF Intersect(RectF self, RectF other)
    {
        float const rightSelf = self.X + self.Width;
        float const rightOther = other.X + other.Width;

        float const bottomSelf = self.Y + self.Height;
        float const bottomOther = other.Y + other.Height;

        float const maxX = (self.X > other.X) ? self.X : other.X;
        float const maxY = (self.Y > other.Y) ? self.Y : other.Y;

        float const minX = (rightSelf < rightOther) ? rightSelf : rightOther;
        float const minY = (bottomSelf < bottomOther) ? bottomSelf : bottomOther;

        RectF result;

        if ((minX > maxX) and (minY > maxY))
        {
            result.X = maxX;
            result.Y = maxY;
            result.Width = minX - maxX;
            result.Height = minY - maxY;
        }
        else
        {
            result.X = 0.0f;
            result.Y = 0.0f;
            result.Width = 0.0f;
            result.Height = 0.0f;
        }

        return result;
    }

    constexpr bool IntersectsWith(RectF self, RectF other)
    {
        return (other.X < (self.X + self.Width)) && (self.X < (other.X + other.Width)) && (other.Y < (self.Y + self.Height)) && (self.Y < (other.Y + other.Height));
    }

    constexpr RectF Union(RectF self, RectF other)
    {
        float const minX = (self.X < other.X) ? self.X : other.X;
        float const minY = (self.Y < other.Y) ? self.Y : other.Y;

        float const rightSelf = self.X + self.Width;
        float const rightOther = other.X + other.Width;
        float const maxX = (rightSelf > rightOther) ? rightSelf : rightOther;

        float const bottomSelf = self.Y + self.Height;
        float const bottomOther = other.Y + other.Height;

        float const maxY = (bottomSelf > bottomOther) ? bottomSelf : bottomOther;

        return RectF{minX, minY, maxX - minX, maxY - minY};
    }

    constexpr RectF Transpose(RectF self)
    {
        return {self.X, self.Y, self.Height, self.Width};
    }

    constexpr RectF CenterTranspose(RectF self)
    {
        return {
            self.X + ((self.Width - self.Height) / 2.0f),
            self.Y + ((self.Height - self.Width) / 2.0f),
            self.Height,
            self.Width,
        };
    }

    constexpr PointF Clip(RectF self, PointF point)
    {
        return {
            Clamp(point.X, self.X, self.X + self.Width),
            Clamp(point.Y, self.Y, self.Y + self.Height),
        };
    }
}
