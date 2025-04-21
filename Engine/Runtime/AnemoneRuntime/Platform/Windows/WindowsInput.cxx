#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Math/Functions.hxx"

#include <hidusage.h>

namespace Anemone
{
    void WindowsInput::StartTracking(HWND handle)
    {
        RAWINPUTDEVICE const rid{
            .usUsagePage = HID_USAGE_PAGE_GENERIC,
            .usUsage = HID_USAGE_GENERIC_MOUSE,
            .dwFlags = 0,
            .hwndTarget = handle,
        };

        this->TrackMouseWindowHandle = handle;

        RECT rcClient;
        GetClientRect(handle, &rcClient);

        std::array<POINT, 2> points{
            POINT{rcClient.left, rcClient.top},
            POINT{rcClient.right, rcClient.bottom},
        };

        MapWindowPoints(handle, nullptr, points.data(), static_cast<UINT>(points.size()));

        RECT const rcClip{
            .left = points[0].x,
            .top = points[0].y,
            .right = points[1].x,
            .bottom = points[1].y,
        };

        ClipCursor(&rcClip);

        GetCursorPos(&this->TrackMouseRestorePosition);
        this->TrackMouseWindowPosition = this->TrackMouseRestorePosition;

        MapWindowPoints(nullptr, handle, &this->TrackMouseWindowPosition, 1);

        this->RawInputLastCursorPosition.x = INT32_MAX;
        this->RawInputLastCursorPosition.y = INT32_MAX;

        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    void WindowsInput::StopTracking(HWND handle)
    {
        if (handle == this->TrackMouseWindowHandle && handle != nullptr)
        {
            RAWINPUTDEVICE const rid{
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = nullptr,
            };

            this->TrackMouseWindowHandle = nullptr;

            ClipCursor(nullptr);

            SetCursorPos(this->TrackMouseRestorePosition.x, this->TrackMouseRestorePosition.y);

            RegisterRawInputDevices(&rid, 1, sizeof(rid));
        }
    }

    void WindowsInput::Activate()
    {
        this->m_modifiers = CaptureModifiers();
    }

    void WindowsInput::Deactivate()
    {
        this->m_modifiers = CaptureModifiers();

        this->StopTracking(this->TrackMouseWindowHandle);
    }

    static constexpr struct WindowsXInputButtonMapping
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

    static constexpr auto GKeyMapping = [] {
        std::array<VirtualKey, 256> result{};
        result[VK_LBUTTON] = VirtualKey::LeftButton;
        result[VK_RBUTTON] = VirtualKey::RightButton;
        result[VK_CANCEL] = VirtualKey::Cancel;
        result[VK_MBUTTON] = VirtualKey::MiddleButton;
        result[VK_XBUTTON1] = VirtualKey::XButton1;
        result[VK_XBUTTON2] = VirtualKey::XButton2;
        result[VK_BACK] = VirtualKey::Back;
        result[VK_TAB] = VirtualKey::Tab;
        result[VK_CLEAR] = VirtualKey::Clear;
        result[VK_RETURN] = VirtualKey::Enter;
        result[VK_SHIFT] = VirtualKey::Shift;
        result[VK_CONTROL] = VirtualKey::Control;
        result[VK_MENU] = VirtualKey::Menu;
        result[VK_PAUSE] = VirtualKey::Pause;
        result[VK_CAPITAL] = VirtualKey::CapitalLock;
        result[VK_KANA] = VirtualKey::Kana;
        result[VK_HANGUL] = VirtualKey::Hangul;
        result[VK_IME_ON] = VirtualKey::ImeOn;
        result[VK_JUNJA] = VirtualKey::Junja;
        result[VK_FINAL] = VirtualKey::Final;
        result[VK_HANJA] = VirtualKey::Hanja;
        result[VK_KANJI] = VirtualKey::Kanji;
        result[VK_IME_OFF] = VirtualKey::ImeOff;
        result[VK_ESCAPE] = VirtualKey::Escape;
        result[VK_CONVERT] = VirtualKey::Convert;
        result[VK_NONCONVERT] = VirtualKey::NonConvert;
        result[VK_ACCEPT] = VirtualKey::Accept;
        result[VK_MODECHANGE] = VirtualKey::ModeChange;
        result[VK_SPACE] = VirtualKey::Space;
        result[VK_PRIOR] = VirtualKey::PageUp;
        result[VK_NEXT] = VirtualKey::PageDown;
        result[VK_END] = VirtualKey::End;
        result[VK_HOME] = VirtualKey::Home;
        result[VK_LEFT] = VirtualKey::Left;
        result[VK_UP] = VirtualKey::Up;
        result[VK_RIGHT] = VirtualKey::Right;
        result[VK_DOWN] = VirtualKey::Down;
        result[VK_SELECT] = VirtualKey::Select;
        result[VK_PRINT] = VirtualKey::Print;
        result[VK_EXECUTE] = VirtualKey::Execute;
        result[VK_SNAPSHOT] = VirtualKey::Snapshot;
        result[VK_INSERT] = VirtualKey::Insert;
        result[VK_DELETE] = VirtualKey::Delete;
        result[VK_HELP] = VirtualKey::Help;
        result['0'] = VirtualKey::Number0;
        result['1'] = VirtualKey::Number1;
        result['2'] = VirtualKey::Number2;
        result['3'] = VirtualKey::Number3;
        result['4'] = VirtualKey::Number4;
        result['5'] = VirtualKey::Number5;
        result['6'] = VirtualKey::Number6;
        result['7'] = VirtualKey::Number7;
        result['8'] = VirtualKey::Number8;
        result['9'] = VirtualKey::Number9;
        result['A'] = VirtualKey::A;
        result['B'] = VirtualKey::B;
        result['C'] = VirtualKey::C;
        result['D'] = VirtualKey::D;
        result['E'] = VirtualKey::E;
        result['F'] = VirtualKey::F;
        result['G'] = VirtualKey::G;
        result['H'] = VirtualKey::H;
        result['I'] = VirtualKey::I;
        result['J'] = VirtualKey::J;
        result['K'] = VirtualKey::K;
        result['L'] = VirtualKey::L;
        result['M'] = VirtualKey::M;
        result['N'] = VirtualKey::N;
        result['O'] = VirtualKey::O;
        result['P'] = VirtualKey::P;
        result['Q'] = VirtualKey::Q;
        result['R'] = VirtualKey::R;
        result['S'] = VirtualKey::S;
        result['T'] = VirtualKey::T;
        result['U'] = VirtualKey::U;
        result['V'] = VirtualKey::V;
        result['W'] = VirtualKey::W;
        result['X'] = VirtualKey::X;
        result['Y'] = VirtualKey::Y;
        result['Z'] = VirtualKey::Z;
        result[VK_LWIN] = VirtualKey::LeftWindows;
        result[VK_RWIN] = VirtualKey::RightWindows;
        result[VK_APPS] = VirtualKey::Application;
        result[VK_SLEEP] = VirtualKey::Sleep;
        result[VK_NUMPAD0] = VirtualKey::NumberPad0;
        result[VK_NUMPAD1] = VirtualKey::NumberPad1;
        result[VK_NUMPAD2] = VirtualKey::NumberPad2;
        result[VK_NUMPAD3] = VirtualKey::NumberPad3;
        result[VK_NUMPAD4] = VirtualKey::NumberPad4;
        result[VK_NUMPAD5] = VirtualKey::NumberPad5;
        result[VK_NUMPAD6] = VirtualKey::NumberPad6;
        result[VK_NUMPAD7] = VirtualKey::NumberPad7;
        result[VK_NUMPAD8] = VirtualKey::NumberPad8;
        result[VK_NUMPAD9] = VirtualKey::NumberPad9;
        result[VK_MULTIPLY] = VirtualKey::Multiply;
        result[VK_ADD] = VirtualKey::Add;
        result[VK_SEPARATOR] = VirtualKey::Separator;
        result[VK_SUBTRACT] = VirtualKey::Subtract;
        result[VK_DECIMAL] = VirtualKey::Decimal;
        result[VK_DIVIDE] = VirtualKey::Divide;
        result[VK_F1] = VirtualKey::F1;
        result[VK_F2] = VirtualKey::F2;
        result[VK_F3] = VirtualKey::F3;
        result[VK_F4] = VirtualKey::F4;
        result[VK_F5] = VirtualKey::F5;
        result[VK_F6] = VirtualKey::F6;
        result[VK_F7] = VirtualKey::F7;
        result[VK_F8] = VirtualKey::F8;
        result[VK_F9] = VirtualKey::F9;
        result[VK_F10] = VirtualKey::F10;
        result[VK_F11] = VirtualKey::F11;
        result[VK_F12] = VirtualKey::F12;
        result[VK_F13] = VirtualKey::F13;
        result[VK_F14] = VirtualKey::F14;
        result[VK_F15] = VirtualKey::F15;
        result[VK_F16] = VirtualKey::F16;
        result[VK_F17] = VirtualKey::F17;
        result[VK_F18] = VirtualKey::F18;
        result[VK_F19] = VirtualKey::F19;
        result[VK_F20] = VirtualKey::F20;
        result[VK_F21] = VirtualKey::F21;
        result[VK_F22] = VirtualKey::F22;
        result[VK_F23] = VirtualKey::F23;
        result[VK_F24] = VirtualKey::F24;
        result[VK_NAVIGATION_VIEW] = VirtualKey::NavigationView;
        result[VK_NAVIGATION_MENU] = VirtualKey::NavigationMenu;
        result[VK_NAVIGATION_UP] = VirtualKey::NavigationUp;
        result[VK_NAVIGATION_DOWN] = VirtualKey::NavigationDown;
        result[VK_NAVIGATION_LEFT] = VirtualKey::NavigationLeft;
        result[VK_NAVIGATION_RIGHT] = VirtualKey::NavigationRight;
        result[VK_NAVIGATION_ACCEPT] = VirtualKey::NavigationAccept;
        result[VK_NAVIGATION_CANCEL] = VirtualKey::NavigationCancel;
        result[VK_NUMLOCK] = VirtualKey::NumberKeyLock;
        result[VK_SCROLL] = VirtualKey::Scroll;
        result[VK_LSHIFT] = VirtualKey::LeftShift;
        result[VK_RSHIFT] = VirtualKey::RightShift;
        result[VK_LCONTROL] = VirtualKey::LeftControl;
        result[VK_RCONTROL] = VirtualKey::RightControl;
        result[VK_LMENU] = VirtualKey::LeftAlt;
        result[VK_RMENU] = VirtualKey::RightAlt;
        result[VK_BROWSER_BACK] = VirtualKey::GoBack;
        result[VK_BROWSER_FORWARD] = VirtualKey::GoForward;
        result[VK_BROWSER_REFRESH] = VirtualKey::Refresh;
        result[VK_BROWSER_STOP] = VirtualKey::Stop;
        result[VK_BROWSER_SEARCH] = VirtualKey::Search;
        result[VK_BROWSER_FAVORITES] = VirtualKey::Favorites;
        result[VK_BROWSER_HOME] = VirtualKey::GoHome;
        result[VK_GAMEPAD_A] = VirtualKey::GamepadA;
        result[VK_GAMEPAD_B] = VirtualKey::GamepadB;
        result[VK_GAMEPAD_X] = VirtualKey::GamepadX;
        result[VK_GAMEPAD_Y] = VirtualKey::GamepadY;
        result[VK_GAMEPAD_RIGHT_SHOULDER] = VirtualKey::GamepadRightShoulder;
        result[VK_GAMEPAD_LEFT_SHOULDER] = VirtualKey::GamepadLeftShoulder;
        result[VK_GAMEPAD_LEFT_TRIGGER] = VirtualKey::GamepadLeftTrigger;
        result[VK_GAMEPAD_RIGHT_TRIGGER] = VirtualKey::GamepadRightTrigger;
        result[VK_GAMEPAD_DPAD_UP] = VirtualKey::GamepadDPadUp;
        result[VK_GAMEPAD_DPAD_DOWN] = VirtualKey::GamepadDPadDown;
        result[VK_GAMEPAD_DPAD_LEFT] = VirtualKey::GamepadDPadLeft;
        result[VK_GAMEPAD_DPAD_RIGHT] = VirtualKey::GamepadDPadRight;
        result[VK_GAMEPAD_MENU] = VirtualKey::GamepadMenu;
        result[VK_GAMEPAD_VIEW] = VirtualKey::GamepadView;
        result[VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON] = VirtualKey::GamepadLeftThumbstickButton;
        result[VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON] = VirtualKey::GamepadRightThumbstickButton;
        result[VK_GAMEPAD_LEFT_THUMBSTICK_UP] = VirtualKey::GamepadLeftThumbstickUp;
        result[VK_GAMEPAD_LEFT_THUMBSTICK_DOWN] = VirtualKey::GamepadLeftThumbstickDown;
        result[VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT] = VirtualKey::GamepadLeftThumbstickRight;
        result[VK_GAMEPAD_LEFT_THUMBSTICK_LEFT] = VirtualKey::GamepadLeftThumbstickLeft;
        result[VK_GAMEPAD_RIGHT_THUMBSTICK_UP] = VirtualKey::GamepadRightThumbstickUp;
        result[VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN] = VirtualKey::GamepadRightThumbstickDown;
        result[VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT] = VirtualKey::GamepadRightThumbstickRight;
        result[VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT] = VirtualKey::GamepadRightThumbstickLeft;
        return result;
    }();

    constexpr VirtualKey MapSystemKeyToVirtualKey(UINT vk)
    {
#if true
        return static_cast<VirtualKey>(vk);
#else
        if (vk < GKeyMapping.size())
        {
            return GKeyMapping[vk];
        }

        return VirtualKey::None;
#endif
    }

    void WindowsInput::Poll()
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
                this->HandleGamepadButton(gamepad, GamepadButton::A);
                this->HandleGamepadButton(gamepad, GamepadButton::B);
                this->HandleGamepadButton(gamepad, GamepadButton::X);
                this->HandleGamepadButton(gamepad, GamepadButton::Y);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftShoulder);
                this->HandleGamepadButton(gamepad, GamepadButton::RightShoulder);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftTrigger);
                this->HandleGamepadButton(gamepad, GamepadButton::RightTrigger);
                this->HandleGamepadButton(gamepad, GamepadButton::DPadUp);
                this->HandleGamepadButton(gamepad, GamepadButton::DPadDown);
                this->HandleGamepadButton(gamepad, GamepadButton::DPadLeft);
                this->HandleGamepadButton(gamepad, GamepadButton::DPadRight);
                this->HandleGamepadButton(gamepad, GamepadButton::Menu);
                this->HandleGamepadButton(gamepad, GamepadButton::View);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftStick);
                this->HandleGamepadButton(gamepad, GamepadButton::RightStick);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftStickUp);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftStickDown);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftStickLeft);
                this->HandleGamepadButton(gamepad, GamepadButton::LeftStickRight);
                this->HandleGamepadButton(gamepad, GamepadButton::RightStickUp);
                this->HandleGamepadButton(gamepad, GamepadButton::RightStickDown);
                this->HandleGamepadButton(gamepad, GamepadButton::RightStickLeft);
                this->HandleGamepadButton(gamepad, GamepadButton::RightStickRight);

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.bLeftTrigger), 255.0f, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)); value != gamepad.LeftTrigger)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftTrigger;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftTrigger = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.bRightTrigger), 255.0f, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)); value != gamepad.RightTrigger)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightTrigger;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightTrigger = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbLX), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); value != gamepad.LeftStick.X)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftStickX;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftStick.X = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbLY), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); value != gamepad.LeftStick.Y)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::LeftStickY;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.LeftStick.Y = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbRX), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); value != gamepad.RightStick.X)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightStickX;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightStick.X = value;
                }

                if (float const value = ApplyLinearDeadzone(static_cast<float>(state.Gamepad.sThumbRY), 32767.0f, static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); value != gamepad.RightStick.Y)
                {
                    GamepadAnalogEventArgs e;
                    e.Axis = GamepadAxis::RightStickY;
                    e.Value = value;

                    IApplicationEvents::GetCurrent()->OnGamepadAnalog(gamepad.DeviceId, e);

                    gamepad.RightStick.Y = value;
                }

                gamepad.ButtonsHeld = gamepad.ButtonsPressed;
            }

            gamepad.Connected = connected;
        }
    }

    void WindowsInput::HandleGamepadButton(WindowsInput::GamepadState& state, GamepadButton button) const
    {
        bool const pressed = state.ButtonsPressed.Get(button);
        bool const held = state.ButtonsHeld.Get(button);

        if (pressed != held)
        {
            GamepadButtonEventArgs e;
            e.Button = button;

            if (pressed)
            {
                IApplicationEvents::GetCurrent()->OnGamepadButtonDown(state.DeviceId, e);
            }
            else
            {
                IApplicationEvents::GetCurrent()->OnGamepadButtonUp(state.DeviceId, e);
            }
        }
    }

    bool WindowsInput::FilterMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam)
    {
        return this->HandleKeyboardMessage(window, message, wparam, lparam) ||
            this->HandleMouseMessage(window, message, wparam, lparam);
    }

    VirtualKeyModifiers WindowsInput::CaptureModifiers()
    {
        VirtualKeyModifiers modifiers{};

        if (GetKeyState(VK_MENU) & Interop::win32_GetKeyState_Down)
        {
            modifiers |= VirtualKeyModifier::Alt;
        }
        else if (GetKeyState(VK_CONTROL) & Interop::win32_GetKeyState_Down)
        {
            modifiers |= VirtualKeyModifier::Control;
        }
        else if (GetKeyState(VK_SHIFT) & Interop::win32_GetKeyState_Down)
        {
            modifiers |= VirtualKeyModifier::Shift;
        }

        return modifiers;
    }

    bool WindowsInput::HandleKeyboardMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam)
    {
        bool pressed = false;

        switch (message)
        {
        default:
            return false;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            pressed = true;
            break;
        }

        int32_t vk = LOWORD(wparam);
        uint32_t const keyFlags = HIWORD(lparam);

        IApplicationEvents* events = IApplicationEvents::GetCurrent();

        switch (vk)
        {
        default:
            break;

        case VK_SHIFT:
        case VK_CONTROL:
        case VK_MENU:
            {
                VirtualKeyModifiers modifier = VirtualKeyModifier::Alt;

                if (vk == VK_CONTROL)
                {
                    modifier = VirtualKeyModifier::Control;
                }
                else if (vk == VK_SHIFT)
                {
                    modifier = VirtualKeyModifier::Shift;
                }

                if (pressed)
                {
                    this->m_modifiers |= modifier;
                }
                else
                {
                    this->m_modifiers &= ~modifier;
                }

                if ((vk == VK_SHIFT) && (!pressed))
                {
                    {
                        KeyEventArgs e{
                            VirtualKey::LeftShift,
                            this->m_modifiers,
                            false,
                        };
                        events->OnKeyDown(window, e);
                    }
                    {
                        KeyEventArgs e{
                            VirtualKey::RightShift,
                            this->m_modifiers,
                            false,
                        };
                        events->OnKeyDown(window, e);
                    }
                }

                // Combine extended key.
                bool const extendedKey = (keyFlags & KF_EXTENDED) != 0;
                UINT32 const scanCode = LOBYTE(keyFlags) | (extendedKey ? 0xE000 : 0);
                vk = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
                break;
            }
        }

        KeyEventArgs e;
        e.Key = MapSystemKeyToVirtualKey(vk);
        e.Modifiers = this->m_modifiers;
        e.Repeat = (keyFlags & KF_REPEAT) != 0;

        if (pressed)
        {
            events->OnKeyDown(window, e);
        }
        else
        {
            events->OnKeyUp(window, e);
        }

        return true;
    }

    constexpr MouseButtons MouseKeyStateToMouseButtons(UINT keyState)
    {
        MouseButtons result{};

        if (keyState & MK_LBUTTON)
        {
            result |= MouseButton::Left;
        }

        if (keyState & MK_MBUTTON)
        {
            result |= MouseButton::Middle;
        }

        if (keyState & MK_RBUTTON)
        {
            result |= MouseButton::Right;
        }

        if (keyState & MK_XBUTTON1)
        {
            result |= MouseButton::X1;
        }

        if (keyState & MK_XBUTTON2)
        {
            result |= MouseButton::X2;
        }

        return result;
    }

    bool WindowsInput::HandleMouseMessage(WindowsWindow& window, UINT message, WPARAM wparam, LPARAM lparam)
    {
        switch (message)
        {
        default:
            return false;

        case WM_INPUT:
            {
                this->HandleRawInputMessage(window, GET_RAWINPUT_CODE_WPARAM(wparam), std::bit_cast<HRAWINPUT>(lparam));
                break;
            }

        case WM_LBUTTONDOWN:
            {
                this->HandleMouseButtonDown(window, VirtualKey::LeftButton, lparam);
                break;
            }
        case WM_RBUTTONDOWN:
            {
                this->HandleMouseButtonDown(window, VirtualKey::RightButton, lparam);
                break;
            }
        case WM_MBUTTONDOWN:
            {
                this->HandleMouseButtonDown(window, VirtualKey::MiddleButton, lparam);
                break;
            }
        case WM_XBUTTONDOWN:
            {
                VirtualKey const key = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
                    ? VirtualKey::XButton1
                    : VirtualKey::XButton2;
                this->HandleMouseButtonDown(window, key, lparam);
                break;
            }

        case WM_LBUTTONUP:
            {
                this->HandleMouseButtonUp(window, VirtualKey::LeftButton, lparam);
                break;
            }
        case WM_RBUTTONUP:
            {
                this->HandleMouseButtonUp(window, VirtualKey::RightButton, lparam);
                break;
            }
        case WM_MBUTTONUP:
            {
                this->HandleMouseButtonUp(window, VirtualKey::MiddleButton, lparam);
                break;
            }
        case WM_XBUTTONUP:
            {
                VirtualKey const key = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
                    ? VirtualKey::XButton1
                    : VirtualKey::XButton2;
                this->HandleMouseButtonUp(window, key, lparam);
                break;
            }

        case WM_LBUTTONDBLCLK:
            {
                this->HandleMouseButtonClick(window, VirtualKey::LeftButton, lparam);
                break;
            }
        case WM_RBUTTONDBLCLK:
            {
                this->HandleMouseButtonClick(window, VirtualKey::RightButton, lparam);
                break;
            }
        case WM_MBUTTONDBLCLK:
            {
                this->HandleMouseButtonClick(window, VirtualKey::MiddleButton, lparam);
                break;
            }
        case WM_XBUTTONDBLCLK:
            {
                VirtualKey const key = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
                    ? VirtualKey::XButton1
                    : VirtualKey::XButton2;
                this->HandleMouseButtonClick(window, key, lparam);
                break;
            }

        case WM_MOUSEMOVE:
            {
                if (this->TrackMouseWindowHandle == nullptr)
                {
                    // Report mouse move only when mouse tracking is disabled.
                    this->HandleMouseMove(window, lparam);
                }
                break;
            }

        case WM_MOUSEWHEEL:
            {
                this->HandleMouseWheel(
                    window,
                    0.0f,
                    static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam)) / static_cast<float>(WHEEL_DELTA),
                    lparam);
                break;
            }

        case WM_MOUSEHWHEEL:
            {
                this->HandleMouseWheel(
                    window,
                    static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam)) / static_cast<float>(WHEEL_DELTA),
                    0.0f,
                    lparam);
                break;
            }
        }

        return true;
    }

    void WindowsInput::HandleRawInputMessage(WindowsWindow& window, DWORD code, HRAWINPUT handle)
    {
        if (code == RIM_INPUT)
        {
            RAWINPUT ri;
            UINT dwSize = sizeof(ri);

            UINT const result = GetRawInputData(handle, RID_INPUT, &ri, &dwSize, sizeof(RAWINPUTHEADER));

            if (result > 0)
            {
                RAWMOUSE const& mouse = ri.data.mouse;
                POINT& last = this->RawInputLastCursorPosition;

                bool const isAbsolute = (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;

                if (isAbsolute)
                {
                    bool const isVirtualDesktop = (mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;

                    int const width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
                    int const height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

                    int const absoluteX = (mouse.lLastX * width) >> 16;
                    int const absoluteY = (mouse.lLastY * height) >> 16;

                    if (last.x == INT32_MAX)
                    {
                        last.x = absoluteX;
                        last.y = absoluteY;
                    }

                    int const relativeX = absoluteX - last.x;
                    int const relativeY = absoluteY - last.y;

                    last.x = absoluteX;
                    last.y = absoluteY;

                    if ((relativeX != 0) || (relativeY != 0))
                    {
                        MouseMoveEventArgs e;
                        e.Position = Math::PointF{
                            static_cast<float>(TrackMouseWindowPosition.x),
                            static_cast<float>(TrackMouseWindowPosition.y),
                        };
                        e.Modifiers = this->m_modifiers;
                        e.Delta = Math::PointF{
                            static_cast<float>(relativeX),
                            static_cast<float>(relativeY),
                        };
                        e.Absolute = true;

                        IApplicationEvents::GetCurrent()->OnMouseMove(window, e);
                    }
                }
                else if ((mouse.lLastX != 0) || (mouse.lLastY != 0))
                {
                    last.x += mouse.lLastX;
                    last.y += mouse.lLastY;

                    MouseMoveEventArgs e;
                    e.Position = Math::PointF{
                        static_cast<float>(TrackMouseWindowPosition.x),
                        static_cast<float>(TrackMouseWindowPosition.y),
                    };
                    e.Modifiers = this->m_modifiers;
                    e.Delta = Math::PointF{
                        static_cast<float>(mouse.lLastX),
                        static_cast<float>(mouse.lLastY),
                    };
                    e.Absolute = true;

                    IApplicationEvents::GetCurrent()->OnMouseMove(window, e);
                }
            }
        }
    }

    void WindowsInput::HandleMouseButtonUp(WindowsWindow& window, VirtualKey key, LPARAM lparam) const
    {
        MouseButtonEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(static_cast<short>(LOWORD(lparam))),
            static_cast<float>(static_cast<short>(HIWORD(lparam))),
        };
        e.Key = key;
        e.Modifiers = this->m_modifiers;

        IApplicationEvents::GetCurrent()->OnMouseButtonUp(window, e);
    }

    void WindowsInput::HandleMouseButtonDown(WindowsWindow& window, VirtualKey key, LPARAM lparam) const
    {
        MouseButtonEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(static_cast<short>(LOWORD(lparam))),
            static_cast<float>(static_cast<short>(HIWORD(lparam))),
        };
        e.Key = key;
        e.Modifiers = this->m_modifiers;

        IApplicationEvents::GetCurrent()->OnMouseButtonDown(window, e);
    }

    void WindowsInput::HandleMouseButtonClick(WindowsWindow& window, VirtualKey key, LPARAM lparam) const
    {
        MouseButtonEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(static_cast<short>(LOWORD(lparam))),
            static_cast<float>(static_cast<short>(HIWORD(lparam))),
        };
        e.Key = key;
        e.Modifiers = this->m_modifiers;

        IApplicationEvents::GetCurrent()->OnMouseButtonDoubleClick(window, e);
    }

    void WindowsInput::HandleMouseMove(WindowsWindow& window, LPARAM lparam) const
    {
        MouseMoveEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(static_cast<short>(LOWORD(lparam))),
            static_cast<float>(static_cast<short>(HIWORD(lparam))),
        };
        e.Modifiers = this->m_modifiers;
        e.Delta = Math::PointF{};
        e.Absolute = true;

        IApplicationEvents::GetCurrent()->OnMouseMove(window, e);
    }

    void WindowsInput::HandleMouseWheel(WindowsWindow& window, float horizontal, float vertical, LPARAM lParam) const
    {
        POINT location{
            .x = static_cast<short>(LOWORD(lParam)),
            .y = static_cast<short>(HIWORD(lParam)),
        };

        MapWindowPoints(nullptr, window.GetNativeHandle(), &location, 1);

        MouseWheelEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(location.x),
            static_cast<float>(location.y),
        };
        e.Horizontal = horizontal;
        e.Vertical = vertical;
        e.Modifiers = this->m_modifiers;

        IApplicationEvents::GetCurrent()->OnMouseWheel(window, e);
    }
}
