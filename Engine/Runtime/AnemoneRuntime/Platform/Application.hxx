#pragma once
#include "AnemoneRuntime/Platform/Window.hxx"
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Point.hxx"

#include <memory>

namespace Anemone
{
    enum class VirtualKey : uint32_t
    {
        None = 0,
        LeftButton = 1,
        RightButton = 2,
        Cancel = 3,
        MiddleButton = 4,
        XButton1 = 5,
        XButton2 = 6,
        Back = 8,
        Tab = 9,
        Clear = 12,
        Enter = 13,
        Shift = 16,
        Control = 17,
        Menu = 18,
        Pause = 19,
        CapitalLock = 20,
        Kana = 21,
        Hangul = 21,
        ImeOn = 22,
        Junja = 23,
        Final = 24,
        Hanja = 25,
        Kanji = 25,
        ImeOff = 26,
        Escape = 27,
        Convert = 28,
        NonConvert = 29,
        Accept = 30,
        ModeChange = 31,
        Space = 32,
        PageUp = 33,
        PageDown = 34,
        End = 35,
        Home = 36,
        Left = 37,
        Up = 38,
        Right = 39,
        Down = 40,
        Select = 41,
        Print = 42,
        Execute = 43,
        Snapshot = 44,
        Insert = 45,
        Delete = 46,
        Help = 47,
        Number0 = 48,
        Number1 = 49,
        Number2 = 50,
        Number3 = 51,
        Number4 = 52,
        Number5 = 53,
        Number6 = 54,
        Number7 = 55,
        Number8 = 56,
        Number9 = 57,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftWindows = 91,
        RightWindows = 92,
        Application = 93,
        Sleep = 95,
        NumberPad0 = 96,
        NumberPad1 = 97,
        NumberPad2 = 98,
        NumberPad3 = 99,
        NumberPad4 = 100,
        NumberPad5 = 101,
        NumberPad6 = 102,
        NumberPad7 = 103,
        NumberPad8 = 104,
        NumberPad9 = 105,
        Multiply = 106,
        Add = 107,
        Separator = 108,
        Subtract = 109,
        Decimal = 110,
        Divide = 111,
        F1 = 112,
        F2 = 113,
        F3 = 114,
        F4 = 115,
        F5 = 116,
        F6 = 117,
        F7 = 118,
        F8 = 119,
        F9 = 120,
        F10 = 121,
        F11 = 122,
        F12 = 123,
        F13 = 124,
        F14 = 125,
        F15 = 126,
        F16 = 127,
        F17 = 128,
        F18 = 129,
        F19 = 130,
        F20 = 131,
        F21 = 132,
        F22 = 133,
        F23 = 134,
        F24 = 135,
        NavigationView = 136,
        NavigationMenu = 137,
        NavigationUp = 138,
        NavigationDown = 139,
        NavigationLeft = 140,
        NavigationRight = 141,
        NavigationAccept = 142,
        NavigationCancel = 143,
        NumberKeyLock = 144,
        Scroll = 145,
        LeftShift = 160,
        RightShift = 161,
        LeftControl = 162,
        RightControl = 163,
        LeftAlt = 164,
        RightAlt = 165,
        GoBack = 166,
        GoForward = 167,
        Refresh = 168,
        Stop = 169,
        Search = 170,
        Favorites = 171,
        GoHome = 172,
        GamepadA = 195,
        GamepadB = 196,
        GamepadX = 197,
        GamepadY = 198,
        GamepadRightShoulder = 199,
        GamepadLeftShoulder = 200,
        GamepadLeftTrigger = 201,
        GamepadRightTrigger = 202,
        GamepadDPadUp = 203,
        GamepadDPadDown = 204,
        GamepadDPadLeft = 205,
        GamepadDPadRight = 206,
        GamepadMenu = 207,
        GamepadView = 208,
        GamepadLeftThumbstickButton = 209,
        GamepadRightThumbstickButton = 210,
        GamepadLeftThumbstickUp = 211,
        GamepadLeftThumbstickDown = 212,
        GamepadLeftThumbstickRight = 213,
        GamepadLeftThumbstickLeft = 214,
        GamepadRightThumbstickUp = 215,
        GamepadRightThumbstickDown = 216,
        GamepadRightThumbstickRight = 217,
        GamepadRightThumbstickLeft = 218
    };

    enum class VirtualKeyModifier : uint8_t
    {
        None = 0u,
        Shift = 1u << 0u,
        Control = 1u << 1u,
        Alt = 1u << 2u,
    };

    using VirtualKeyModifiers = Flags<VirtualKeyModifier>;

    enum class GamepadButton : uint8_t
    {
        A,
        B,
        X,
        Y,
        LeftShoulder,
        RightShoulder,
        LeftTrigger,
        RightTrigger,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Menu,
        View,
        LeftStick,
        RightStick,
        LeftStickUp,
        LeftStickDown,
        LeftStickLeft,
        LeftStickRight,
        RightStickUp,
        RightStickDown,
        RightStickLeft,
        RightStickRight,
    };

    enum class GamepadAxis : uint8_t
    {
        LeftTrigger,
        RightTrigger,
        LeftStickX,
        LeftStickY,
        RightStickX,
        RightStickY,
    };

    struct InputDeviceId final
    {
        uint32_t Inner;
    };

    enum class MouseButton : uint8_t
    {
        Left = 1u << 0u,
        Middle = 1u << 1u,
        Right = 1u << 2u,
        X1 = 1u << 3u,
        X2 = 1u << 4u,
    };

    using MouseButtons = Flags<MouseButton>;

    class Window;

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


    struct RUNTIME_API IApplicationEvents
    {
    private:
        static IApplicationEvents* GCurrent;

    public:
        IApplicationEvents();
        virtual ~IApplicationEvents();

        static IApplicationEvents* GetCurrent()
        {
            return GCurrent;
        }

        // Mouse Events
        virtual void OnMouseEnter(Window& window, MouseEventArgs& args) = 0;
        virtual void OnMouseLeave(Window& window, MouseEventArgs& args) = 0;
        virtual void OnMouseMove(Window& window, MouseMoveEventArgs& args) = 0;
        virtual void OnMouseWheel(Window& window, MouseWheelEventArgs& args) = 0;
        virtual void OnMouseButtonDown(Window& window, MouseButtonEventArgs& args) = 0;
        virtual void OnMouseButtonUp(Window& window, MouseButtonEventArgs& args) = 0;
        virtual void OnMouseButtonDoubleClick(Window& window, MouseButtonEventArgs& args) = 0;

        // Keyboard Events
        virtual void OnKeyDown(Window& window, KeyEventArgs& args) = 0;
        virtual void OnKeyUp(Window& window, KeyEventArgs& args) = 0;
        virtual void OnCharacterReceived(Window& window, CharacterReceivedEventArgs& args) = 0;

        // Input Events
        virtual void OnGamepadAnalog(InputDeviceId device, GamepadAnalogEventArgs& args) = 0;
        virtual void OnGamepadButtonDown(InputDeviceId device, GamepadButtonEventArgs& args) = 0;
        virtual void OnGamepadButtonUp(InputDeviceId device, GamepadButtonEventArgs& args) = 0;

        // Window Events
        virtual void OnWindowClose(Window& window, WindowCloseEventArgs& args) = 0;
        virtual void OnWindowActivated(Window& window, WindowActivatedEventArgs& args) = 0;
        virtual void OnWindowSizeChanged(Window& window, WindowSizeChangedEventArgs& args) = 0;
        virtual void OnWindowLocationChanged(Window& window, WindowLocationChangedEventArgs& args) = 0;
        virtual void OnWindowResizeStarted(Window& window, WindowEventArgs& args) = 0;
        virtual void OnWindowResizeCompleted(Window& window, WindowEventArgs& args) = 0;
        virtual void OnWindowDpiChanged(Window& window, WindowDpiChangedEventArgs& args) = 0;

        // virtual void OnWindowFocusChanged(Window& window, WindowEventArgs& args) = 0; ?
        // virtual void OnWindowStateChanged(Window& window, WindowEventArgs& args) = 0; ?

        // System Events
        virtual void OnEndSession(EndSessionEventArgs& args) = 0;
        virtual void OnSystemSuspending() = 0;
        virtual void OnSystemResuming() = 0;
        virtual void OnDisplayChange() = 0;
    };
}

namespace Anemone
{
    struct Application
    {
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
        RUNTIME_API static void ProcessMessages();

        RUNTIME_API static std::unique_ptr<Window> MakeWindow(WindowType type);
    };
}
