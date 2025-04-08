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
        e.Key = static_cast<VirtualKey>(vk);
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
