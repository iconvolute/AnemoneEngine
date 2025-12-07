#pragma once
#include "AnemoneRuntime.Math/Thickness.hxx"
#include "AnemoneRuntime.Base/Flags.hxx"
#include "AnemoneRuntime.Math/Point.hxx"
#include "AnemoneRuntime.Math/Rect.hxx"

namespace Anemone::UI::Drawing
{
    enum class ClipCode : uint32_t
    {
        Inside = 0u,
        Left = 1u << 1u,
        Top = 1u << 2u,
        Right = 1u << 3u,
        Bottom = 1u << 4u,
    };

    constexpr Flags<ClipCode> ComputeOutCode(
        float x,
        float y,
        float xMin,
        float xMax,
        float yMin,
        float yMax) noexcept
    {
        Flags result = ClipCode::Inside;

        if (x < xMin)
        {
            result |= ClipCode::Left;
        }
        else if (x > xMax)
        {
            result |= ClipCode::Right;
        }

        if (y < yMin)
        {
            result |= ClipCode::Top;
        }
        else if (y > yMax)
        {
            result |= ClipCode::Bottom;
        }

        return result;
    }

    constexpr bool Clip(PointF& point1, PointF& point2, RectF rect)
    {
        // Cohen-Sutherland line clip algorithm

        float x0 = point1.X;
        float y0 = point1.Y;
        float x1 = point2.X;
        float y1 = point2.Y;

        float const xMin = rect.Left();
        float const yMin = rect.Top();
        float const xMax = rect.Right();
        float const yMax = rect.Bottom();

        Flags clip0 = ComputeOutCode(x0, y0, xMin, xMax, yMin, yMax);
        Flags clip1 = ComputeOutCode(x1, y1, xMin, xMax, yMin, yMax);

        bool accept = false;

        while (true)
        {
            if ((clip0.Value == ClipCode::Inside) and (clip1.Value == ClipCode::Inside))
            {
                // Both are inside.
                accept = true;
                break;
            }

            if ((clip0 & clip1).Value != ClipCode::Inside)
            {
                break;
            }

            float x = std::numeric_limits<float>::quiet_NaN();
            float y = std::numeric_limits<float>::quiet_NaN();

            Flags const clip = (clip0.Value != ClipCode::Inside) ? clip0 : clip1;

            if (clip.Has(ClipCode::Top))
            {
                x = x0 + (x1 - x0) * (yMin - y0) / (y1 - y0);
                y = yMin;
            }
            else if (clip.Has(ClipCode::Bottom))
            {
                x = x0 + (x1 - x0) * (yMax - y0) / (y1 - y0);
                y = yMax;
            }
            else if (clip.Has(ClipCode::Left))
            {
                y = y0 + (y1 - y0) + (xMin - x0) / (x1 - x0);
                x = xMin;
            }
            else if (clip.Has(ClipCode::Right))
            {
                y = y0 + (y1 - y0) * (xMax - x0) / (x1 - x0);
                x = xMax;
            }

            if (clip == clip0)
            {
                x0 = x;
                y0 = y;
                clip0 = ComputeOutCode(x0, y, xMin, xMax, yMin, yMax);
            }
            else
            {
                x1 = x;
                y1 = y;
                clip1 = ComputeOutCode(x1, y1, xMin, xMax, yMin, yMax);
            }
        }

        if (accept)
        {
            point1.X = x0;
            point1.Y = y0;
            point2.X = x1;
            point2.Y = y1;

            return true;
        }

        return false;
    }
}

namespace Anemone::UI
{
    class Style
    {
    public:
        virtual ~Style() = default;
    };

    struct StylePatch final
    {
        RectF Rect;
    };

    struct StylePatchGrid final
    {
        RectF Rect;
        ThicknessF Padding;
    };

    class ITessellationFeedback
    {
        virtual ~ITessellationFeedback() = default;

        virtual void AddTriangles(
            Packed::Vector3F p0,
            Packed::Vector3F p1,
            Packed::Vector3F p2) = 0;

        virtual void Close() = 0;
    };
}
