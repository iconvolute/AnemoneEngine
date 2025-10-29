#include "AnemoneApplication/Platform/Windows/WindowsHostInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#include "AnemoneBase/UninitializedObject.hxx"
#include "AnemoneInterop/Windows/UI.hxx"

#include <hidusage.h>

namespace Anemone
{
    namespace
    {
        constexpr auto GKeyMapping = []
        {
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
    }

    namespace
    {
        UninitializedObject<WindowsHostInput> gWindowsHostInput;
    }

    void WindowsHostInput::Initialize()
    {
        gWindowsHostInput.Create();
    }

    void WindowsHostInput::Finalize()
    {
        gWindowsHostInput.Destroy();
    }

    WindowsHostInput& WindowsHostInput::Get()
    {
        return *gWindowsHostInput;
    }

    void WindowsHostInput::NotifyWindowActivated(WindowsHostWindow& window)
    {
        (void)window;
        this->_keyModifiers = CaptureModifiers();
    }

    void WindowsHostInput::NotifyWindowDeactivated(WindowsHostWindow& window)
    {
        (void)window;
        this->_keyModifiers = CaptureModifiers();

        this->StopTracking(this->_trackMouseWindowHandle);
    }

    void WindowsHostInput::NotifyApplicationActivated()
    {
        this->_keyModifiers = CaptureModifiers();
    }

    void WindowsHostInput::NotifyApplicationDeactivated()
    {
        this->_keyModifiers = CaptureModifiers();

        this->StopTracking(this->_trackMouseWindowHandle);
    }

    bool WindowsHostInput::StartTracking(HWND handle)
    {
        RAWINPUTDEVICE const rid{
            .usUsagePage = HID_USAGE_PAGE_GENERIC,
            .usUsage = HID_USAGE_GENERIC_MOUSE,
            .dwFlags = 0,
            .hwndTarget = handle,
        };

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        {
            this->_trackMouseWindowHandle = handle;

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

            GetCursorPos(&this->_trackMouseRestorePosition);
            this->_trackMouseWindowPosition = this->_trackMouseRestorePosition;

            MapWindowPoints(nullptr, handle, &this->_trackMouseWindowPosition, 1);

            this->_rawInputLastCursorPosition.x = INT32_MAX;
            this->_rawInputLastCursorPosition.y = INT32_MAX;

            return true;
        }

        return false;
    }

    void WindowsHostInput::StopTracking(HWND handle)
    {
        if (handle == this->_trackMouseWindowHandle && handle != nullptr)
        {
            // Unregister the raw input device
            RAWINPUTDEVICE const rid{
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = nullptr,
            };

            RegisterRawInputDevices(&rid, 1, sizeof(rid));

            this->_trackMouseWindowHandle = nullptr;

            // Unclip the cursor
            ClipCursor(nullptr);

            // Restore the cursor position
            SetCursorPos(this->_trackMouseRestorePosition.x, this->_trackMouseRestorePosition.y);
        }
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::ProcessMessage(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message)
    {
        switch (message.Message)
        {
        case WM_KEYUP:
        case WM_SYSKEYUP:
            return this->WmKey(
                window,
                message,
                FALSE);

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            return this->WmKey(
                window,
                message,
                TRUE);

        case WM_INPUT:
            return this->WmInput(
                window,
                message);

        case WM_LBUTTONDOWN:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::LeftButton,
                TRUE,
                FALSE);

        case WM_RBUTTONDOWN:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::RightButton,
                TRUE,
                FALSE);

        case WM_MBUTTONDOWN:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::MiddleButton,
                TRUE,
                FALSE);

        case WM_XBUTTONDOWN:
            return this->WmMouseButton(
                window,
                message,
                GET_XBUTTON_WPARAM(message.WParam) == XBUTTON1 ? VirtualKey::XButton1 : VirtualKey::XButton2,
                TRUE,
                FALSE);

        case WM_LBUTTONUP:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::LeftButton,
                FALSE,
                FALSE);

        case WM_RBUTTONUP:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::RightButton,
                FALSE,
                FALSE);

        case WM_MBUTTONUP:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::MiddleButton,
                FALSE,
                FALSE);

        case WM_XBUTTONUP:
            return this->WmMouseButton(
                window,
                message,
                GET_XBUTTON_WPARAM(message.WParam) == XBUTTON1 ? VirtualKey::XButton1 : VirtualKey::XButton2,
                FALSE,
                FALSE);

        case WM_LBUTTONDBLCLK:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::LeftButton,
                FALSE,
                TRUE);

        case WM_RBUTTONDBLCLK:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::RightButton,
                FALSE,
                TRUE);

        case WM_MBUTTONDBLCLK:
            return this->WmMouseButton(
                window,
                message,
                VirtualKey::MiddleButton,
                FALSE,
                TRUE);

        case WM_XBUTTONDBLCLK:
            return this->WmMouseButton(
                window,
                message,
                GET_XBUTTON_WPARAM(message.WParam) == XBUTTON1 ? VirtualKey::XButton1 : VirtualKey::XButton2,
                FALSE,
                FALSE);

        case WM_MOUSEMOVE:
            return this->WmMouseMove(
                window,
                message);

        case WM_MOUSEWHEEL:
            return this->WmMouseWheel(
                window,
                message);

        case WM_MOUSEHWHEEL:
            return this->WmMouseHWheel(
                window,
                message);

        default:
            break;
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmInput(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message)
    {
        UINT const code = GET_RAWINPUT_CODE_WPARAM(message.WParam);
        HRAWINPUT const handle = std::bit_cast<HRAWINPUT>(message.LParam);

        if (code == RIM_INPUT)
        {
            RAWINPUT ri;
            UINT dwSize = sizeof(ri);

            UINT const result = GetRawInputData(handle, RID_INPUT, &ri, &dwSize, sizeof(RAWINPUTHEADER));

            if (result > 0)
            {
                RAWMOUSE const& mouse = ri.data.mouse;
                POINT& last = this->_rawInputLastCursorPosition;

                bool const absolute = (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;

                if (absolute)
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
                            static_cast<float>(_trackMouseWindowPosition.x),
                            static_cast<float>(_trackMouseWindowPosition.y),
                        };
                        e.Modifiers = this->_keyModifiers;
                        e.Delta = Math::PointF{
                            static_cast<float>(relativeX),
                            static_cast<float>(relativeY),
                        };
                        e.Absolute = true;

                        window.GetEvents().OnMouseMove(window, e);
                    }
                }
                else if ((mouse.lLastX != 0) || (mouse.lLastY != 0))
                {
                    last.x += mouse.lLastX;
                    last.y += mouse.lLastY;

                    MouseMoveEventArgs e;
                    e.Position = Math::PointF{
                        static_cast<float>(_trackMouseWindowPosition.x),
                        static_cast<float>(_trackMouseWindowPosition.y),
                    };
                    e.Modifiers = this->_keyModifiers;
                    e.Delta = Math::PointF{
                        static_cast<float>(mouse.lLastX),
                        static_cast<float>(mouse.lLastY),
                    };
                    e.Absolute = true;

                    window.GetEvents().OnMouseMove(window, e);
                }
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmKey(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message,
        BOOL pressed)
    {
        UINT32 vk = LOWORD(message.WParam);
        UINT32 keyFlags = HIWORD(message.LParam);

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
                    this->_keyModifiers |= modifier;
                }
                else
                {
                    this->_keyModifiers &= ~modifier;
                }

                if ((vk == VK_SHIFT) && (!pressed))
                {
                    {
                        KeyEventArgs e{
                            VirtualKey::LeftShift,
                            this->_keyModifiers,
                            false,
                        };
                        window.GetEvents().OnKeyDown(window, e);
                    }
                    {
                        KeyEventArgs e{
                            VirtualKey::RightShift,
                            this->_keyModifiers,
                            false,
                        };
                        window.GetEvents().OnKeyDown(window, e);
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
        e.Modifiers = this->_keyModifiers;
        e.Repeat = (keyFlags & KF_REPEAT) != 0;

        if (pressed)
        {
            window.GetEvents().OnKeyDown(window, e);
        }
        else
        {
            window.GetEvents().OnKeyUp(window, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmMouseButton(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message,
        VirtualKey virtualKey,
        BOOL pressed,
        BOOL click)
    {
        MouseButtonEventArgs e;
        e.Position = Math::PointF{
            static_cast<float>(static_cast<short>(LOWORD(message.LParam))),
            static_cast<float>(static_cast<short>(HIWORD(message.LParam))),
        };
        e.Key = virtualKey;
        e.Modifiers = this->_keyModifiers;

        if (pressed)
        {
            if (click)
            {
                window.GetEvents().OnMouseButtonClick(window, e);
            }
            else
            {
                window.GetEvents().OnMouseButtonDown(window, e);
            }
        }
        else
        {
            window.GetEvents().OnMouseButtonUp(window, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmMouseMove(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message)
    {
        // Report mouse move events only if raw input is not enabled for that window.
        if (not this->IsTracking(window.GetHandle()))
        {
            MouseMoveEventArgs e;
            e.Position = Math::PointF{
                static_cast<float>(static_cast<short>(LOWORD(message.LParam))),
                static_cast<float>(static_cast<short>(HIWORD(message.LParam))),
            };
            e.Modifiers = this->_keyModifiers;
            e.Delta = Math::PointF{};
            e.Absolute = true;

            window.GetEvents().OnMouseMove(window, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmMouseWheel(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message)
    {
        POINT location{
            .x = static_cast<short>(LOWORD(message.LParam)),
            .y = static_cast<short>(HIWORD(message.LParam)),
        };

        MapWindowPoints(nullptr, window.GetHandle(), &location, 1);

        MouseWheelEventArgs e;
        e.Position = Interop::Windows::ToPointF(location);
        e.Horizontal = 0.0f;
        e.Vertical = static_cast<float>(GET_WHEEL_DELTA_WPARAM(message.WParam)) / static_cast<float>(WHEEL_DELTA);
        e.Modifiers = this->_keyModifiers;

        window.GetEvents().OnMouseWheel(window, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowsHostInput::WmMouseHWheel(
        WindowsHostWindow& window,
        Interop::Windows::WindowMessage const& message)
    {
        POINT location{
            .x = static_cast<short>(LOWORD(message.LParam)),
            .y = static_cast<short>(HIWORD(message.LParam)),
        };

        MapWindowPoints(nullptr, window.GetHandle(), &location, 1);

        MouseWheelEventArgs e;
        e.Position = Interop::Windows::ToPointF(location);
        e.Horizontal = static_cast<float>(GET_WHEEL_DELTA_WPARAM(message.WParam)) / static_cast<float>(WHEEL_DELTA);
        e.Vertical = 0.0f;
        e.Modifiers = this->_keyModifiers;

        window.GetEvents().OnMouseWheel(window, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    VirtualKeyModifiers WindowsHostInput::CaptureModifiers()
    {
        VirtualKeyModifiers modifiers{};

        if (Interop::Windows::IsKeyPressed(VK_MENU))
        {
            modifiers |= VirtualKeyModifier::Alt;
        }

        if (Interop::Windows::IsKeyPressed(VK_CONTROL))
        {
            modifiers |= VirtualKeyModifier::Control;
        }

        if (Interop::Windows::IsKeyPressed(VK_SHIFT))
        {
            modifiers |= VirtualKeyModifier::Shift;
        }

        return modifiers;
    }

    MouseButtons WindowsHostInput::ConvertToMouseButtons(UINT keyState)
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
}
