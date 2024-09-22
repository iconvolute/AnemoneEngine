#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Reference.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Point.hxx"

#include <atomic>
#include <string_view>

namespace Anemone::App
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

    enum class WindowActivationState : uint8_t
    {
        Deactivated,
        Activated,
        PointerActivated,
    };

    class RUNTIME_API Window
    {
        friend class Reference<Window>;

    public:
        Window();
        Window(Window const&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window const&) = delete;
        Window& operator=(Window&&) = delete;
        virtual ~Window();

    private: // Reference<T>
        std::atomic<int32_t> m_ReferenceCount{0};

        void AcquireReference();
        void ReleaseReference();

    public:
        struct NativeHandle final
        {
            void* Inner;
        };

    public:
        virtual void Close() = 0;
        virtual void Minimize() = 0;
        virtual void Maximize() = 0;
        virtual void Restore() = 0;
        virtual void BringToFront(bool force) = 0;
        virtual void Focus() = 0;
        virtual void SetVisible(bool value) = 0;
        virtual void SetEnabled(bool value) = 0;
        virtual void SetInputEnabled(bool value) = 0;
        virtual bool GetInputEnabled() const = 0;
        virtual bool IsMinimized() const = 0;
        virtual bool IsMaximized() const = 0;
        virtual bool IsFocused() const = 0;
        virtual bool IsVisible() const = 0;
        virtual bool IsEnabled() const = 0;

        virtual WindowMode GetWindowMode() const = 0;
        virtual void SetWindowMode(WindowMode value) = 0;

        virtual void SetTitle(std::string_view const& value) = 0;

        virtual NativeHandle GetNativeHandle() const = 0;

        virtual Math::RectF GetPlacement() const = 0;
        virtual void SetPlacement(Math::RectF const& value) = 0;

        virtual std::optional<Math::SizeF> GetMinimumSize() const = 0;
        virtual void SetMinimumSize(std::optional<Math::SizeF> const& value) = 0;

        virtual std::optional<Math::SizeF> GetMaximumSize() const = 0;
        virtual void SetMaximumSize(std::optional<Math::SizeF> const& value) = 0;

        virtual Math::RectF GetBounds() const = 0;
        virtual Math::RectF GetClientBounds() const = 0;
    };
}
