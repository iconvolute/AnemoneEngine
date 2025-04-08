// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsError.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <dwmapi.h>

namespace Anemone::Internal
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
        default:
        case WindowType::Game:
            {
                switch (mode)
                {
                default:
                case WindowMode::Windowed:
                    {
                        return {
                            .Style = WS_OVERLAPPEDWINDOW,
                            .ExStyle = 0,
                        };
                    }

                case WindowMode::Borderless:
                case WindowMode::Fullscreen:
                    {
                        return {
                            .Style = WS_POPUP,
                            .ExStyle = WS_EX_TOPMOST,
                        };
                    }
                }
            }

        case WindowType::Viewport:
            {
                return {
                    .Style = WS_CHILD,
                    .ExStyle = WS_EX_TRANSPARENT,
                };
            }

        case WindowType::Form:
            {
                return {
                    .Style = WS_OVERLAPPEDWINDOW,
                    .ExStyle = WS_EX_APPWINDOW,
                };
            }

        case WindowType::Dialog:
            {
                return {
                    .Style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER,
                    .ExStyle = WS_EX_DLGMODALFRAME,
                };
            }
        }
    }
}

namespace Anemone
{
    WindowsWindow::WindowsWindow(WindowType type)
        : m_type{type}
    {
        auto [dwStyle, dwExStyle] = Internal::MapNativeWindowStyle(this->m_type, this->m_mode);

        HINSTANCE const hInstance = GetModuleHandleW(nullptr);

        HWND const handle = CreateWindowExW(
            dwExStyle,
            MAKEINTATOM(Private::GApplicationStatics->MainWindowClass),
            L"Anemone",
            dwStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            hInstance,
            this);

        {
            constexpr DWORD preference = DWMWCP_DONOTROUND;

            if (HRESULT const hr = DwmSetWindowAttribute(handle, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(DWORD)); FAILED(hr))
            {
                AE_VERIFY_HRESULT(hr);
            }
        }

        {
            constexpr BOOL enable = TRUE;

            if (HRESULT const hr = DwmSetWindowAttribute(handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &enable, sizeof(BOOL)); FAILED(hr))
            {
                AE_VERIFY_HRESULT(hr);
            }
        }

        if (handle == nullptr)
        {
            Debugger::ReportApplicationStop("Failed to create window.");
        }

        ShowWindow(handle, SW_SHOWNORMAL);
        UpdateWindow(handle);

        Private::GApplicationStatics->WindowsCollection.PushBack(this);
    }

    WindowsWindow::~WindowsWindow()
    {
        Private::GApplicationStatics->WindowsCollection.Remove(this);

        if (this->m_windowHandle != nullptr)
        {
            DestroyWindow(this->m_windowHandle);
            this->m_windowHandle = nullptr;
        }
    }

    void WindowsWindow::Close()
    {
        CloseWindow(this->m_windowHandle);
    }

    void WindowsWindow::Minimize()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_windowHandle, SW_MINIMIZE);
        }
    }

    bool WindowsWindow::IsMinimized() const
    {
        return IsIconic(this->m_windowHandle) != FALSE;
    }

    void WindowsWindow::Maximize()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_windowHandle, SW_MAXIMIZE);
        }
    }
    bool WindowsWindow::IsMaximized() const
    {
        return IsZoomed(this->m_windowHandle) != FALSE;
    }

    void WindowsWindow::Restore()
    {
        if (this->m_mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_windowHandle, SW_RESTORE);
        }
    }

    void WindowsWindow::BringToFront(bool force)
    {
        if ((this->m_mode == WindowMode::Windowed) && (this->m_type != WindowType::Viewport))
        {
            // Only windowed, top-level windows can be brought to front
            if (IsIconic(this->m_windowHandle) != FALSE)
            {
                // Restore minimized window
                ShowWindow(this->m_windowHandle, SW_RESTORE);
            }
            else
            {
                // Activate non-minimized window
                SetActiveWindow(this->m_windowHandle);
            }

            if (force)
            {
                BringWindowToTop(this->m_windowHandle);
            }
            else
            {
                ShowWindow(this->m_windowHandle, SW_SHOW);
            }
        }
    }

    void WindowsWindow::Focus()
    {
        SetFocus(this->m_windowHandle);
    }

    bool WindowsWindow::IsFocused() const
    {
        return GetFocus() == this->m_windowHandle;
    }

    void WindowsWindow::SetVisible(bool value)
    {
        ShowWindow(this->m_windowHandle, value ? SW_SHOW : SW_HIDE);
    }

    bool WindowsWindow::IsVisible() const
    {
        return IsWindowVisible(this->m_windowHandle) != FALSE;
    }

    void WindowsWindow::SetEnabled(bool value)
    {
        EnableWindow(this->m_windowHandle, value ? TRUE : FALSE);
    }

    bool WindowsWindow::IsEnabled() const
    {
        return IsWindowEnabled(this->m_windowHandle) != FALSE;
    }

    void WindowsWindow::SetInputEnabled(bool value)
    {
        this->m_inputEnabled = value;
    }

    bool WindowsWindow::GetInputEnabled() const
    {
        return this->m_inputEnabled;
    }

    WindowMode WindowsWindow::GetMode() const
    {
        return this->m_mode;
    }

    void WindowsWindow::SetMode(WindowMode value)
    {
        if (this->m_mode != value)
        {
            if (this->m_type == WindowType::Game)
            {
                if (this->m_mode == WindowMode::Windowed)
                {
                    // Store window placement for going back to windowed mode.
                    this->m_preFullscreenPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(this->m_windowHandle, &this->m_preFullscreenPlacement);
                }

                // Adjust styles.
                auto [dwStyle, dwExStyle] = Internal::MapNativeWindowStyle(this->m_type, value);

                SetWindowLongW(this->m_windowHandle, GWL_STYLE, static_cast<::LONG>(dwStyle));
                SetWindowLongW(this->m_windowHandle, GWL_EXSTYLE, static_cast<::LONG>(dwExStyle));

                if (value == WindowMode::Windowed)
                {
                    // Restore original window placement.
                    SetWindowPlacement(
                        this->m_windowHandle,
                        &this->m_preFullscreenPlacement);
                }
                else
                {
                    // Maximize window.
                    SetWindowPos(
                        this->m_windowHandle,
                        HWND_TOP,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                    ShowWindow(
                        this->m_windowHandle,
                        SW_SHOWMAXIMIZED);
                }

                this->m_mode = value;
            }
        }
    }

    void WindowsWindow::SetTitle(std::string_view value)
    {
        Interop::string_buffer<wchar_t, 128> wTitle{};
        Interop::win32_WidenString(wTitle, value);

        SetWindowTextW(this->m_windowHandle, wTitle.data());
    }

    Math::RectF WindowsWindow::GetPlacement() const
    {
        WINDOWPLACEMENT wp{.length = sizeof(wp)};
        GetWindowPlacement(this->m_windowHandle, &wp);

        return Interop::win32_into_Rectangle(wp.rcNormalPosition);
    }

    void WindowsWindow::SetPlacement(Math::RectF value)
    {
        SetWindowPos(
            this->m_windowHandle,
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

    void WindowsWindow::SetMinimumSize(std::optional<Math::SizeF> value)
    {
        this->m_minimumSize = value;

        if (this->m_minimumSize)
        {
            // Update window frame
            SetWindowPos(
                this->m_windowHandle,
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

    void WindowsWindow::SetMaximumSize(std::optional<Math::SizeF> value)
    {
        this->m_maximumSize = value;

        if (this->m_minimumSize)
        {
            // Update window frame
            SetWindowPos(
                this->m_windowHandle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    void WindowsWindow::SetCursor(CursorType value)
    {
        this->m_cursorType = value;
        this->m_cursorHandle = Private::GApplicationStatics->GetCursor(value);

    }

    CursorType WindowsWindow::GetCursor() const
    {
        return this->m_cursorType;
    }

    Math::RectF WindowsWindow::GetBounds() const
    {
        RECT rc;
        GetWindowRect(this->m_windowHandle, &rc);

        return Interop::win32_into_Rectangle(rc);
    }

    Math::RectF WindowsWindow::GetClientBounds() const
    {
        RECT rc;
        GetClientRect(this->m_windowHandle, &rc);

        return Interop::win32_into_Rectangle(rc);
    }


    LRESULT CALLBACK WindowsWindow::WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        WindowsWindow* window = std::bit_cast<WindowsWindow*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

        if (window == nullptr)
            [[unlikely]]
        {
            if (message == WM_NCCREATE)
            {
                // Associate window with the handle.
                CREATESTRUCTW const* const params = std::bit_cast<CREATESTRUCTW const*>(lparam);

                window = static_cast<WindowsWindow*>(params->lpCreateParams);
                window->m_windowHandle = handle;

                SetWindowLongPtrW(handle, GWLP_USERDATA, std::bit_cast<LONG_PTR>(window));
            }
            else
            {
                // Window still being created.
                return DefWindowProcW(handle, message, wparam, lparam);
            }
        }

        AE_ASSERT(window != nullptr);

        IApplicationEvents* events = IApplicationEvents::GetCurrent();

        bool handled = false;

        if (window->GetInputEnabled())
        {
            handled = Private::GApplicationStatics->Input.FilterMessage(*window, message, wparam, lparam);
        }

        if (not handled)
        {
            switch (message)
            {
            case WM_CLOSE:
                {
                    WindowCloseEventArgs e{};
                    events->OnWindowClose(*window, e);

                    if (e.Cancel)
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
                        uint16_t const lowSurrogate = static_cast<uint16_t>(wparam);

                        uint32_t character;

                        if (IS_SURROGATE_PAIR(window->m_characterHighSurrogate, lowSurrogate))
                        {
                            character = Interop::win32_DecodeSurrogatePair(window->m_characterHighSurrogate, lowSurrogate);
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

                        events->OnCharacterReceived(*window, e);
                    }
                    break;
                }

            case WM_DPICHANGED:
                {
                    DWORD const dpi = LOWORD(wparam);

                    WindowDpiChangedEventArgs e{
                        .Dpi = static_cast<float>(dpi),
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

                    events->OnWindowDpiChanged(*window, e);
                    break;
                }

            case WM_POWERBROADCAST:
                {
                    switch (wparam)
                    {
                    case PBT_APMSUSPEND:
                        {
                            events->OnSystemSuspending();
                            break;
                        }

                    case PBT_APMRESUMESUSPEND:
                        {
                            events->OnSystemResuming();
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
                    events->OnDisplayChange();
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
                    events->OnWindowResizeStarted(*window, e);

                    break;
                }

            case WM_EXITSIZEMOVE:
                {
                    {
                        window->m_resizing = false;
                        WindowEventArgs e{};
                        events->OnWindowResizeCompleted(*window, e);
                    }

                    {
                        RECT rc;
                        GetClientRect(handle, &rc);
                        WindowSizeChangedEventArgs e{
                            .Size = Interop::win32_into_Size(rc),
                        };
                        events->OnWindowSizeChanged(*window, e);
                    }
                    {
                        RECT rc;
                        GetWindowRect(handle, &rc);
                        WindowLocationChangedEventArgs e{
                            .Location = Interop::win32_into_Point(rc),
                        };
                        events->OnWindowLocationChanged(*window, e);
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
                            events->OnWindowSizeChanged(*window, e);
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
                    if ((lparam != 0) && (wparam != 0) && (window->m_type == WindowType::Game) && (window->m_mode == WindowMode::Windowed) && (IsZoomed(handle) != FALSE))
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
                    WindowActivatedEventArgs e{
                        .Activated = (wparam == WA_ACTIVE) or (wparam == WA_CLICKACTIVE),
                    };

                    if (e.Activated)
                    {
                        Private::GApplicationStatics->Input.Deactivate();
                    }
                    else
                    {
                        Private::GApplicationStatics->Input.Activate();
                    }

                    events->OnWindowActivated(*window, e);
                    break;
                }

            case WM_ACTIVATEAPP:
                {
                    if (wparam == 0)
                    {
                        Private::GApplicationStatics->Input.Deactivate();
                    }
                    else
                    {
                        Private::GApplicationStatics->Input.Activate();
                    }
                    break;
                }

            case WM_ENDSESSION:
                {
                    UINT const reason = static_cast<UINT>(lparam);

                    EndSessionEventArgs e{
                        .LogOff = (reason & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF,
                        .Shutdown = (wparam != FALSE),
                        .Force = (reason & ENDSESSION_CRITICAL) == ENDSESSION_CRITICAL,
                        .CloseApplication = (reason & ENDSESSION_CLOSEAPP) == ENDSESSION_CLOSEAPP,
                    };

                    events->OnEndSession(e);

                    break;
                }

            case WM_SETCURSOR:
                {
                    UINT const hitTest = LOWORD(lparam);

                    if (hitTest == HTCLIENT)
                    {
                        if (Private::GApplicationStatics->Input.IsTracking(window->m_windowHandle))
                        {
                            ::SetCursor(nullptr);
                            return TRUE;
                        }

                        if (window->m_cursorHandle)
                        {
                            ::SetCursor(*window->m_cursorHandle);
                            return TRUE;
                        }
                    }
                    break;
                }

            case WM_SYSCOMMAND:
                {
                    switch (wparam & 0xFFF0u)
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
                    // TODO: Do not clear the background if we are rendering over it.
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
