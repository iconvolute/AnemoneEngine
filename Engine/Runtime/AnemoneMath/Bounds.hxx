#pragma once
#include <span>

namespace Anemone::Math
{
    struct PointF;
    struct SizeF;
    struct ThicknessF;
    struct RectF;

    struct BoundsF
    {
        float Left;
        float Top;
        float Right;
        float Bottom;

        [[nodiscard]] constexpr bool operator==(BoundsF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(BoundsF const& other) const = default;

        [[nodiscard]] constexpr static BoundsF FromLocationSize(float x, float y, float width, float height);
        [[nodiscard]] constexpr static BoundsF FromLocationSize(float x, float y, float size);
        [[nodiscard]] constexpr static BoundsF FromRectangle(RectF rect);
        [[nodiscard]] constexpr static BoundsF FromBounds(float left, float top, float right, float bottom);
        [[nodiscard]] constexpr static BoundsF FromPoints(PointF p1, PointF p2);
        [[nodiscard]] constexpr static BoundsF FromLocationSize(PointF location, SizeF size);
        [[nodiscard]] constexpr static BoundsF FromCenterSize(PointF location, SizeF size);
        [[nodiscard]] constexpr static BoundsF FromSize(SizeF size);
        [[nodiscard]] constexpr static BoundsF Empty();
        [[nodiscard]] constexpr static BoundsF NaN();

        [[nodiscard]] constexpr static BoundsF FromPoints(std::span<PointF const> points);

        [[nodiscard]] constexpr PointF TopLeft() const;
        [[nodiscard]] constexpr PointF TopRight() const;
        [[nodiscard]] constexpr PointF BottomLeft() const;
        [[nodiscard]] constexpr PointF BottomRight() const;

        [[nodiscard]] constexpr float Width() const;
        [[nodiscard]] constexpr float Height() const;

        [[nodiscard]] constexpr SizeF Size() const;
        [[nodiscard]] constexpr PointF Location() const;
        [[nodiscard]] constexpr PointF Center() const;
        [[nodiscard]] constexpr bool IsEmpty() const;
        [[nodiscard]] bool IsNaN() const;
    };

    [[nodiscard]] constexpr BoundsF Inflate(BoundsF self, float value);
    [[nodiscard]] constexpr BoundsF Inflate(BoundsF self, float horizontal, float vertical);
    [[nodiscard]] constexpr BoundsF Inflate(BoundsF self, float left, float top, float right, float bottom);
    [[nodiscard]] constexpr BoundsF Inflate(BoundsF self, ThicknessF border);
    [[nodiscard]] constexpr BoundsF Inflate(BoundsF self, SizeF size);

    [[nodiscard]] constexpr BoundsF Deflate(BoundsF self, float value);
    [[nodiscard]] constexpr BoundsF Deflate(BoundsF self, float horizontal, float vertical);
    [[nodiscard]] constexpr BoundsF Deflate(BoundsF self, float left, float top, float right, float bottom);
    [[nodiscard]] constexpr BoundsF Deflate(BoundsF self, ThicknessF border);
    [[nodiscard]] constexpr BoundsF Deflate(BoundsF self, SizeF size);

    [[nodiscard]] constexpr BoundsF Translate(BoundsF self, float x, float y);

    [[nodiscard]] constexpr bool Contains(BoundsF self, float x, float y);
    [[nodiscard]] constexpr bool Contains(BoundsF self, PointF point);
    [[nodiscard]] constexpr bool Contains(BoundsF self, BoundsF other);

    [[nodiscard]] constexpr BoundsF Intersect(BoundsF self, BoundsF other);
    [[nodiscard]] constexpr bool IntersectsWith(BoundsF self, BoundsF other);
    [[nodiscard]] constexpr BoundsF Union(BoundsF self, BoundsF other);
    [[nodiscard]] constexpr BoundsF Union(BoundsF self, PointF other);
    [[nodiscard]] constexpr BoundsF Transpose(BoundsF self);
    [[nodiscard]] constexpr BoundsF CenterTranspose(BoundsF self);
    [[nodiscard]] constexpr PointF Clip(BoundsF self, PointF point);
}

#include "AnemoneMath/Bounds.inl"
