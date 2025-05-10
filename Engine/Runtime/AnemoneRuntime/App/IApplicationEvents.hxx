#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Input/Types.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Point.hxx"

namespace Anemone
{
    class IWindow;
    class IApplication;
    class IApplicationEvents;

    struct MouseEventArgs
    {
        Math::PointF Position;
        VirtualKeyModifiers Modifiers;
    };

    struct MouseWheelEventArgs : MouseEventArgs
    {
        float Horizontal;
        float Vertical;
    };

    struct MouseButtonEventArgs : MouseEventArgs
    {
        VirtualKey Key;
    };

    struct MouseMoveEventArgs : MouseEventArgs
    {
        Math::PointF Delta;
        bool Absolute;
    };

    struct CharacterReceivedEventArgs
    {
        char32_t Character;
        bool Repeat;
    };

    struct KeyEventArgs
    {
        VirtualKey Key;
        VirtualKeyModifiers Modifiers;
        bool Repeat;
    };

    struct GamepadButtonEventArgs
    {
        GamepadButton Button;
    };

    struct GamepadAnalogEventArgs
    {
        GamepadAxis Axis;
        float Value;
    };

    struct WindowEventArgs
    {
    };

    struct WindowCloseEventArgs : WindowEventArgs
    {
        bool Cancel = false;
    };

    struct WindowActivatedEventArgs : WindowEventArgs
    {
        bool Activated;
    };

    struct WindowSizeChangedEventArgs : WindowEventArgs
    {
        Math::SizeF Size;
    };

    struct WindowLocationChangedEventArgs : WindowEventArgs
    {
        Math::PointF Location;
    };

    struct WindowDpiChangedEventArgs : WindowEventArgs
    {
        float Dpi;
        float Scale;
    };

    struct EndSessionEventArgs
    {
        bool LogOff;
        bool Shutdown;
        bool Force;
        bool CloseApplication;
    };

    class IApplicationEvents
    {
    public:
        IApplicationEvents() = default;
        IApplicationEvents(IApplicationEvents const&) = delete;
        IApplicationEvents(IApplicationEvents&&) = delete;
        IApplicationEvents& operator=(IApplicationEvents const&) = delete;
        IApplicationEvents& operator=(IApplicationEvents&&) = delete;
        virtual ~IApplicationEvents() = default;

        // Mouse Events
        virtual void OnMouseEnter(IWindow& window, MouseEventArgs& args) = 0;
        virtual void OnMouseLeave(IWindow& window, MouseEventArgs& args) = 0;
        virtual void OnMouseMove(IWindow& window, MouseMoveEventArgs& args) = 0;
        virtual void OnMouseWheel(IWindow& window, MouseWheelEventArgs& args) = 0;
        virtual void OnMouseButtonDown(IWindow& window, MouseButtonEventArgs& args) = 0;
        virtual void OnMouseButtonUp(IWindow& window, MouseButtonEventArgs& args) = 0;
        virtual void OnMouseButtonClick(IWindow& window, MouseButtonEventArgs& args) = 0;

        // Keyboard Events
        virtual void OnKeyDown(IWindow& window, KeyEventArgs& args) = 0;
        virtual void OnKeyUp(IWindow& window, KeyEventArgs& args) = 0;
        virtual void OnCharacterReceived(IWindow& window, CharacterReceivedEventArgs& args) = 0;

        // Input Events
        virtual void OnGamepadAnalog(InputDeviceId device, GamepadAnalogEventArgs& args) = 0;
        virtual void OnGamepadButtonDown(InputDeviceId device, GamepadButtonEventArgs& args) = 0;
        virtual void OnGamepadButtonUp(InputDeviceId device, GamepadButtonEventArgs& args) = 0;

        // Window Events
        virtual void OnWindowClose(IWindow& window, WindowCloseEventArgs& args) = 0;
        virtual void OnWindowActivated(IWindow& window, WindowActivatedEventArgs& args) = 0;
        virtual void OnWindowSizeChanged(IWindow& window, WindowSizeChangedEventArgs& args) = 0;
        virtual void OnWindowLocationChanged(IWindow& window, WindowLocationChangedEventArgs& args) = 0;
        virtual void OnWindowResizeStarted(IWindow& window, WindowEventArgs& args) = 0;
        virtual void OnWindowResizeCompleted(IWindow& window, WindowEventArgs& args) = 0;
        virtual void OnWindowDpiChanged(IWindow& window, WindowDpiChangedEventArgs& args) = 0;

        // virtual void OnWindowFocusChanged(Window& window, WindowEventArgs& args) = 0; ?
        // virtual void OnWindowStateChanged(Window& window, WindowEventArgs& args) = 0; ?

        // System Events
        virtual void OnEndSession(EndSessionEventArgs& args) = 0;
        virtual void OnSystemSuspending() = 0;
        virtual void OnSystemResuming() = 0;
        virtual void OnDisplayChange() = 0;
    };
}
