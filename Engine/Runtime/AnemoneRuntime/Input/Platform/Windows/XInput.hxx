#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Input/Types.hxx"
#include "AnemoneRuntime/App/IApplicationEvents.hxx"

#include <array>
#include <Xinput.h>

namespace Anemone
{
    class Window;
}

namespace Anemone
{
    class XInput final
    {
    public:
        static XInput& Get();

    public:
        XInput() = default;
        XInput(XInput const&) = delete;
        XInput(XInput&&) = delete;
        XInput& operator=(XInput const&) = delete;
        XInput& operator=(XInput&&) = delete;
        ~XInput() = default;

        void Poll(IApplicationEvents& events);

    private:
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

        void HandleGamepadButton(IApplicationEvents& events, GamepadState& state, GamepadButton button) const;
    };
}
