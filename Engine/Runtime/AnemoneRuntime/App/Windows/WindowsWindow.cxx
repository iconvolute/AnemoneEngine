#include "AnemoneRuntime/App/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/App/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/App/Windows/WindowsRuntime.hxx"

namespace Anemone::App
{
    struct NativeWindowStyle final
    {
        DWORD Style;
        DWORD ExStyle;
    };

    constexpr NativeWindowStyle MapNativeWindowStyle(WindowType type, WindowMode mode)
    {
        switch (type)
        {
        case WindowType::Game:
            switch (mode)
            {
            case WindowMode::Windowed:
                return {
                    .Style = WS_OVERLAPPEDWINDOW,
                    .ExStyle = 0,
                };

            case WindowMode::Borderless:
            case WindowMode::Fullscreen:
                return {
                    .Style = WS_POPUP,
                    .ExStyle = WS_EX_TOPMOST,
                };
            }

        case WindowType::Viewport:
            return {
                .Style = WS_CHILD,
                .ExStyle = WS_EX_TRANSPARENT,
            };

        case WindowType::Form:
            return {
                .Style = WS_OVERLAPPEDWINDOW,
                .ExStyle = WS_EX_APPWINDOW,
            };

        case WindowType::Dialog:
            return {
                .Style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER,
                .ExStyle = WS_EX_DLGMODALFRAME,
            };
        }

        std::unreachable();
    }
}

namespace Anemone::App
{
    // Detail:
    //  WindowsWindow::m_handle is set in the first message sent to the window procedure.
    WindowsWindow::WindowsWindow(Application* application, WindowType type)
        : Window{application}
        , m_type{type}
    {
        auto [dwStyle, dwExStyle] = MapNativeWindowStyle(this->m_type, this->m_mode);

        HWND const hWindow = CreateWindowExW(
            dwExStyle,
            MAKEINTATOM(GWindowsAppState->WindowClass),
            L"Anemone",
            dwStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            GWindowsAppState->Instance,
            this);

        AE_ENSURE(hWindow != nullptr);

        ShowWindow(hWindow, SW_SHOW);
    }

    WindowsWindow::~WindowsWindow()
    {
        if (this->m_handle != nullptr)
        {
            DestroyWindow(this->m_handle);
            this->m_handle = nullptr;
        }
    }

    void WindowsWindow::Close()
    {
        CloseWindow(this->m_handle);
    }

    void WindowsWindow::Minimize()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_handle, SW_MINIMIZE);
        }
    }

    void WindowsWindow::Maximize()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_handle, SW_MAXIMIZE);
        }
    }

    void WindowsWindow::Restore()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_handle, SW_RESTORE);
        }
    }

    void WindowsWindow::BringToFront(bool force)
    {
        if ((this->m_mode == WindowMode::Windowed) && (this->m_type != WindowType::Viewport))
        {
            // Only windowed, top-level windows can be brought to front
            if (IsIconic(this->m_handle) != FALSE)
            {
                // Restore minimized window
                ShowWindow(this->m_handle, SW_RESTORE);
            }
            else
            {
                // Activate non-minimized window
                SetActiveWindow(this->m_handle);
            }

            if (force)
            {
                BringWindowToTop(this->m_handle);
            }
            else
            {
                ShowWindow(this->m_handle, SW_SHOW);
            }
        }
    }

    void WindowsWindow::Focus()
    {
        SetFocus(this->m_handle);
    }

    void WindowsWindow::SetVisible(bool value)
    {
        ShowWindow(this->m_handle, value ? SW_SHOW : SW_HIDE);
    }

    void WindowsWindow::SetEnabled(bool value)
    {
        EnableWindow(this->m_handle, value ? TRUE : FALSE);
    }

    void WindowsWindow::SetInputEnabled(bool value)
    {
        this->m_inputEnabled = value;
    }

    bool WindowsWindow::GetInputEnabled() const
    {
        return this->m_inputEnabled;
    }

    bool WindowsWindow::IsMinimized() const
    {
        return IsIconic(this->m_handle) != FALSE;
    }

    bool WindowsWindow::IsMaximized() const
    {
        return IsZoomed(this->m_handle) != FALSE;
    }

    bool WindowsWindow::IsFocused() const
    {
        return GetFocus() == this->m_handle;
    }

    bool WindowsWindow::IsVisible() const
    {
        return IsWindowVisible(this->m_handle) != FALSE;
    }

    bool WindowsWindow::IsEnabled() const
    {
        return IsWindowEnabled(this->m_handle) != FALSE;
    }

    WindowMode WindowsWindow::GetWindowMode() const
    {
        return this->m_mode;
    }

    void WindowsWindow::SetWindowMode(WindowMode value)
    {
        if (this->m_mode != value)
        {
            if (this->m_type == WindowType::Game)
            {
                if (this->m_mode == WindowMode::Windowed)
                {
                    // Store window placement for going back to windowed mode.
                    this->m_preFullscreenPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(this->m_handle, &this->m_preFullscreenPlacement);
                }

                // Adjust styles.
                auto [dwStyle, dwExStyle] = MapNativeWindowStyle(this->m_type, value);

                SetWindowLongW(this->m_handle, GWL_STYLE, static_cast<::LONG>(dwStyle));
                SetWindowLongW(this->m_handle, GWL_EXSTYLE, static_cast<::LONG>(dwExStyle));

                if (value == WindowMode::Windowed)
                {
                    //::ShowWindow(
                    //    this->m_Handle,
                    //    SW_SHOWNORMAL);

                    SetWindowPlacement(
                        this->m_handle,
                        &this->m_preFullscreenPlacement);
                }
                else
                {
                    SetWindowPos(
                        this->m_handle,
                        HWND_TOP,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                    ShowWindow(
                        this->m_handle,
                        SW_SHOWMAXIMIZED);
                }

                this->m_mode = value;
            }
        }
    }

    void WindowsWindow::SetTitle(std::string_view const& value)
    {
        Platform::win32_string_buffer<wchar_t, 128> wTitle{};
        Platform::win32_WidenString(wTitle, value);

        SetWindowTextW(this->m_handle, wTitle.data());
    }

    Window::NativeHandle WindowsWindow::GetNativeHandle() const
    {
        return {.Inner = this->m_handle};
    }

    Math::RectF WindowsWindow::GetPlacement() const
    {
        WINDOWPLACEMENT wp{.length = sizeof(wp)};
        GetWindowPlacement(this->m_handle, &wp);

        return Platform::win32_into_Rectangle(wp.rcNormalPosition);
    }

    void WindowsWindow::SetPlacement(Math::RectF const& value)
    {
        SetWindowPos(
            this->m_handle,
            nullptr,
            static_cast<int>(value.X),
            static_cast<int>(value.Y),
            static_cast<int>(value.Width),
            static_cast<int>(value.Height),
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    std::optional<Math::SizeF> WindowsWindow::GetMinimumSize() const
    {
        return this->m_minimumSize;
    }

    void WindowsWindow::SetMinimumSize(std::optional<Math::SizeF> const& value)
    {
        this->m_minimumSize = value;

        if (this->m_minimumSize)
        {
            // Trigger DWM to update the window frame.
            SetWindowPos(
                this->m_handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    std::optional<Math::SizeF> WindowsWindow::GetMaximumSize() const
    {
        return this->m_maximumSize;
    }

    void WindowsWindow::SetMaximumSize(std::optional<Math::SizeF> const& value)
    {
        this->m_maximumSize = value;

        if (this->m_minimumSize)
        {
            // Trigger DWM to update the window frame.
            SetWindowPos(
                this->m_handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    Math::RectF WindowsWindow::GetBounds() const
    {
        RECT rc;
        GetWindowRect(this->m_handle, &rc);

        return Platform::win32_into_Rectangle(rc);
    }

    Math::RectF WindowsWindow::GetClientBounds() const
    {
        RECT rc;
        GetClientRect(this->m_handle, &rc);

        return Platform::win32_into_Rectangle(rc);
    }
}

namespace Anemone::App
{
    LRESULT CALLBACK WindowsWindow::WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        WindowsWindow* window = std::bit_cast<WindowsWindow*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

        if (window == nullptr) [[unlikely]]
        {
            if (message == WM_GETMINMAXINFO)
            {
                return DefWindowProcW(handle, message, wparam, lparam);
            }

            AE_ASSERT(message == WM_NCCREATE);

            CREATESTRUCTW const* const params = std::bit_cast<CREATESTRUCTW const*>(lparam);

            window = static_cast<WindowsWindow*>(params->lpCreateParams);
            window->m_handle = handle;

            SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }

        AE_ASSERT(window != nullptr);

        Application* application = Application::GetCurrent();

        bool handled = false;

        if (window->GetInputEnabled())
        {
            handled = GWindowsInput->FilterMessage(*window, message, wparam, lparam);
        }

        if (not handled)
        {
            switch (message)
            {
            case WM_CLOSE:
                {
                    WindowCloseEventArgs args{};
                    application->OnWindowClose(*window, args);

                    if (args.Cancel)
                    {
                        return 0;
                    }

                    break;
                }

            case WM_CHAR:
                {
                    if (IS_HIGH_SURROGATE(wparam))
                    {
                        window->m_characterHighSurrogate = static_cast<uint16_t>(wparam);
                    }
                    else
                    {
                        uint32_t character;
                        uint16_t const lowSurrogate = static_cast<uint16_t>(wparam);

                        if (IS_SURROGATE_PAIR(window->m_characterHighSurrogate, lowSurrogate))
                        {
                            character = Platform::win32_DecodeSurrogatePair(window->m_characterHighSurrogate, lowSurrogate);
                            window->m_characterHighSurrogate = 0;
                        }
                        else
                        {
                            character = lowSurrogate;
                        }

                        uint32_t const keyFlags = HIWORD(lparam);

                        CharacterReceivedEventArgs e{
                            .Character = static_cast<char32_t>(character),
                            .Repeat = (keyFlags & KF_REPEAT) == KF_REPEAT,
                        };

                        application->OnCharacterReceived(*window, e);
                    }

                    break;
                }

            case WM_DPICHANGED:
                {
                    DWORD const dpi = LOWORD(wparam);

                    WindowDpiChangedEventArgs e{
                        .Dpi = dpi,
                        .Scale = static_cast<float>(dpi) / 96.0f,
                    };

                    RECT const& rc = *std::bit_cast<RECT const*>(lparam);

                    SetWindowPos(
                        handle,
                        nullptr,
                        rc.left,
                        rc.top,
                        rc.right - rc.left,
                        rc.bottom - rc.top,
                        SWP_NOZORDER | SWP_NOACTIVATE);

                    application->OnWindowDpiChanged(*window, e);

                    break;
                }

            case WM_POWERBROADCAST:
                {
                    switch (wparam)
                    {
                    case PBT_APMSUSPEND:
                        {
                            SystemEventArgs e{};
                            application->OnSystemSuspending(e);

                            break;
                        }

                    case PBT_APMRESUMESUSPEND:
                        {
                            SystemEventArgs e{};
                            application->OnSystemResuming(e);

                            break;
                        }
                    default:
                        {
                            break;
                        }
                    }

                    break;
                }

            case WM_DISPLAYCHANGE:
                {
                    SystemEventArgs e{};
                    application->OnDisplayChange(e);

                    break;
                }

            case WM_MENUCHAR:
                {
                    // A menu is active and the user presses a key that does not correspond
                    // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
                    return MAKELRESULT(0, MNC_CLOSE);
                }

            case WM_GETDLGCODE:
                {
                    return DLGC_WANTALLKEYS;
                }

            case WM_ENTERSIZEMOVE:
                {
                    window->m_resizing = true;

                    WindowEventArgs e{};
                    application->OnWindowResizeStarted(*window, e);

                    break;
                }

            case WM_EXITSIZEMOVE:
                {
                    {
                        window->m_resizing = false;
                        WindowEventArgs e{};
                        application->OnWindowResizeCompleted(*window, e);
                    }

                    {
                        RECT rc;
                        GetClientRect(handle, &rc);
                        WindowSizeChangedEventArgs e{
                            .Size = Platform::win32_into_Size(rc),
                        };
                        application->OnWindowSizeChanged(*window, e);
                    }
                    {
                        RECT rc;
                        GetWindowRect(handle, &rc);
                        WindowLocationChangedEventArgs e{
                            .Location = Platform::win32_into_Point(rc),
                        };
                        application->OnWindowLocationChanged(*window, e);
                    }

                    break;
                }

            case WM_SIZE:
                {
                    if (wparam != SIZE_MINIMIZED)
                    {
                        if (!window->m_resizing && IsIconic(handle) == FALSE)
                        {
                            WindowSizeChangedEventArgs e{
                                .Size{
                                    .Width = static_cast<float>(LOWORD(lparam)),
                                    .Height = static_cast<float>(HIWORD(lparam)),
                                },
                            };
                            application->OnWindowSizeChanged(*window, e);
                        }
                    }

                    break;
                }

            case WM_GETMINMAXINFO:
                {
                    MINMAXINFO& mmi = *std::bit_cast<MINMAXINFO*>(lparam);

                    if (window->m_minimumSize)
                    {
                        auto [width, height] = window->m_minimumSize.value();

                        mmi.ptMinTrackSize.x = static_cast<LONG>(width);
                        mmi.ptMinTrackSize.y = static_cast<LONG>(height);
                    }

                    if (window->m_maximumSize)
                    {
                        DWORD const dwStyle = GetWindowLongW(handle, GWL_STYLE);
                        DWORD const dwStyleEx = GetWindowLongW(handle, GWL_EXSTYLE);

                        RECT rcBorder{};
                        AdjustWindowRectEx(&rcBorder, dwStyle, FALSE, dwStyleEx);

                        auto [width, height] = window->m_maximumSize.value();

                        mmi.ptMaxTrackSize.x = static_cast<LONG>(width) + (rcBorder.right - rcBorder.left);
                        mmi.ptMaxTrackSize.y = static_cast<LONG>(height) + (rcBorder.bottom - rcBorder.top);
                    }

                    break;
                }

            case WM_NCCALCSIZE:
                {
                    if ((lparam != 0) && (wparam != 0) && (window->m_type == WindowType::Game) && (window->m_mode == WindowMode::Windowed) && IsZoomed(handle))
                    {
                        // Maximized fullscreen border-less game window has visible border in multiple displays
                        // scenario. Limit this by adjusting window placement to just fit display - we are still
                        // render over whole area anyway.

                        WINDOWINFO wi{.cbSize = sizeof(wi)};
                        GetWindowInfo(handle, &wi);

                        NCCALCSIZE_PARAMS& params = *std::bit_cast<LPNCCALCSIZE_PARAMS>(lparam);

                        params.rgrc[0].left += static_cast<LONG>(wi.cxWindowBorders);
                        params.rgrc[0].top += static_cast<LONG>(wi.cyWindowBorders);
                        params.rgrc[0].right -= static_cast<LONG>(wi.cxWindowBorders);
                        params.rgrc[0].bottom -= static_cast<LONG>(wi.cyWindowBorders);

                        params.rgrc[1] = params.rgrc[0];

                        params.lppos->x += static_cast<LONG>(wi.cxWindowBorders);
                        params.lppos->y += static_cast<LONG>(wi.cyWindowBorders);
                        params.lppos->cx -= 2 * static_cast<LONG>(wi.cxWindowBorders);
                        params.lppos->cy -= 2 * static_cast<LONG>(wi.cyWindowBorders);

                        return WVR_VALIDRECTS;
                    }

                    break;
                }

            case WM_ACTIVATE:
                {
                    WindowActivationState state = WindowActivationState::Deactivated;

                    if (wparam == WA_ACTIVE)
                    {
                        state = WindowActivationState::Activated;
                    }
                    else if (wparam == WA_CLICKACTIVE)
                    {
                        state = WindowActivationState::PointerActivated;
                    }

                    if (state == WindowActivationState::Deactivated)
                    {
                        GWindowsInput->Deactivate();
                    }
                    else
                    {
                        GWindowsInput->Activate();
                    }

                    WindowActivatedEventArgs e{
                        .State = state,
                    };

                    application->OnWindowActivated(*window, e);

                    break;
                }

            case WM_ACTIVATEAPP:
                {
                    if (wparam == 0)
                    {
                        GWindowsInput->Deactivate();
                    }
                    else
                    {
                        GWindowsInput->Activate();
                    }

                    break;
                }

            case WM_ENDSESSION:
                {
                    EndSessionEventArgs e{
                        .ShuttingDown = (wparam != FALSE),
                        .Logoff = (lparam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF,
                        .Force = (lparam & ENDSESSION_CRITICAL) == ENDSESSION_CRITICAL,
                        .CloseApplication = (lparam & ENDSESSION_CLOSEAPP) == ENDSESSION_CLOSEAPP,
                    };

                    application->OnEndSession(e);

                    break;
                }

            case WM_SETCURSOR:
                {
                    UINT const hitTest = LOWORD(lparam);

                    if (hitTest == HTCLIENT)
                    {
                        if (GWindowsInput->IsTracking(window->m_handle))
                        {
                            SetCursor(nullptr);
                            return TRUE;
                        }

                        if (window->m_cursor)
                        {
                            SetCursor(*window->m_cursor);
                            return TRUE;
                        }
                    }

                    break;
                }

            case WM_SYSCOMMAND:
                {
                    switch (wparam & 0xFFF0)
                    {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                        {
                            // Prevent screen saver and monitor power events.
                            return 0;
                        }

                    case SC_KEYMENU:
                        {
                            if (window->m_type == WindowType::Game)
                            {
                                // Prevent ALT key from activating the menu bar.
                                return 0;
                            }

                            break;
                        }

                    default:
                        {
                            break;
                        }
                    }

                    break;
                }

            case WM_ERASEBKGND:
                {
                    // TODO: Implement this once we have a rendering system.
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        return DefWindowProcW(handle, message, wparam, lparam);
    }
}
