#pragma once
#include "AnemoneRuntime/App/Application.hxx"
#include "AnemoneRuntime/App/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Flags.hxx"

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <array>
#include <Xinput.h>

namespace Anemone::App
{
    class WindowsInput final
    {
    private:
        struct GamepadButtonState final
        {
            bool A;
            bool B;
            bool X;
            bool Y;
            bool LeftShoulder;
            bool RightShoulder;
            bool LeftTrigger;
            bool RightTrigger;
            bool DPadUp;
            bool DPadDown;
            bool DPadLeft;
            bool DPadRight;
            bool Menu;
            bool View;
            bool LeftStick;
            bool RightStick;
            bool LeftStickUp;
            bool LeftStickDown;
            bool LeftStickLeft;
            bool LeftStickRight;
            bool RightStickUp;
            bool RightStickDown;
            bool RightStickLeft;
            bool RightStickRight;
        };

        struct GamepadState final
        {
            DWORD PacketId{};
            bool Connected{};
            GamepadButtonState ButtonsPressed{};
            GamepadButtonState ButtonsHeld{};
            float LeftTrigger{};
            float RightTrigger{};
            Math::PointF LeftStick{};
            Math::PointF RightStick{};
        };

        GamepadState m_gamepads[XUSER_MAX_COUNT]{};
        VirtualKeyModifiers m_modifiers{};

        HWND TrackMouseWindowHandle{};
        POINT TrackMouseRestorePosition{};
        POINT TrackMouseWindowPosition{};
        POINT RawInputLastCursorPosition{};

    public:
        void StartTracking(HWND handle);
        void StopTracking(HWND handle);
        bool IsTracking(HWND handle) const
        {
            return this->TrackMouseWindowHandle == handle;
        }
        void Activate();
        void Deactivate();
        void Poll();

        bool FilterMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam);

        static VirtualKeyModifiers CaptureModifiers();

        void HandleGamepadButton(InputDeviceId device, VirtualKey key, bool pressed, bool held) const;

        bool HandleKeyboardMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam);
        bool HandleMouseMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam);
        void HandleRawInputMessage(WindowsWindow& window, DWORD code, HRAWINPUT handle);

        void HandleMouseButtonUp(WindowsWindow& window, VirtualKey key, LPARAM lparam) const;
        void HandleMouseButtonDown(WindowsWindow& window, VirtualKey key, LPARAM lparam) const;
        void HandleMouseButtonClick(WindowsWindow& window, VirtualKey key, LPARAM lparam) const;
        void HandleMouseMove(WindowsWindow& window, LPARAM lparam) const;
        void HandleMouseWheel(WindowsWindow& window, float horizontal, float vertical, LPARAM lParam) const;
    };
}
