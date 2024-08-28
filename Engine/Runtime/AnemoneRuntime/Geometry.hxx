#pragma once
#include <algorithm>
#include <span>
#include <cmath>

namespace Anemone
{
    template <typename T>
    struct Point;

    template <typename T>
    struct Size;

    template <typename T>
    struct Rectangle;

    template <typename T>
    struct Margins;
}

namespace Anemone
{
    template <typename T>
    struct Point
    {
        using ValueType = T;

        ValueType X;
        ValueType Y;

        [[nodiscard]] friend constexpr bool operator==(Point const& self, Point const& other)
        {
            return (self.X == other.X) && (self.Y == other.Y);
        }

        [[nodiscard]] friend constexpr bool operator!=(Point const& self, Point const& other)
        {
            return !(self == other);
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct Size
    {
        using ValueType = T;

        ValueType Width;
        ValueType Height;

        [[nodiscard]] constexpr bool IsEmpty() const
        {
            return (this->Width <= ValueType{0}) || (this->Height <= ValueType{0});
        }

        [[nodiscard]] constexpr bool IsZero() const
        {
            return (this->Width == ValueType{0}) && (this->Height == ValueType{0});
        }

        [[nodiscard]] friend constexpr bool operator==(Size const& self, Size const& other)
        {
            return (self.Width == other.Width) && (self.Height == other.Height);
        }

        [[nodiscard]] friend constexpr bool operator!=(Size const& self, Size const& other)
        {
            return !(self == other);
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct Rectangle
    {
        using ValueType = T;

        ValueType X;
        ValueType Y;
        ValueType Width;
        ValueType Height;

        [[nodiscard]] static constexpr Rectangle FromCenterSize(
            Point<ValueType> const& location,
            Size<ValueType> const& size)
        {
            return Rectangle{
                .X = location.X - (size.Width / ValueType{2}),
                .Y = location.Y - (size.Height / ValueType{2}),
                .Width = size.Width,
                .Height = size.Height,
            };
        }

        [[nodiscard]] static constexpr Rectangle FromLocationSize(
            Point<ValueType> const& location,
            Size<ValueType> const& size)
        {
            return Rectangle{
                .X = location.X,
                .Y = location.Y,
                .Width = size.Width,
                .Height = size.Height,
            };
        }

        [[nodiscard]] static constexpr Rectangle FromBounds(
            ValueType left,
            ValueType top,
            ValueType right,
            ValueType bottom)
        {
            return Rectangle{
                .X = left,
                .Y = top,
                .Width = (right - left),
                .Height = (bottom - top),
            };
        }

        [[nodiscard]] static constexpr Rectangle FromPoints(
            Point<ValueType> const& p1,
            Point<ValueType> const& p2)
        {
            ValueType x;
            ValueType y;
            ValueType w;
            ValueType h;

            if (p1.x < p2.x)
            {
                x = p1.x;
                w = p2.x - p1.x;
            }
            else
            {
                x = p2.x;
                w = p1.x - p2.x;
            }

            if (p1.y < p2.y)
            {
                y = p1.y;
                h = p2.y - p1.y;
            }
            else
            {
                y = p2.y;
                h = p1.y - p2.y;
            }

            return {
                .X = x,
                .Y = y,
                .Width = w,
                .Height = h,
            };
        }

        [[nodiscard]] static constexpr Rectangle FromPoints(
            std::span<Point<ValueType> const> points)
        {
            ValueType minX{};
            ValueType minY{};
            ValueType maxX{};
            ValueType maxY{};

            for (Point<ValueType> const& point : points)
            {
                minX = std::min(minX, point.X);
                minY = std::min(minY, point.Y);
                maxX = std::max(maxX, point.X);
                maxY = std::max(maxY, point.Y);
            }

            return Rectangle::FromBounds(
                minX,
                minY,
                maxX,
                maxY);
        }

        [[nodiscard]] constexpr Point<ValueType> GetLocation() const
        {
            return {
                .X = this->X,
                .Y = this->Y,
            };
        }

        [[nodiscard]] constexpr Size<ValueType> GetSize() const
        {
            return {
                .Width = this->Width,
                .Height = this->Height,
            };
        }

        [[nodiscard]] constexpr Point<ValueType> GetCenter() const
        {
            return {
                .X = this->X + (this->Width / ValueType{2}),
                .Y = this->Y + (this->Height / ValueType{2}),
            };
        }

        [[nodiscard]] constexpr ValueType GetLeft() const
        {
            return this->X;
        }

        [[nodiscard]] constexpr ValueType GetTop() const
        {
            return this->Y;
        }

        [[nodiscard]] constexpr ValueType GetRight() const
        {
            return this->X + this->Width;
        }

        [[nodiscard]] constexpr ValueType GetBottom() const
        {
            return this->Y + this->Height;
        }

        [[nodiscard]] constexpr Point<ValueType> GetTopLeft() const
        {
            return {
                .X = this->X,
                .Y = this->Y,
            };
        }

        [[nodiscard]] constexpr Point<ValueType> GetTopRight() const
        {
            return {
                .X = this->X + this->Width,
                .Y = this->Y,
            };
        }

        [[nodiscard]] constexpr Point<ValueType> GetBottomLeft() const
        {
            return {
                .X = this->X,
                .Y = this->Y + this->Height,
            };
        }

        [[nodiscard]] constexpr Point<ValueType> GetBottomRight() const
        {
            return {
                .X = this->X + this->Width,
                .Y = this->Y + this->Height,
            };
        }

        [[nodiscard]] constexpr bool IsEmppty() const
        {
            return (this->Width <= ValueType{0}) || (this->Height <= ValueType{0});
        }

        [[nodiscard]] constexpr bool IsZero() const
        {
            return (this->Width == ValueType{0}) && (this->Height == ValueType{0});
        }

        [[nodiscard]] friend constexpr bool operator==(Rectangle const& self, Rectangle const& other)
        {
            return (self.X == other.X) && (self.Y == other.Y) && (self.Width == other.Width) && (self.Height == other.Height);
        }

        [[nodiscard]] friend constexpr bool operator!=(Rectangle const& self, Rectangle const& other)
        {
            return !(self == other);
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct Margins
    {
        using ValueType = T;

        ValueType Left;
        ValueType Top;
        ValueType Right;
        ValueType Bottom;

        [[nodiscard]] constexpr ValueType GetHorizontal() const
        {
            return this->Left + this->Right;
        }

        [[nodiscard]] constexpr ValueType GetVertical() const
        {
            return this->Top + this->Bottom;
        }

        [[nodiscard]] friend constexpr bool operator==(Margins const& self, Margins const& other)
        {
            return (self.Left == other.Left) && (self.Top == other.Top) && (self.Right == other.Right) && (self.Bottom == other.Bottom);
        }
    };
}

namespace Anemone
{
    template <typename T>
    [[nodiscard]] constexpr Size<T> Transposed(Size<T> const& self)
    {
        return {
            .Width = self.Height,
            .Height = self.Width,
        };
    }

    template <std::integral T, std::floating_point U>
    [[nodiscard]] constexpr Rectangle<T> Truncate(Rectangle<U> const& self)
    {
        return {
            .X = static_cast<T>(self.X),
            .Y = static_cast<T>(self.Y),
            .Width = static_cast<T>(self.Width),
            .Height = static_cast<T>(self.Height),
        };
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr Rectangle<T> Round(Rectangle<T> const& self)
    {
        return {
            .X = std::round(self.X),
            .Y = std::round(self.Y),
            .Width = std::round(self.Width),
            .Height = std::round(self.Height),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> United(Rectangle<T> const& self, Rectangle<T> const& other)
    {
        T const x = std::min(self.X, other.X);
        T const y = std::min(self.Y, other.Y);
        T const w = std::max(self.GetRight(), other.GetRight()) - x;
        T const h = std::max(self.GetBottom(), other.GetBottom()) - y;

        return {
            .X = x,
            .Y = y,
            .Width = w,
            .Height = h,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> United(Rectangle<T> const& self, Point<T> const& other)
    {
        T const x = std::min(self.X, other.X);
        T const y = std::min(self.Y, other.Y);
        T const w = std::max(self.GetRight(), other.X) - x;
        T const h = std::max(self.GetBottom(), other.Y) - y;

        return {
            .X = x,
            .Y = y,
            .Width = w,
            .Height = h,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Intersected(Rectangle<T> const& self, Rectangle<T> const& other)
    {
        T const x = std::max(self.X, other.X);
        T const y = std::max(self.Y, other.Y);
        T const w = std::min(self.GetRight(), other.GetRight()) - x;
        T const h = std::min(self.GetBottom(), other.GetBottom()) - y;

        return {
            .X = x,
            .Y = y,
            .Width = w,
            .Height = h,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Inflated(Rectangle<T> const& self, T horizontal, T vertical)
    {
        return {
            .X = self.X - horizontal,
            .Y = self.Y - vertical,
            .Width = self.Width + (horizontal * T{2}),
            .Height = self.height + (vertical * T{2}),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Inflated(Rectangle<T> const& self, T value)
    {
        T const value2 = value * T{2};

        return {
            .X = self.X - value,
            .Y = self.Y - value,
            .Width = self.Width + value2,
            .Height = self.Height + value2,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Inflated(Rectangle<T> const& self, Size<T> const& size)
    {
        return Inflated<T>(self, size.Width, size.Height);
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Inflated(Rectangle<T> const& self, Margins<T> const& margins)
    {
        return {
            .X = self.X - margins.Left,
            .Y = self.Y - margins.Top,
            .Width = self.Width + (margins.Left + margins.Right),
            .Height = self.Height + (margins.Top + margins.Bottom),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Inflated(Rectangle<T> const& self, T left, T top, T right, T bottom)
    {
        return {
            .X = self.X - left,
            .Y = self.Y - top,
            .Width = self.Width + (left + right),
            .Height = self.Height + (top + bottom),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Deflated(Rectangle<T> const& self, T horizontal, T vertical)
    {
        return {
            .X = self.X + horizontal,
            .Y = self.Y + vertical,
            .Width = self.Width - (horizontal * T{2}),
            .Height = self.Height - (vertical * T{2}),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Deflated(Rectangle<T> const& self, T value)
    {
        T const value2 = value * T{2};

        return {
            .X = self.X + value,
            .Y = self.Y + value,
            .Width = self.Width - value2,
            .Height = self.Height - value2,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Deflated(Rectangle<T> const& self, Size<T> const& size)
    {
        return Deflated<T>(self, size.Width, size.Height);
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Deflated(Rectangle<T> const& self, Margins<T> const& margins)
    {
        return {
            .X = self.X + margins.Left,
            .Y = self.Y + margins.Top,
            .Width = self.Width - (margins.Left + margins.Right),
            .Height = self.Height - (margins.Top + margins.Bottom),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Deflated(Rectangle<T> const& self, T left, T top, T right, T bottom)
    {
        return {
            .X = self.X + left,
            .Y = self.Y + top,
            .Width = self.Width - (left + right),
            .Height = self.Height - (top + bottom),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Translated(Rectangle<T> const& self, T x, T y)
    {
        return {
            .X = self.X + x,
            .Y = self.Y + y,
            .Width = self.Width,
            .Height = self.Height,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Point<T> Clamp(Rectangle<T> const& self, Point<T> const& point)
    {
        return {
            .X = std::clamp(point.X, self.X, self.X + self.Width),
            .Y = std::clamp(point.Y, self.Y, self.Y + self.Height),
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Transposed(Rectangle<T> const& self)
    {
        return {
            .X = self.X,
            .Y = self.Y,
            .Width = self.Height,
            .Height = self.Width,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> CenterTransposed(Rectangle<T> const& self)
    {
        T const x = self.X + (self.Width / T{2});
        T const y = self.Y + (self.Height / T{2});
        T const w = self.Height;
        T const h = self.Width;

        return {
            .X = x - (w / T{2}),
            .Y = y - (h / T{2}),
            .Width = w,
            .Height = h,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr Rectangle<T> Centered(Rectangle<T> const& self, Point<T> const& location)
    {
        return {
            .X = location.X - (self.Width / T{2}),
            .Y = location.Y - (self.Height / T{2}),
            .Width = self.Width,
            .Height = self.Height,
        };
    }

    template <typename T>
    [[nodiscard]] constexpr bool Intersects(Rectangle<T> const& self, Rectangle<T> const& other)
    {
        return (other.X < (self.X + self.Width)) && ((other.X + other.Width) >= self.X) && (other.Y < (self.Y + self.Height)) && ((other.Y + other.Height) >= self.Y);
    }

    template <typename T>
    [[nodiscard]] constexpr bool Contains(Rectangle<T> const& self, Rectangle<T> const& other)
    {
        return (other.X >= self.X) && ((other.X + other.Width) <= (self.X + self.Width)) && (other.Y >= self.Y) && ((other.Y + other.Height) <= (self.Y + self.Height));
    }

    template <typename T>
    [[nodiscard]] constexpr bool Contains(Rectangle<T> const& self, T x, T y)
    {
        return (x >= self.X) && (y < (self.X + self.Width)) && (y >= self.Y) && (y < (self.Y + self.Height));
    }

    template <typename T>
    [[nodiscard]] constexpr bool Contains(Rectangle<T> const& self, Point<T> const& other)
    {
        return Contains<T>(self, other.X, other.Y);
    }

    template <typename T>
    [[nodiscard]] constexpr Margins<T> Scaled(Margins<T> const& self, T value)
    {
        return {
            .Left = self.Left * value,
            .Top = self.Top * value,
            .Right = self.Right * value,
            .Bottom = self.Bottom * value,
        };
    }
}
