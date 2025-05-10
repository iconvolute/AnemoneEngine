#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"

namespace Anemone::Interop::Windows
{
    anemone_forceinline HCURSOR LoadSystemCursor(LPCWSTR id)
    {
        return static_cast<HCURSOR>(LoadImageW(nullptr, id, IMAGE_CURSOR, 0, 0, LR_SHARED));
    }

    inline constexpr UINT GetKeyState_None = 0u;
    inline constexpr UINT GetKeyState_Down = 0x8000u;
    inline constexpr UINT GetKeyState_Locked = 0x0001u;

    struct WindowStyle final
    {
        DWORD Style;
        DWORD ExStyle;
    };

    anemone_forceinline bool IsKeyPressed(UINT key)
    {
        return (GetKeyState(key) & GetKeyState_Down) != 0;
    }

    anemone_forceinline bool IsKeyLocked(UINT key)
    {
        return (GetKeyState(key) & GetKeyState_Locked) != 0;
    }

    anemone_forceinline bool IsKeyPressedAsync(UINT key)
    {
        return (GetAsyncKeyState(key) & GetKeyState_Down) != 0;
    }

    anemone_forceinline bool IsKeyLockedAsync(UINT key)
    {
        return (GetAsyncKeyState(key) & GetKeyState_Locked) != 0;
    }

    anemone_forceinline void RestoreUI() noexcept
    {
        while (ShowCursor(TRUE) < 0)
        {
            // do nothing
        }

        ReleaseCapture();

        ClipCursor(nullptr);
    }

    struct WindowMessageResult final
    {
        bool Handled;
        LRESULT Result;

        static WindowMessageResult WithResult(LRESULT result)
        {
            return WindowMessageResult{
                .Handled = true,
                .Result = result,
            };
        }

        static WindowMessageResult Default()
        {
            return {
                .Handled = false,
                .Result = 0,
            };
        }
    };

    inline float GetWindowDpiScale(HWND hwnd)
    {
        UINT const dpi = ::GetDpiForWindow(hwnd);
        float const scale = static_cast<float>(dpi) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
        return scale;
    }

    constexpr Math::PointF ToPointF(POINT const& value)
    {
        return {
            .X = static_cast<float>(value.x),
            .Y = static_cast<float>(value.y),
        };
    }
    
    constexpr POINT ToPOINT(Math::PointF const& value)
    {
        return {
            .x = static_cast<LONG>(value.X),
            .y = static_cast<LONG>(value.Y),
        };
    }

    constexpr Math::PointF ToPointF(RECT const& value)
    {
        return {
            .X = static_cast<float>(value.left),
            .Y = static_cast<float>(value.top),
        };
    }

    constexpr POINT ToPOINT(Math::RectF const& value)
    {
        return {
            .x = static_cast<LONG>(value.X),
            .y = static_cast<LONG>(value.Y),
        };
    }

    constexpr Math::SizeF ToSizeF(SIZE const& value)
    {
        return {
            .Width = static_cast<float>(value.cx),
            .Height = static_cast<float>(value.cy),
        };
    }

    constexpr SIZE ToSIZE(Math::SizeF const& value)
    {
        return {
            .cx = static_cast<LONG>(value.Width),
            .cy = static_cast<LONG>(value.Height),
        };
    }

    constexpr Math::SizeF ToSizeF(RECT const& value)
    {
        return {
            .Width = static_cast<float>(value.right - value.left),
            .Height = static_cast<float>(value.bottom - value.top),
        };
    }

    constexpr SIZE ToSIZE(Math::RectF const& value)
    {
        return {
            .cx = static_cast<LONG>(value.Width),
            .cy = static_cast<LONG>(value.Height),
        };
    }

    constexpr Math::RectF ToRectF(RECT const& value)
    {
        return {
            .X = static_cast<float>(value.left),
            .Y = static_cast<float>(value.top),
            .Width = static_cast<float>(value.right - value.left),
            .Height = static_cast<float>(value.bottom - value.top),
        };
    }

    constexpr RECT ToRECT(Math::RectF const& value)
    {
        return {
            .left = static_cast<LONG>(value.X),
            .top = static_cast<LONG>(value.Y),
            .right = static_cast<LONG>(value.X + value.Width),
            .bottom = static_cast<LONG>(value.Y + value.Height),
        };
    }

    constexpr Math::RectF ToRectF(POINT const& value)
    {
        return {
            .X = static_cast<float>(value.x),
            .Y = static_cast<float>(value.y),
            .Width = 0.0f,
            .Height = 0.0f,
        };
    }

    constexpr RECT ToRECT(Math::PointF const& value)
    {
        return {
            .left = static_cast<LONG>(value.X),
            .top = static_cast<LONG>(value.Y),
            .right = static_cast<LONG>(value.X),
            .bottom = static_cast<LONG>(value.Y),
        };
    }
    
    constexpr Math::RectF ToRectF(SIZE const& value)
    {
        return {
            .X = 0.0f,
            .Y = 0.0f,
            .Width = static_cast<float>(value.cx),
            .Height = static_cast<float>(value.cy),
        };
    }

    constexpr RECT ToRECT(Math::SizeF const& value)
    {
        return {
            .left = 0,
            .top = 0,
            .right = static_cast<LONG>(value.Width),
            .bottom = static_cast<LONG>(value.Height),
        };
    }

    constexpr Math::RectF ToRectF(POINT const& location, SIZE const& size)
    {
        return {
            .X = static_cast<float>(location.x),
            .Y = static_cast<float>(location.y),
            .Width = static_cast<float>(size.cx),
            .Height = static_cast<float>(size.cy),
        };
    }

    constexpr RECT ToRECT(Math::PointF const& location, Math::SizeF const& size)
    {
        return {
            .left = static_cast<LONG>(location.X),
            .top = static_cast<LONG>(location.Y),
            .right = static_cast<LONG>(location.X + size.Width),
            .bottom = static_cast<LONG>(location.Y + size.Height),
        };
    }
}
