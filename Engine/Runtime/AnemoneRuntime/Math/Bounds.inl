#pragma once
#include <algorithm>

#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Thickness.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Functions.hxx"

namespace Anemone::Math
{
    constexpr BoundsF BoundsF::FromLocationSize(float x, float y, float width, float height)
    {
        return {x, y, x + width, y + height};
    }

    constexpr BoundsF BoundsF::FromLocationSize(float x, float y, float size)
    {
        return {x, y, x + size, y + size};
    }

    constexpr BoundsF BoundsF::FromRectangle(RectF rect)
    {
        return {rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height};
    }

    constexpr BoundsF BoundsF::FromBounds(float left, float top, float right, float bottom)
    {
        return {left, top, right, bottom};
    }

    constexpr BoundsF BoundsF::FromPoints(PointF p1, PointF p2)
    {
        return {p1.X, p1.Y, p2.X, p2.Y};
    }

    constexpr BoundsF BoundsF::FromLocationSize(PointF location, SizeF size)
    {
        return {location.X, location.Y, location.X + size.Width, location.Y + size.Height};
    }

    constexpr BoundsF BoundsF::FromCenterSize(PointF location, SizeF size)
    {
        float const halfWidth = size.Width * 0.5f;
        float const halfHeight = size.Height * 0.5f;

        return {location.X - halfWidth, location.Y - halfHeight, location.X + halfWidth, location.Y + halfHeight};
    }

    constexpr BoundsF BoundsF::FromSize(SizeF size)
    {
        return {0.0f, 0.0f, size.Width, size.Height};
    }

    constexpr BoundsF BoundsF::Empty()
    {
        return {};
    }

    constexpr BoundsF BoundsF::NaN()
    {
        float const nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan, nan, nan};
    }

    constexpr BoundsF BoundsF::FromPoints(std::span<PointF const> points)
    {
        if (points.empty())
        {
            return BoundsF::NaN();
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

        return {minX, minY, maxX, maxY};
    }

    constexpr PointF BoundsF::TopLeft() const
    {
        return {this->Left, this->Top};
    }

    constexpr PointF BoundsF::TopRight() const
    {
        return {this->Right, this->Top};
    }

    constexpr PointF BoundsF::BottomLeft() const
    {
        return {this->Left, this->Bottom};
    }

    constexpr PointF BoundsF::BottomRight() const
    {
        return {this->Right, this->Bottom};
    }

    constexpr float BoundsF::Width() const
    {
        return this->Right - this->Left;
    }

    constexpr float BoundsF::Height() const
    {
        return this->Bottom - this->Top;
    }

    constexpr SizeF BoundsF::Size() const
    {
        return {this->Right - this->Left, this->Bottom - this->Top};
    }

    constexpr PointF BoundsF::Location() const
    {
        return {this->Left, this->Top};
    }

    constexpr PointF BoundsF::Center() const
    {
        float const halfWidth = (this->Right - this->Left) * 0.5f;
        float const halfHeight = (this->Bottom - this->Top) * 0.5f;
        return {this->Left + halfWidth, this->Top + halfHeight};
    }

    constexpr bool BoundsF::IsEmpty() const
    {
        return (this->Right <= this->Left) or (this->Bottom <= this->Top);
    }

    inline bool BoundsF::IsNaN() const
    {
        return std::isnan(this->Left) || std::isnan(this->Top) || std::isnan(this->Right) || std::isnan(this->Bottom);
    }

    constexpr BoundsF Inflate(BoundsF self, float value)
    {
        return {self.Left - value, self.Top - value, self.Right + value, self.Bottom + value};
    }

    constexpr BoundsF Inflate(BoundsF self, float horizontal, float vertical)
    {
        return {self.Left - horizontal, self.Top - vertical, self.Right + horizontal, self.Bottom + vertical};
    }

    constexpr BoundsF Inflate(BoundsF self, float left, float top, float right, float bottom)
    {
        return {self.Left - left, self.Top - top, self.Right + right, self.Bottom + bottom};
    }

    constexpr BoundsF Inflate(BoundsF self, ThicknessF border)
    {
        return {self.Left - border.Left, self.Top - border.Top, self.Right + border.Right, self.Bottom + border.Bottom};
    }

    constexpr BoundsF Inflate(BoundsF self, SizeF size)
    {
        return {self.Left - size.Width, self.Top - size.Height, self.Right + size.Width, self.Bottom + size.Height};
    }

    constexpr BoundsF Deflate(BoundsF self, float value)
    {
        return {self.Left + value, self.Top + value, self.Right - value, self.Bottom - value};
    }

    constexpr BoundsF Deflate(BoundsF self, float horizontal, float vertical)
    {
        return {self.Left + horizontal, self.Top + vertical, self.Right - horizontal, self.Bottom - vertical};
    }

    constexpr BoundsF Deflate(BoundsF self, float left, float top, float right, float bottom)
    {
        return {self.Left + left, self.Top + top, self.Right - right, self.Bottom - bottom};
    }

    constexpr BoundsF Deflate(BoundsF self, ThicknessF border)
    {
        return {self.Left + border.Left, self.Top + border.Top, self.Right - border.Right, self.Bottom - border.Bottom};
    }

    constexpr BoundsF Deflate(BoundsF self, SizeF size)
    {
        return {self.Left + size.Width, self.Top + size.Height, self.Right - size.Width, self.Bottom - size.Height};
    }

    constexpr BoundsF Translate(BoundsF self, float x, float y)
    {
        return {self.Left + x, self.Top + y, self.Right + x, self.Bottom + y};
    }

    constexpr bool Contains(BoundsF self, float x, float y)
    {
        return (self.Left <= x) and (x <= self.Right) and (self.Top <= y) and (y <= self.Bottom);
    }

    constexpr bool Contains(BoundsF self, PointF point)
    {
        return Contains(self, point.X, point.Y);
    }

    constexpr bool Contains(BoundsF self, BoundsF other)
    {
        return (self.Left <= other.Left) && (other.Right <= self.Right) && (self.Top <= other.Top) && (other.Bottom <= self.Bottom);
    }

    constexpr BoundsF Intersect(BoundsF self, BoundsF other)
    {
        float const left = (self.Left > other.Left) ? self.Left : other.Left;
        float const top = (self.Top > other.Top) ? self.Top : other.Top;
        float const right = (self.Right < other.Right) ? self.Right : other.Right;
        float const bottom = (self.Bottom < other.Bottom) ? self.Bottom : other.Bottom;

        BoundsF result;

        if ((right >= left) and (bottom >= top))
        {
            result.Left = left;
            result.Top = top;
            result.Right = right;
            result.Bottom = bottom;
        }
        else
        {
            result.Left = 0.0f;
            result.Top = 0.0f;
            result.Right = 0.0f;
            result.Bottom = 0.0f;
        }

        return result;
    }

    constexpr bool IntersectsWith(BoundsF self, BoundsF other)
    {
        return (other.Left < self.Right) and (self.Left < other.Right) and (other.Top < self.Bottom) and (self.Top < other.Bottom);
    }

    constexpr BoundsF Union(BoundsF self, BoundsF other)
    {
        float const left = (self.Left < other.Left) ? self.Left : other.Left;
        float const top = (self.Top < other.Top) ? self.Top : other.Top;
        float const right = (self.Right > other.Right) ? self.Right : other.Right;
        float const bottom = (self.Bottom > other.Bottom) ? self.Bottom : other.Bottom;

        return {left, top, right, bottom};
    }

    constexpr BoundsF Union(BoundsF self, PointF other)
    {
        float const left = (self.Left < other.X) ? self.Left : other.X;
        float const top = (self.Top < other.Y) ? self.Top : other.Y;
        float const right = (self.Right > other.X) ? self.Right : other.X;
        float const bottom = (self.Bottom > other.Y) ? self.Bottom : other.Y;

        return {left, top, right, bottom};
    }

    constexpr BoundsF Transpose(BoundsF self)
    {
        return {
            self.Left,
            self.Top,
            self.Left + (self.Bottom - self.Top),
            self.Top + (self.Right - self.Left),
        };
    }

    constexpr BoundsF CenterTranspose(BoundsF self)
    {
        // Size of the bounds.
        float const width = self.Right - self.Left;
        float const height = self.Bottom - self.Top;

        // Half of the size.
        float const halfWidth = width * 0.5f;
        float const halfHeight = height * 0.5f;

        // Compute the center position.
        float const centerX = self.Left + halfWidth;
        float const centerY = self.Top + halfHeight;

        // Return the new bounds with transposed width and height.
        return {
            centerX - halfHeight,
            centerY - halfWidth,
            centerX + halfHeight,
            centerY + halfWidth,
        };
    }

    constexpr PointF Clip(BoundsF self, PointF point)
    {
        return {
            Clamp(point.X, self.Left, self.Right),
            Clamp(point.Y, self.Top, self.Bottom),
        };
    }
}
