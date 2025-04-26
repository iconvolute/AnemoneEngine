#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/System/Application.hxx"

#include <array>
#include <Xinput.h>

namespace Anemone::Internal::Windows
{
    class WindowImpl;

    // Implements input management for Windows.
    class XInputImpl final
    {
    public:
        XInputImpl() = default;
        XInputImpl(XInputImpl const&) = delete;
        XInputImpl(XInputImpl&&) = delete;
        XInputImpl& operator=(XInputImpl const&) = delete;
        XInputImpl& operator=(XInputImpl&&) = delete;
        ~XInputImpl() = default;

        void Poll();

    private:
        static float ApplyLinearDeadzone(float value, float max, float deadzone);

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

        void HandleGamepadButton(GamepadState& state, GamepadButton button) const;
    };
}
