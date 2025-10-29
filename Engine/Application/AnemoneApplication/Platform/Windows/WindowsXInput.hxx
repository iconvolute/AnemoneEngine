#pragma once
#include "AnemoneInterop/Windows/UI.hxx"
#include "AnemoneApplication/HostApplicationEvents.hxx"

#include <array>
#include <Xinput.h>

namespace Anemone
{
    class WindowsHostWindow;
}

namespace Anemone
{
    class WindowsXInput final
    {
    public:
        static void Initialize();
        static void Finalize();
        static WindowsXInput& Get();

    public:
        WindowsXInput() = default;
        WindowsXInput(WindowsXInput const&) = delete;
        WindowsXInput(WindowsXInput&&) = delete;
        ~WindowsXInput() = default;
        WindowsXInput& operator=(WindowsXInput const&) = delete;
        WindowsXInput& operator=(WindowsXInput&&) = delete;

        void Poll(HostApplicationEvents& events);

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

        void HandleGamepadButton(HostApplicationEvents& events, GamepadState& state, GamepadButton button) const;
    };
}
