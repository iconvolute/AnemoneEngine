#include "AnemoneRuntime/Input/Platform/Windows/XInput.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/App/Platform/Windows/Window.hxx"

namespace Anemone
{
    namespace
    {
        Anemone::UninitializedObject<XInput> GXInput;
    }

    void XInput::Initialize()
    {
        GXInput.Create();
    }

    void XInput::Finalize()
    {
        GXInput.Destroy();
    }

    XInput& XInput::Get()
    {
        return *GXInput;
    }
}

namespace Anemone
{
    namespace
    {
        constexpr struct WindowsXInputButtonMapping
        {
            WORD Source;
            GamepadButton Button;
        } GWindowsXInputButtonMapping[]{
            {XINPUT_GAMEPAD_A, GamepadButton ::A},
            {XINPUT_GAMEPAD_B, GamepadButton ::B},
            {XINPUT_GAMEPAD_X, GamepadButton ::X},
            {XINPUT_GAMEPAD_Y, GamepadButton ::Y},
            {XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton ::LeftShoulder},
            {XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton ::RightShoulder},
            {XINPUT_GAMEPAD_DPAD_UP, GamepadButton ::DPadUp},
            {XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton ::DPadDown},
            {XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton ::DPadLeft},
            {XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton ::DPadRight},
            {XINPUT_GAMEPAD_START, GamepadButton ::Menu},
            {XINPUT_GAMEPAD_BACK, GamepadButton ::View},
            {XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton ::LeftStick},
            {XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton ::RightStick},
        };

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
    }

    void XInput::Poll(IApplicationEvents& events)
    {
        XINPUT_STATE state;

        for (DWORD dwUserIndex = 0; dwUserIndex < XUSER_MAX_COUNT; ++dwUserIndex)
        {
            bool const connected = XInputGetState(dwUserIndex, &state) == ERROR_SUCCESS;

            GamepadState& gamepad = this->m_gamepads[dwUserIndex];

            if (connected)
            {
                if (!gamepad.Connected)
                {
                    // Device was connected.
                    gamepad = {};
                    gamepad.DeviceId = InputDeviceId{dwUserIndex};
                }
                else if (gamepad.PacketId == state.dwPacketNumber)
                {
                    // Device was connected and no new data is available.
                    continue;
                }

                gamepad.PacketId = state.dwPacketNumber;

                // Capture buttons state.

                gamepad.ButtonsPressed = [&]
                {
                    GamepadButtonState result{};

                    for (auto const mapping : GWindowsXInputButtonMapping)
                    {
                        result.Set(mapping.Button, state.Gamepad.wButtons & mapping.Source);
                    }

                    result.Set(GamepadButton::LeftTrigger, state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
                    result.Set(GamepadButton::RightTrigger, state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
                    result.Set(GamepadButton::LeftStickUp, state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                    result.Set(GamepadButton::LeftStickDown, state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                    result.Set(GamepadButton::LeftStickLeft, state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                    result.Set(GamepadButton::LeftStickRight, state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                    result.Set(GamepadButton::RightStickUp, state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                    result.Set(GamepadButton::RightStickDown, state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                    result.Set(GamepadButton::RightStickLeft, state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                    result.Set(GamepadButton::RightStickRight, state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                    return result;
                }();

                // Emit events
                this->HandleGamepadButton(events, gamepad, GamepadButton::A);
                this->HandleGamepadButton(events, gamepad, GamepadButton::B);
                this->HandleGamepadButton(events, gamepad, GamepadButton::X);
                this->HandleGamepadButton(events, gamepad, GamepadButton::Y);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftShoulder);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightShoulder);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftTrigger);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightTrigger);
                this->HandleGamepadButton(events, gamepad, GamepadButton::DPadUp);
                this->HandleGamepadButton(events, gamepad, GamepadButton::DPadDown);
                this->HandleGamepadButton(events, gamepad, GamepadButton::DPadLeft);
                this->HandleGamepadButton(events, gamepad, GamepadButton::DPadRight);
                this->HandleGamepadButton(events, gamepad, GamepadButton::Menu);
                this->HandleGamepadButton(events, gamepad, GamepadButton::View);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftStick);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightStick);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftStickUp);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftStickDown);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftStickLeft);
                this->HandleGamepadButton(events, gamepad, GamepadButton::LeftStickRight);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightStickUp);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightStickDown);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightStickLeft);
                this->HandleGamepadButton(events, gamepad, GamepadButton::RightStickRight);

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.bLeftTrigger), 255.0f, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)); value != gamepad.LeftTrigger)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftTrigger;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftTrigger = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.bRightTrigger), 255.0f, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)); value != gamepad.RightTrigger)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightTrigger;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightTrigger = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbLX), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); value != gamepad.LeftStick.X)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftStickX;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftStick.X = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbLY), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); value != gamepad.LeftStick.Y)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftStickY;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftStick.Y = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbRX), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); value != gamepad.RightStick.X)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightStickX;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightStick.X = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbRY), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); value != gamepad.RightStick.Y)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightStickY;
                    e.Value = value;

                    events.OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightStick.Y = value;
                }

                gamepad.ButtonsHeld = gamepad.ButtonsPressed;
            }

            gamepad.Connected = connected;
        }
    }

    void XInput::HandleGamepadButton(IApplicationEvents& events, GamepadState& state, GamepadButton button) const
    {
        bool const pressed = state.ButtonsPressed.Get(button);
        bool const held = state.ButtonsHeld.Get(button);

        if (pressed != held)
        {
            GamepadButtonEventArgs e;
            e.Button = button;

            if (pressed)
            {
                events.OnGamepadButtonDown(state.DeviceId, e);
            }
            else
            {
                events.OnGamepadButtonUp(state.DeviceId, e);
            }
        }
    }
}
