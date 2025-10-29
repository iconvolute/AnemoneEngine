#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneMath/Size.hxx"
#include "AnemoneMath/Rect.hxx"

#include <string_view>
#include <optional>

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

    class HostWindow
        : public ReferenceCounted<HostWindow>
    {
    public:
        HostWindow() = default;
        HostWindow(HostWindow const&) = delete;
        HostWindow(HostWindow&&) = delete;
        virtual ~HostWindow() = default;
        HostWindow& operator=(HostWindow const&) = delete;
        HostWindow& operator=(HostWindow&&) = delete;

    public:
        virtual void* GetNativeHandle() const = 0;

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

        virtual bool IsActive() = 0;

        virtual void Activate() = 0;

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

    using HostWindowRef = Reference<HostWindow>;
}
