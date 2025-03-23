#pragma once
#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

//#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
//#include "AnemoneRuntime/Platform/Windows/WindowsWindow.hxx"
//#include "AnemoneRuntime/Platform/Windows/WindowsApplication.hxx"
//#include "AnemoneRuntime/UninitializedObject.hxx"
//#include "AnemoneRuntime/Flags.hxx"

#include <array>
#include <Xinput.h>


namespace Anemone
{
    class WindowsWindow;
}

namespace Anemone
{
    class WindowsInput final
    {
    private:
        constexpr float ApplyLinearDeadzone(float value, float max, float deadzone)
        {
            if (value < -deadzone)
            {
                value += deadzone;
            }
            else if (value > deadzone)
            {
                value -= deadzone;
            }
            else
            {
                return 0.0f;
            }

            float const scaled = value / (max - deadzone);
            return Anemone::Math::Clamp(scaled, -1.0f, 1.0f);
        }

        struct GamepadButtonState final
        {
            uint64_t Value{};

            bool Get(GamepadButton button) const
            {
                uint64_t const mask = uint64_t{1} << static_cast<size_t>(button);
                return (this->Value & mask) != 0;
            }

            void Set(GamepadButton button, bool state)
            {
                uint64_t const mask = uint64_t{1} << static_cast<size_t>(button);

                if (state)
                {
                    this->Value |= mask;
                }
                else
                {
                    this->Value &= ~mask;
                }
            }
        };

        struct GamepadState final
        {
            InputDeviceId DeviceId{};
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

    private:
        void HandleGamepadButton(GamepadState& state, GamepadButton button) const;

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
