#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"

#include <memory>

namespace Anemone::Platform
{
    enum class CursorType : uint8_t
    {
        None,
        Arrow,
        IBeam,
        Wait,
        Cross,
        SizeNWSE,
        SizeNESW,
        SizeWE,
        SizeNS,
        SizeAll,
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
        : public IntrusiveListNode<Window>
    {
        friend struct IntrusiveList<Window>;

    public:
        Window();
        Window(Window const&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window const&) = delete;
        Window& operator=(Window&&) = delete;
        virtual ~Window();

    public:
        static std::unique_ptr<Window> Create(WindowType type);

    public:
        virtual void Close() = 0;

        virtual void Minimize() = 0;
        virtual bool IsMinimized() const = 0;

        virtual void Maximize() = 0;
        virtual bool IsMaximized() const = 0;

        virtual void Restore() = 0;
        virtual void BringToFront(bool force) = 0;

        virtual void Focus() = 0;
        virtual bool IsFocused() const = 0;

        virtual void SetVisible(bool value) = 0;
        virtual bool IsVisible() const = 0;

        virtual void SetEnabled(bool value) = 0;
        virtual bool IsEnabled() const = 0;

        virtual void SetInputEnabled(bool value) = 0;
        virtual bool GetInputEnabled() const = 0;

        virtual WindowMode GetMode() const = 0;
        virtual void SetMode(WindowMode value) = 0;

        virtual void SetTitle(std::string_view value) = 0;

        virtual Math::RectF GetPlacement() const = 0;
        virtual void SetPlacement(Math::RectF value) = 0;

        virtual std::optional<Math::SizeF> GetMinimumSize() const = 0;
        virtual void SetMinimumSize(std::optional<Math::SizeF> value) = 0;

        virtual std::optional<Math::SizeF> GetMaximumSize() const = 0;
        virtual void SetMaximumSize(std::optional<Math::SizeF> value) = 0;

        virtual void SetCursor(CursorType value) = 0;
        virtual CursorType GetCursor() const = 0;

        virtual Math::RectF GetBounds() const = 0;
        virtual Math::RectF GetClientBounds() const = 0;
    };
}
