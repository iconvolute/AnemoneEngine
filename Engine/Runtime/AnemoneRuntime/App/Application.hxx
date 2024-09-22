#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"

#include "AnemoneRuntime/App/Window.hxx"

namespace Anemone::App
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

    class InputDevice
    {
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
    };

    class Window;

    struct EventArgs
    {
    };

    struct PointerWheelEventArgs
    {
        Math::PointF Position;
        float Horizontal;
        float Vertical;
        VirtualKeyModifiers Modifiers;
    };

    struct PointerButtonEventArgs
    {
        Math::PointF Position;
        VirtualKey Key;
        VirtualKeyModifiers Modifiers;
    };

    struct PointerEventArgs
    {
        Math::PointF Position;
        VirtualKeyModifiers Modifiers;
    };

    struct PointerMoveEventArgs
    {
        Math::PointF Position;
        VirtualKeyModifiers Modifiers;
        Math::PointF Delta;
        bool Absolute;
    };

    struct CharacterReceivedEventArgs
    {
        char32_t Character{};
        bool Repeat{};
    };

    struct KeyEventArgs
    {
        VirtualKey Key;
        VirtualKeyModifiers Modifiers;
        bool Repeat;
    };

    struct GamepadButtonEventArgs
    {
        VirtualKey Key;
    };

    struct GamepadAnalogEventArgs
    {
        GamepadAxis Axis;
        float Value;
    };

    struct WindowEventArgs : EventArgs
    {
    };

    struct WindowCloseEventArgs : WindowEventArgs
    {
        bool Cancel{false};
    };

    struct WindowActivatedEventArgs : WindowEventArgs
    {
        WindowActivationState State;
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
        uint32_t Dpi;
        float Scale;
    };

    struct ApplicationEventArgs : EventArgs
    {
    };

    struct SystemEventArgs : EventArgs
    {
    };

    struct EndSessionEventArgs : SystemEventArgs
    {
        bool ShuttingDown;
        bool Logoff;
        bool Force;
        bool CloseApplication;
    };

    // NOTE: Rename this to `ApplicationView`?
    class RUNTIME_API Application
    {
    private:
        static Application* GCurrent;

    public:
        static Application* GetCurrent()
        {
            return GCurrent;
        }

    public:
        Application();
        Application(Application const&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application const&) = delete;
        Application& operator=(Application&&) = delete;
        virtual ~Application();

    public:
        static Reference<Window> MakeWindow(WindowType type);
        static void ProcessMessages();

    public:
        virtual void OnWindowClose(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowCloseEventArgs& args){}

        virtual void OnWindowActivated(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowActivatedEventArgs& args) { }

        virtual void OnWindowSizeChanged(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowSizeChangedEventArgs& args) { }

        virtual void OnWindowLocationChanged(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowLocationChangedEventArgs& args) { }

        virtual void OnWindowResizeStarted(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowEventArgs& args) { }

        virtual void OnWindowResizeCompleted(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowEventArgs& args) { }

        virtual void OnWindowDpiChanged(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] WindowDpiChangedEventArgs& args) { }

        virtual void OnCharacterReceived(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] CharacterReceivedEventArgs& args) { }

        virtual void OnKeyUp(
            [[maybe_unused]] Window& sender,
            [[maybe_unused]] KeyEventArgs& args) { }

        virtual void OnKeyDown(
            [[maybe_unused]] Window& sender,
            [[maybe_unused]] KeyEventArgs& args) { }

        virtual void OnPointerEnter(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerEventArgs& args) { }

        virtual void OnPointerLeave(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerEventArgs& args) { }

        virtual void OnPointerMove(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerMoveEventArgs& args) { }

        virtual void OnPointerUp(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerButtonEventArgs& args) { }

        virtual void OnPointerDown(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerButtonEventArgs& args) { }

        virtual void OnPointerWheel(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerWheelEventArgs& args) { }

        virtual void OnPointerDoubleClick(
            [[maybe_unused]] Window& window,
            [[maybe_unused]] PointerButtonEventArgs& args) { }

        virtual void OnGamepadAnalog(
            [[maybe_unused]] InputDeviceId device,
            [[maybe_unused]] GamepadAnalogEventArgs& args) { }

        virtual void OnGamepadButtonDown(
            [[maybe_unused]] InputDeviceId device,
            [[maybe_unused]] GamepadButtonEventArgs& args) { }

        virtual void OnGamepadButtonUp(
            [[maybe_unused]] InputDeviceId& device,
            [[maybe_unused]] GamepadButtonEventArgs& args) { }

        virtual void OnEndSession(
            [[maybe_unused]] EndSessionEventArgs& args) { }

        virtual void OnSystemSuspending(
            [[maybe_unused]] SystemEventArgs& args) { }

        virtual void OnSystemResuming(
            [[maybe_unused]] SystemEventArgs& args) { }

        virtual void OnDisplayChange(
            [[maybe_unused]] SystemEventArgs& args) { }
    };
}
