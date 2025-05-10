#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Thickness.hxx"
#include "AnemoneRuntime/Base/ErrorCode.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"

#include <expected>

namespace Anemone
{
    enum class CursorType : uint8_t
    {
        None,
        Arrow,
        ArrowWait,
        Text,
        SizeHorizontal,
        SizeVertical,
        SizeLeft,
        SizeTop,
        SizeRight,
        SizeBottom,
        SizeTopLeft,
        SizeTopRight,
        SizeBottomLeft,
        SizeBottomRight,
        SizeAll,
        Cross,
    };

    enum class WindowType : uint8_t
    {
        Game,
        Viewport,
        Form,
        Dialog,
    };

    enum class WindowMode : uint8_t
    {
        Windowed,
        Fullscreen,
        Borderless,
    };

    class RUNTIME_API Window
    {
    public:
        Window() = default;
        Window(Window const&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window const&) = delete;
        Window& operator=(Window&&) = delete;
        virtual ~Window() = default;

    public:
        virtual void Close() = 0;
        virtual bool IsClosed() = 0;

        virtual void Minimize() = 0;
        virtual bool IsMinimized() = 0;

        virtual void Maximize() = 0;
        virtual bool IsMaximized() = 0;

        virtual void Restore() = 0;
        virtual void BringToFront(bool force) = 0;

        virtual void Focus() = 0;
        virtual bool IsFocused() = 0;

        virtual void SetVisible(bool value) = 0;
        virtual bool IsVisible() = 0;

        virtual void SetEnabled(bool value) = 0;
        virtual bool IsEnabled() = 0;

        virtual void SetInputEnabled(bool value) = 0;
        virtual bool GetInputEnabled() = 0;

        virtual WindowMode GetMode() = 0;
        virtual void SetMode(WindowMode value) = 0;

        virtual void SetTitle(std::string_view value) = 0;

        virtual std::optional<Math::SizeF> GetMinimumSize() = 0;
        virtual void SetMinimumSize(std::optional<Math::SizeF> value) = 0;

        virtual std::optional<Math::SizeF> GetMaximumSize() = 0;
        virtual void SetMaximumSize(std::optional<Math::SizeF> value) = 0;

        virtual void SetCursor(CursorType value) = 0;
        virtual CursorType GetCursor() = 0;

        virtual Math::RectF GetBounds() = 0;
        virtual void SetBounds(Math::RectF value) = 0;

        virtual Math::RectF GetClientBounds() = 0;
    };
}
