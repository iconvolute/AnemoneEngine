#include "AnemoneRuntime/App/Platform/Windows/Window.hxx"
#include "AnemoneRuntime/App/Platform/Windows/Application.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/Dwm.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Input/Platform/Windows/WindowsInput.hxx"

namespace Anemone
{
    namespace
    {
        const WCHAR GWindowClassName[] = L"AnemoneWindow";

        constexpr Interop::Windows::WindowStyle MapNativeWindowStyle(WindowType type, WindowMode mode)
        {
            switch (type)
            {
            case WindowType::Game:
                switch (mode)
                {
                case WindowMode::Windowed:
                    return {WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW};

                case WindowMode::Borderless:
                case WindowMode::Fullscreen:
                    return {WS_POPUP, WS_EX_TOPMOST};
                }

            case WindowType::Viewport:
                return {WS_CHILD, WS_EX_TRANSPARENT};

            case WindowType::Form:
                return {WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW};

            case WindowType::Dialog:
                return {WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER, WS_EX_DLGMODALFRAME};
            }

            return {};
        }
    }

    Window::Window(
        Application* application,
        WindowType windowType,
        WindowMode windowMode)
        : _application{application}
        , _events{application->_events}
        , _type{windowType}
        , _mode{windowMode}
    {
        HINSTANCE const hInstance = GetModuleHandleW(nullptr);

        RegisterWindowClass();

        Interop::Windows::WindowStyle style = MapNativeWindowStyle(this->_type, this->_mode);

        HWND const handle = CreateWindowExW(
            style.ExStyle,
            GWindowClassName,
            L"Anemone",
            style.Style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            hInstance,
            this);

        if (not handle)
        {
            AE_VERIFY_WIN32(GetLastError());
        }

        if (HRESULT const hr = Interop::Windows::SetWindowCornerPreference(handle, DWMWCP_DONOTROUND); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
        }

        if (HRESULT const hr = Interop::Windows::SetWindowUseImmersiveDarkMode(handle, TRUE); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
        }

        if (handle == nullptr)
        {
            Debug::ReportApplicationStop("Failed to create window.");
        }

        ShowWindow(handle, SW_SHOWNORMAL);
        UpdateWindow(handle);

        this->_application->_windows.PushBack(this);
    }

    Window::~Window()
    {
        this->_application->_windows.Remove(this);

        if (this->_handle)
        {
            DestroyWindow(this->_handle);
            this->_handle = nullptr;
        }
    }

    void* Window::GetNativeHandle() const
    {
        return this->_handle;
    }

    void Window::Close()
    {
        AE_ASSERT(this->ValidateState());

        CloseWindow(this->_handle);
    }

    bool Window::IsClosed()
    {
        return this->_closed;
    }

    void Window::Minimize()
    {
        AE_ASSERT(this->ValidateState());

        if (this->_mode == WindowMode::Windowed)
        {
            ShowWindow(this->_handle, SW_MINIMIZE);
        }
    }

    bool Window::IsMinimized()
    {
        AE_ASSERT(this->ValidateState());

        return IsIconic(this->_handle);
    }

    void Window::Maximize()
    {
        AE_ASSERT(this->ValidateState());

        if (this->_mode == WindowMode::Windowed)
        {
            ShowWindow(this->_handle, SW_MAXIMIZE);
        }
    }

    bool Window::IsMaximized()
    {
        AE_ASSERT(this->ValidateState());

        return IsZoomed(this->_handle);
    }

    void Window::Restore()
    {
        AE_ASSERT(this->ValidateState());

        if (this->_mode == WindowMode::Windowed)
        {
            ShowWindow(this->_handle, SW_RESTORE);
        }
    }

    void Window::BringToFront(bool force)
    {
        AE_ASSERT(this->ValidateState());

        if ((this->_mode == WindowMode::Windowed) and (this->_type != WindowType::Viewport))
        {
            // Only windowed, top-level windows can be brought to front
            if (IsIconic(this->_handle) != FALSE)
            {
                // Restore minimized window
                ShowWindow(this->_handle, SW_RESTORE);
            }
            else
            {
                // Activate non-minimized window
                SetActiveWindow(this->_handle);
            }

            if (force)
            {
                BringWindowToTop(this->_handle);
            }
            else
            {
                ShowWindow(this->_handle, SW_SHOW);
            }
        }
    }

    void Window::Focus()
    {
        AE_ASSERT(this->ValidateState());

        SetFocus(this->_handle);
    }

    bool Window::IsFocused()
    {
        AE_ASSERT(this->ValidateState());

        return GetFocus() == this->_handle;
    }

    bool Window::IsActive()
    {
        AE_ASSERT(this->ValidateState());

        return this->_active;
    }

    void Window::Activate()
    {
        AE_ASSERT(this->ValidateState());

        SetForegroundWindow(this->_handle);
    }

    void Window::SetVisible(bool value)
    {
        AE_ASSERT(this->ValidateState());

        ShowWindow(this->_handle, value ? SW_SHOW : SW_HIDE);
    }

    bool Window::IsVisible()
    {
        AE_ASSERT(this->ValidateState());

        return not IsIconic(this->_handle) and IsWindowVisible(this->_handle);
    }

    void Window::SetEnabled(bool value)
    {
        AE_ASSERT(this->ValidateState());

        EnableWindow(this->_handle, value ? TRUE : FALSE);
    }

    bool Window::IsEnabled()
    {
        AE_ASSERT(this->ValidateState());

        return IsWindowEnabled(this->_handle);
    }

    void Window::SetInputEnabled(bool value)
    {
        AE_ASSERT(this->ValidateState());

        this->_inputEnabled = value;
    }

    bool Window::GetInputEnabled()
    {
        AE_ASSERT(this->ValidateState());

        return this->_inputEnabled;
    }

    WindowMode Window::GetMode()
    {
        AE_ASSERT(this->ValidateState());

        return this->_mode;
    }

    void Window::SetMode(WindowMode value)
    {
        AE_ASSERT(this->ValidateState());

        if (this->_mode != value)
        {
            // Mode change is allowed only for game windows.

            if (this->_type == WindowType::Game)
            {
                if (this->_mode == WindowMode::Windowed)
                {
                    // Store window placement for going back to windowed mode.
                    this->_preFullscreenPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(this->_handle, &this->_preFullscreenPlacement);
                }

                // Update mode.
                this->_mode = value;

                // Adjust styles.
                Interop::Windows::WindowStyle style = MapNativeWindowStyle(this->_type, this->_mode);

                SetWindowLongW(this->_handle, GWL_STYLE, static_cast<::LONG>(style.Style));
                SetWindowLongW(this->_handle, GWL_EXSTYLE, static_cast<::LONG>(style.ExStyle));

                if (value == WindowMode::Windowed)
                {
                    // Restore original window placement.
                    SetWindowPlacement(
                        this->_handle,
                        &this->_preFullscreenPlacement);
                }
                else
                {
                    // Maximize window.
                    SetWindowPos(
                        this->_handle,
                        HWND_TOP,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                    ShowWindow(
                        this->_handle,
                        SW_SHOWMAXIMIZED);
                }
            }
            else
            {
                AE_TRACE(Error, "Only game window can change mode. (handle: {})", fmt::ptr(this->_handle));
            }
        }
    }

    void Window::SetTitle(std::string_view value)
    {
        AE_ASSERT(this->ValidateState());

        Interop::string_buffer<wchar_t, 128> wTitle{};
        Interop::Windows::WidenString(wTitle, value);

        SetWindowTextW(this->_handle, wTitle.data());
    }

    std::optional<Math::SizeF> Window::GetMinimumSize()
    {
        AE_ASSERT(this->ValidateState());

        return this->_minimumSize;
    }

    void Window::SetMinimumSize(std::optional<Math::SizeF> value)
    {
        AE_ASSERT(this->ValidateState());

        this->_minimumSize = value;

        if (this->_minimumSize)
        {
            // Update window frame
            SetWindowPos(
                this->_handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    std::optional<Math::SizeF> Window::GetMaximumSize()
    {
        AE_ASSERT(this->ValidateState());

        return this->_maximumSize;
    }

    void Window::SetMaximumSize(std::optional<Math::SizeF> value)
    {
        AE_ASSERT(this->ValidateState());

        this->_maximumSize = value;

        if (this->_maximumSize)
        {
            // Update window frame
            SetWindowPos(
                this->_handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    void Window::SetCursor(CursorType value)
    {
        AE_ASSERT(this->ValidateState());

        this->_cursorType = value;
        this->_cursorHandle = this->_application->GetCursor(value);
    }

    CursorType Window::GetCursor()
    {
        AE_ASSERT(this->ValidateState());

        return this->_cursorType;
    }

    Math::RectF Window::GetBounds()
    {
        AE_ASSERT(this->ValidateState());

        if (not IsIconic(this->_handle))
        {
            // If window is not minimized, get and update cached window bounds.
            RECT rc{};

            if (not GetWindowRect(this->_handle, &rc))
            {
                AE_VERIFY_WIN32(GetLastError());
            }

            this->_cachedWindowBounds = Interop::Windows::ToRectF(rc);
        }

        return this->_cachedWindowBounds;
    }

    void Window::SetBounds(Math::RectF value)
    {
        AE_ASSERT(this->ValidateState());

        SetWindowPos(
            this->_handle,
            nullptr,
            static_cast<int>(value.X),
            static_cast<int>(value.Y),
            static_cast<int>(value.Width),
            static_cast<int>(value.Height),
            SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    Math::RectF Window::GetClientBounds()
    {
        AE_ASSERT(this->ValidateState());

        if (not IsIconic(this->_handle))
        {
            // If window is not minimized, get and update cached client bounds.
            RECT rc{};

            if (not GetClientRect(this->_handle, &rc))
            {
                AE_VERIFY_WIN32(GetLastError());
            }

            this->_cachedClientBounds = Interop::Windows::ToRectF(rc);
        }

        return this->_cachedClientBounds;
    }

    void Window::RegisterWindowClass()
    {
        //
        // Try to register window class.
        //

        HINSTANCE hInstance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wndClassEx{};
        // TODO: Verify if cbSize is needed.

        if (not GetClassInfoExW(hInstance, GWindowClassName, &wndClassEx))
        {
            wndClassEx.cbSize = sizeof(WNDCLASSEXW);
            wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
            wndClassEx.lpfnWndProc = WndProc;
            wndClassEx.hInstance = hInstance;
            wndClassEx.hIcon = this->_application->_appIcon;
            wndClassEx.hCursor = this->_application->_cursorArrow;
            wndClassEx.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
            wndClassEx.lpszClassName = GWindowClassName;
            wndClassEx.hIconSm = this->_application->_appIcon;

            if (not RegisterClassExW(&wndClassEx))
            {
                Debug::ReportApplicationStop("Failed to register window class.");
            }
        }
    }

    Window* Window::GetWindowFromHandle(HWND handle)
    {
        return std::bit_cast<Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
    }

    LRESULT CALLBACK Window::WndProc(
        HWND handle,
        UINT message,
        WPARAM wparam,
        LPARAM lparam)
    {
        AE_ASSERT(handle);

        Interop::Windows::WindowMessage msg{
            .HWnd = handle,
            .Message = message,
            .WParam = wparam,
            .LParam = lparam,
        };

        Interop::Windows::WindowMessageResult result{};

        if (message == WM_NCCREATE)
        {
            result = WmNcCreate(msg);
        }
        else if (Window* window = GetWindowFromHandle(handle))
        {
            if (window->GetInputEnabled())
            {
                result = WindowsInput::Get().ProcessMessage(*window, msg);
            }

            if (not result.Handled)
            {
                switch (message)
                {
                case WM_CLOSE:
                    result = window->WmClose(msg);
                    break;

                case WM_CHAR:
                    result = window->WmChar(msg);
                    break;

                case WM_DPICHANGED:
                    result = window->WmDpiChanged(msg);
                    break;

                case WM_POWERBROADCAST:
                    result = window->WmPowerBroadcast(msg);
                    break;

                case WM_DISPLAYCHANGE:
                    result = window->WmDisplayChange(msg);
                    break;

                case WM_MENUCHAR:
                    result = window->WmMenuChar(msg);

                    break;

                case WM_GETDLGCODE:
                    result = window->WmGetDlgCode(msg);
                    break;

                case WM_ENTERSIZEMOVE:
                    result = window->WmEnterSizeMove(msg);
                    break;

                case WM_EXITSIZEMOVE:
                    result = window->WmExitSizeMove(msg);
                    break;

                case WM_SIZE:
                    result = window->WmSize(msg);
                    break;

                case WM_GETMINMAXINFO:
                    result = window->WmGetMinMaxInfo(msg);
                    break;

                case WM_NCCALCSIZE:
                    result = window->WmNcCalcSize(msg);
                    break;

                case WM_ACTIVATE:
                    result = window->WmActivate(msg);
                    break;

                case WM_ACTIVATEAPP:
                    result = window->WmActivateApp(msg);
                    break;

                case WM_ENDSESSION:
                    result = window->WmEndSession(msg);
                    break;

                case WM_SETCURSOR:
                    result = window->WmSetCursor(msg);
                    break;

                case WM_SYSCOMMAND:
                    result = window->WmSysCommand(msg);
                    break;

                case WM_ERASEBKGND:
                    result = window->WmEraseBackground(msg);
                    break;

                default:
                    break;
                }
            }
        }

        if (not result.Handled)
        {
            return DefWindowProcW(handle, message, wparam, lparam);
        }

        return result.Result;
    }

    Interop::Windows::WindowMessageResult Window::WmNcCreate(
        Interop::Windows::WindowMessage const& message)
    {
        CREATESTRUCTW* cs = std::bit_cast<CREATESTRUCTW*>(message.LParam);
        Window* self = static_cast<Window*>(cs->lpCreateParams);

        AE_ASSERT(self != nullptr);
        AE_ASSERT(self->_handle == nullptr);

        self->_handle = message.HWnd;
        SetWindowLongPtrW(message.HWnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(self));

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmClose(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;

        WindowCloseEventArgs e{};

        this->_events->OnWindowClose(*this, e);

        if (not e.Cancel)
        {
            // If was not canceled, destroy window and clear handle.
            this->_closed = true;
            DestroyWindow(this->_handle);
            this->_handle = nullptr;
        }

        return Interop::Windows::WindowMessageResult::WithResult(0);
    }

    Interop::Windows::WindowMessageResult Window::WmChar(
        Interop::Windows::WindowMessage const& message)
    {
        if (IS_HIGH_SURROGATE(message.WParam))
        {
            this->_characterHighSurrogate = static_cast<uint16_t>(message.WParam);
        }
        else
        {
            uint16_t const lowSurrogate = static_cast<uint16_t>(message.WParam);

            uint32_t character;

            if (IS_SURROGATE_PAIR(this->_characterHighSurrogate, lowSurrogate))
            {
                character = Interop::Windows::DecodeSurrogatePair(this->_characterHighSurrogate, lowSurrogate);
                this->_characterHighSurrogate = 0;
            }
            else
            {
                character = lowSurrogate;
            }

            uint32_t const keyFlags = HIWORD(message.LParam);

            CharacterReceivedEventArgs e{
                .Character = static_cast<char32_t>(character),
                .Repeat = (keyFlags & KF_REPEAT) == KF_REPEAT,
            };

            this->_events->OnCharacterReceived(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmDpiChanged(
        Interop::Windows::WindowMessage const& message)
    {
        DWORD const dpi = LOWORD(message.WParam);

        WindowDpiChangedEventArgs e{
            .Dpi = static_cast<float>(dpi),
            .Scale = static_cast<float>(dpi) / 96.0f,
        };

        RECT const& rc = *std::bit_cast<RECT const*>(message.LParam);

        SetWindowPos(
            this->_handle,
            nullptr,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        this->_events->OnWindowDpiChanged(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmPowerBroadcast(
        Interop::Windows::WindowMessage const& message)
    {
        switch (message.WParam)
        {
        case PBT_APMSUSPEND:
            {
                this->_events->OnSystemSuspending();
                break;
            }

        case PBT_APMRESUMESUSPEND:
            {
                this->_events->OnSystemResuming();
                break;
            }

        default:
            {
                break;
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmDisplayChange(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;
        this->_events->OnDisplayChange();

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmMenuChar(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;

        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return Interop::Windows::WindowMessageResult::WithResult(MAKELRESULT(0, MNC_CLOSE));
    }

    Interop::Windows::WindowMessageResult Window::WmGetDlgCode(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;
        return Interop::Windows::WindowMessageResult::WithResult(DLGC_WANTALLKEYS);
    }

    Interop::Windows::WindowMessageResult Window::WmEnterSizeMove(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;
        this->_resizing = true;

        WindowEventArgs e{};
        this->_events->OnWindowResizeStarted(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmExitSizeMove(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;

        this->_resizing = false;

        {
            WindowEventArgs e{};
            this->_events->OnWindowResizeCompleted(*this, e);
        }

        {
            RECT rc;
            GetClientRect(this->_handle, &rc);
            WindowSizeChangedEventArgs e{
                .Size = Interop::Windows::ToSizeF(rc),
            };
            this->_events->OnWindowSizeChanged(*this, e);
        }
        {
            RECT rc;
            GetWindowRect(this->_handle, &rc);
            WindowLocationChangedEventArgs e{
                .Location = Interop::Windows::ToPointF(rc),
            };
            this->_events->OnWindowLocationChanged(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmSize(
        Interop::Windows::WindowMessage const& message)
    {
        if ((message.WParam != SIZE_MINIMIZED) && (!this->_resizing) && !IsIconic(this->_handle))
        {
            WindowSizeChangedEventArgs e{
                .Size{
                    .Width = static_cast<float>(LOWORD(message.LParam)),
                    .Height = static_cast<float>(HIWORD(message.LParam)),
                },
            };

            this->_events->OnWindowSizeChanged(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmGetMinMaxInfo(
        Interop::Windows::WindowMessage const& message)
    {
        MINMAXINFO& mmi = *std::bit_cast<MINMAXINFO*>(message.LParam);

        if (this->_minimumSize)
        {
            auto [width, height] = this->_minimumSize.value();

            mmi.ptMinTrackSize.x = static_cast<LONG>(width);
            mmi.ptMinTrackSize.y = static_cast<LONG>(height);
        }

        if (this->_maximumSize)
        {
            DWORD const dwStyle = GetWindowLongW(this->_handle, GWL_STYLE);
            DWORD const dwStyleEx = GetWindowLongW(this->_handle, GWL_EXSTYLE);

            RECT rcBorder{};
            AdjustWindowRectEx(&rcBorder, dwStyle, FALSE, dwStyleEx);

            auto [width, height] = this->_maximumSize.value();

            mmi.ptMaxTrackSize.x = static_cast<LONG>(width) + (rcBorder.right - rcBorder.left);
            mmi.ptMaxTrackSize.y = static_cast<LONG>(height) + (rcBorder.bottom - rcBorder.top);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmNcCalcSize(
        Interop::Windows::WindowMessage const& message)
    {
        if ((this->_type == WindowType::Game) and (this->_mode != WindowMode::Windowed) and IsZoomed(this->_handle))
        {
            // Maximized fullscreen border-less game window has visible border in multiple displays
            // scenario. Limit this by adjusting window placement to just fit display - we are still
            // render over whole area anyway.

            if ((message.WParam != 0) and (message.LParam != 0))
            {
                WINDOWINFO wi{.cbSize = sizeof(wi)};
                GetWindowInfo(this->_handle, &wi);

                NCCALCSIZE_PARAMS& params = *std::bit_cast<LPNCCALCSIZE_PARAMS>(message.LParam);

                params.rgrc[0].left += static_cast<LONG>(wi.cxWindowBorders);
                params.rgrc[0].top += static_cast<LONG>(wi.cyWindowBorders);
                params.rgrc[0].right -= static_cast<LONG>(wi.cxWindowBorders);
                params.rgrc[0].bottom -= static_cast<LONG>(wi.cyWindowBorders);

                params.rgrc[1] = params.rgrc[0];

                params.lppos->x += static_cast<LONG>(wi.cxWindowBorders);
                params.lppos->y += static_cast<LONG>(wi.cyWindowBorders);
                params.lppos->cx -= 2 * static_cast<LONG>(wi.cxWindowBorders);
                params.lppos->cy -= 2 * static_cast<LONG>(wi.cyWindowBorders);

                return Interop::Windows::WindowMessageResult::WithResult(WVR_VALIDRECTS);
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmActivate(
        Interop::Windows::WindowMessage const& message)
    {
        UINT const reason = LOWORD(message.WParam);

        this->_active = (reason == WA_ACTIVE) or (reason == WA_CLICKACTIVE);

        WindowsInput& windowsInput = WindowsInput::Get();

        if (this->_active)
        {
            windowsInput.NotifyWindowActivated(*this);
        }
        else
        {
            windowsInput.NotifyWindowDeactivated(*this);
        }

        WindowActivatedEventArgs e{
            .Activated = this->_active,
        };

        this->_events->OnWindowActivated(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmActivateApp(
        Interop::Windows::WindowMessage const& message)
    {
        WindowsInput& windowsInput = WindowsInput::Get();

        if (message.WParam == 0)
        {
            windowsInput.NotifyWindowActivated(*this);
            windowsInput.NotifyApplicationActivated();
        }
        else
        {
            windowsInput.NotifyWindowDeactivated(*this);
            windowsInput.NotifyApplicationDeactivated();
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmEndSession(
        Interop::Windows::WindowMessage const& message)
    {
        UINT const reason = static_cast<UINT>(message.LParam);

        EndSessionEventArgs e{
            .LogOff = (reason & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF,
            .Shutdown = (message.WParam != FALSE),
            .Force = (reason & ENDSESSION_CRITICAL) == ENDSESSION_CRITICAL,
            .CloseApplication = (reason & ENDSESSION_CLOSEAPP) == ENDSESSION_CLOSEAPP,
        };

        this->_events->OnEndSession(e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmSetCursor(
        Interop::Windows::WindowMessage const& message)
    {
        UINT const hitTest = LOWORD(message.LParam);

        if (hitTest == HTCLIENT)
        {
            if (WindowsInput::Get().IsTracking(this->_handle))
            {
                // Hide cursor if we are tracking it.
                ::SetCursor(nullptr);
                return Interop::Windows::WindowMessageResult::WithResult(TRUE);
            }

            if (this->_cursorHandle)
            {
                // Set the cursor to the one we are using.
                ::SetCursor(*this->_cursorHandle);
                return Interop::Windows::WindowMessageResult::WithResult(TRUE);
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmSysCommand(
        Interop::Windows::WindowMessage const& message)
    {
        switch (message.WParam & 0xFFF0u)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            {
                // Prevent screen saver and monitor power events.
                return Interop::Windows::WindowMessageResult::WithResult(0);
            }

        case SC_KEYMENU:
            {
                if (this->_type == WindowType::Game)
                {
                    // Prevent ALT key from activating the menu bar.
                    return Interop::Windows::WindowMessageResult::WithResult(0);
                }

                break;
            }

        default:
            {
                break;
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmEraseBackground(
        Interop::Windows::WindowMessage const& message)
    {
        (void)message;
        // Not implemented yet.
        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult Window::WmShowWindow(
        Interop::Windows::WindowMessage const& message)
    {
        WindowVisibilityChangedEventArgs e{
            .Visible = message.WParam != FALSE,
        };

        this->_events->OnWindowVisibilityChanged(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }
}
