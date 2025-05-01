// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsError.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <dwmapi.h>

namespace Anemone::Internal::Windows
{
    static const WCHAR GWindowClassName[] = L"AnemoneWindow";

    WindowImpl::WindowImpl(WindowType windowType, WindowMode windowMode)
        : m_Type{windowType}
        , m_Mode{windowMode}
    {
        RegisterWindowClass();

        DWORD dwStyle = 0;
        DWORD dwExStyle = 0;
        MapNativeWindowStyle(dwStyle, dwExStyle, this->m_Type, this->m_Mode);

        HWND const handle = CreateWindowExW(
            dwExStyle,
            GWindowClassName,
            L"Anemone",
            dwStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            GInstanceHandle,
            this);

        if (not handle)
        {
            AE_VERIFY_WIN32(GetLastError());
        }

        // TODO: Move to interop namespace
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

        GApplicationStatics->WindowsCollection.PushBack(this);
    }

    WindowImpl::~WindowImpl()
    {
        GApplicationStatics->WindowsCollection.Remove(this);

        if (this->m_Handle)
        {
            DestroyWindow(this->m_Handle);
            this->m_Handle = nullptr;
        }
    }

    void WindowImpl::Close()
    {
        AE_ASSERT(this->ValidateState());

        CloseWindow(this->m_Handle);
    }

    bool WindowImpl::IsClosed()
    {
        return this->m_IsClosed;
    }

    void WindowImpl::Minimize()
    {
        AE_ASSERT(this->ValidateState());

        if (this->m_Mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_Handle, SW_MINIMIZE);
        }
    }

    bool WindowImpl::IsMinimized()
    {
        AE_ASSERT(this->ValidateState());

        return IsIconic(this->m_Handle);
    }

    void WindowImpl::Maximize()
    {
        AE_ASSERT(this->ValidateState());

        if (this->m_Mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_Handle, SW_MAXIMIZE);
        }
    }

    bool WindowImpl::IsMaximized()
    {
        AE_ASSERT(this->ValidateState());

        return IsZoomed(this->m_Handle);
    }

    void WindowImpl::Restore()
    {
        AE_ASSERT(this->ValidateState());

        if (this->m_Mode == WindowMode::Windowed)
        {
            ShowWindow(this->m_Handle, SW_RESTORE);
        }
    }

    void WindowImpl::BringToFront(bool force)
    {
        AE_ASSERT(this->ValidateState());

        if ((this->m_Mode == WindowMode::Windowed) && (this->m_Type != WindowType::Viewport))
        {
            // Only windowed, top-level windows can be brought to front
            if (IsIconic(this->m_Handle) != FALSE)
            {
                // Restore minimized window
                ShowWindow(this->m_Handle, SW_RESTORE);
            }
            else
            {
                // Activate non-minimized window
                SetActiveWindow(this->m_Handle);
            }

            if (force)
            {
                BringWindowToTop(this->m_Handle);
            }
            else
            {
                ShowWindow(this->m_Handle, SW_SHOW);
            }
        }
    }

    void WindowImpl::Focus()
    {
        AE_ASSERT(this->ValidateState());

        SetFocus(this->m_Handle);
    }

    bool WindowImpl::IsFocused()
    {
        AE_ASSERT(this->ValidateState());

        return GetFocus() == this->m_Handle;
    }

    void WindowImpl::SetVisible(bool value)
    {
        AE_ASSERT(this->ValidateState());

        ShowWindow(this->m_Handle, value ? SW_SHOW : SW_HIDE);
    }

    bool WindowImpl::IsVisible()
    {
        AE_ASSERT(this->ValidateState());

        return not IsIconic(this->m_Handle) and IsWindowVisible(this->m_Handle);
    }

    void WindowImpl::SetEnabled(bool value)
    {
        AE_ASSERT(this->ValidateState());

        EnableWindow(this->m_Handle, value ? TRUE : FALSE);
    }

    bool WindowImpl::IsEnabled()
    {
        AE_ASSERT(this->ValidateState());

        return IsWindowEnabled(this->m_Handle);
    }

    void WindowImpl::SetInputEnabled(bool value)
    {
        AE_ASSERT(this->ValidateState());

        this->m_InputEnabled = value;
    }

    bool WindowImpl::GetInputEnabled()
    {
        AE_ASSERT(this->ValidateState());

        return this->m_InputEnabled;
    }

    WindowMode WindowImpl::GetMode()
    {
        AE_ASSERT(this->ValidateState());

        return this->m_Mode;
    }

    void WindowImpl::SetMode(WindowMode value)
    {
        AE_ASSERT(this->ValidateState());

        if (this->m_Mode != value)
        {
            // Mode change is allowed only for game windows.

            if (this->m_Type == WindowType::Game)
            {
                if (this->m_Mode == WindowMode::Windowed)
                {
                    // Store window placement for going back to windowed mode.
                    this->m_PreFullscreenPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(this->m_Handle, &this->m_PreFullscreenPlacement);
                }

                // Update mode.
                this->m_Mode = value;

                // Adjust styles.
                DWORD dwStyle = 0;
                DWORD dwExStyle = 0;
                MapNativeWindowStyle(dwStyle, dwExStyle, this->m_Type, this->m_Mode);

                SetWindowLongW(this->m_Handle, GWL_STYLE, static_cast<::LONG>(dwStyle));
                SetWindowLongW(this->m_Handle, GWL_EXSTYLE, static_cast<::LONG>(dwExStyle));

                if (value == WindowMode::Windowed)
                {
                    // Restore original window placement.
                    SetWindowPlacement(
                        this->m_Handle,
                        &this->m_PreFullscreenPlacement);
                }
                else
                {
                    // Maximize window.
                    SetWindowPos(
                        this->m_Handle,
                        HWND_TOP,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                    ShowWindow(
                        this->m_Handle,
                        SW_SHOWMAXIMIZED);
                }
            }
            else
            {
                AE_TRACE(Error, "Only game window can change mode. (handle: {})", fmt::ptr(this->m_Handle));
            }
        }
    }

    void WindowImpl::SetTitle(std::string_view value)
    {
        AE_ASSERT(this->ValidateState());

        Interop::string_buffer<wchar_t, 128> wTitle{};
        Interop::Windows::WidenString(wTitle, value);

        SetWindowTextW(this->m_Handle, wTitle.data());
    }

    std::optional<Math::SizeF> WindowImpl::GetMinimumSize()
    {
        AE_ASSERT(this->ValidateState());

        return this->m_MinimumSize;
    }

    void WindowImpl::SetMinimumSize(std::optional<Math::SizeF> value)
    {
        AE_ASSERT(this->ValidateState());

        this->m_MinimumSize = value;

        if (this->m_MinimumSize)
        {
            // Update window frame
            SetWindowPos(
                this->m_Handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    std::optional<Math::SizeF> WindowImpl::GetMaximumSize()
    {
        AE_ASSERT(this->ValidateState());

        return this->m_MaximumSize;
    }

    void WindowImpl::SetMaximumSize(std::optional<Math::SizeF> value)
    {
        AE_ASSERT(this->ValidateState());

        this->m_MaximumSize = value;

        if (this->m_MaximumSize)
        {
            // Update window frame
            SetWindowPos(
                this->m_Handle,
                nullptr,
                0,
                0,
                0,
                0,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }

    void WindowImpl::SetCursor(CursorType value)
    {
        AE_ASSERT(this->ValidateState());

        this->m_CursorType = value;
        this->m_CursorHandle = GApplicationStatics->GetCursor(value);
    }

    CursorType WindowImpl::GetCursor()
    {
        AE_ASSERT(this->ValidateState());

        return this->m_CursorType;
    }

    Math::RectF WindowImpl::GetBounds()
    {
        AE_ASSERT(this->ValidateState());

        if (not IsIconic(this->m_Handle))
        {
            // If window is not minimized, get and update cached window bounds.
            RECT rc{};

            if (not GetWindowRect(this->m_Handle, &rc))
            {
                AE_VERIFY_WIN32(GetLastError());
            }

            this->m_CachedWindowBounds = Interop::Windows::ToRectF(rc);
        }

        return this->m_CachedWindowBounds;
    }

    void WindowImpl::SetBounds(Math::RectF value)
    {
        AE_ASSERT(this->ValidateState());

        SetWindowPos(
            this->m_Handle,
            nullptr,
            static_cast<int>(value.X),
            static_cast<int>(value.Y),
            static_cast<int>(value.Width),
            static_cast<int>(value.Height),
            SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    Math::RectF WindowImpl::GetClientBounds()
    {
        AE_ASSERT(this->ValidateState());

        if (not IsIconic(this->m_Handle))
        {
            // If window is not minimized, get and update cached client bounds.
            RECT rc{};

            if (not GetClientRect(this->m_Handle, &rc))
            {
                AE_VERIFY_WIN32(GetLastError());
            }

            this->m_CachedClientBounds = Interop::Windows::ToRectF(rc);
        }

        return this->m_CachedClientBounds;
    }

    bool WindowImpl::ValidateState() const
    {
        return (this->m_Handle != nullptr);
    }

    void WindowImpl::RegisterWindowClass()
    {
        //
        // Try to register window class.
        //

        WNDCLASSEXW wndClassEx{};
        // TODO: Verify if cbSize is needed.

        if (not GetClassInfoExW(GInstanceHandle, GWindowClassName, &wndClassEx))
        {
            wndClassEx.cbSize = sizeof(WNDCLASSEXW);
            wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
            wndClassEx.lpfnWndProc = WindowImpl::WndProc;
            wndClassEx.hInstance = GInstanceHandle;
            wndClassEx.hIcon = GApplicationStatics->ApplicationIconHandle;
            wndClassEx.hCursor = GApplicationStatics->ArrowCursor;
            wndClassEx.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
            wndClassEx.lpszClassName = GWindowClassName;
            wndClassEx.hIconSm = GApplicationStatics->ApplicationIconHandle;

            if (not RegisterClassExW(&wndClassEx))
            {
                Debugger::ReportApplicationStop("Failed to register window class.");
            }
        }
    }

    void WindowImpl::MapNativeWindowStyle(DWORD& style, DWORD& exStyle, WindowType type, WindowMode mode)
    {
        switch (type)
        {
        default:
        case WindowType::Game:
            switch (mode)
            {
            default:
            case WindowMode::Windowed:
                style = WS_OVERLAPPEDWINDOW;
                exStyle = WS_EX_APPWINDOW;
                break;

            case WindowMode::Borderless:
            case WindowMode::Fullscreen:
                style = WS_POPUP;
                exStyle = WS_EX_TOPMOST;
                break;
            }
            break;

        case WindowType::Viewport:
            style = WS_CHILD;
            exStyle = WS_EX_TRANSPARENT;
            break;

        case WindowType::Form:
            style = WS_OVERLAPPEDWINDOW;
            exStyle = WS_EX_APPWINDOW;
            break;

        case WindowType::Dialog:
            style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER;
            exStyle = WS_EX_DLGMODALFRAME;
            break;
        }
    }

    WindowImpl* WindowImpl::GetWindowFromHandle(HWND handle)
    {
        return std::bit_cast<WindowImpl*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
    }

    LRESULT CALLBACK WindowImpl::WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        AE_ASSERT(handle);

        Interop::Windows::WindowMessageResult result{};

        if (message == WM_NCCREATE)
        {
            result = WmNcCreate(handle, lparam);
        }
        else if (WindowImpl* window = GetWindowFromHandle(handle))
        {
            bool handled = false;

            if (window->GetInputEnabled())
            {
                handled = GApplicationStatics->Input.ProcessMessage(result, *window, message, wparam, lparam);
            }

            if (not handled)
            {
                switch (message)
                {
                case WM_CLOSE:
                    result = window->WmClose(wparam, lparam);
                    break;

                case WM_CHAR:
                    result = window->WmChar(wparam, lparam);
                    break;

                case WM_DPICHANGED:
                    result = window->WmDpiChanged(wparam, lparam);
                    break;

                case WM_POWERBROADCAST:
                    result = window->WmPowerBroadcast(wparam, lparam);
                    break;

                case WM_DISPLAYCHANGE:
                    result = window->WmDisplayChange(wparam, lparam);
                    break;

                case WM_MENUCHAR:
                    result = window->WmMenuChar(wparam, lparam);

                    break;

                case WM_GETDLGCODE:
                    result = window->WmGetDlgCode(wparam, lparam);
                    break;

                case WM_ENTERSIZEMOVE:
                    result = window->WmEnterSizeMove(wparam, lparam);
                    break;

                case WM_EXITSIZEMOVE:
                    result = window->WmExitSizeMove(wparam, lparam);
                    break;

                case WM_SIZE:
                    result = window->WmSize(wparam, lparam);
                    break;

                case WM_GETMINMAXINFO:
                    result = window->WmGetMinMaxInfo(wparam, lparam);
                    break;

                case WM_NCCALCSIZE:
                    result = window->WmNcCalcSize(wparam, lparam);
                    break;

                case WM_ACTIVATE:
                    result = window->WmActivate(wparam, lparam);
                    break;

                case WM_ACTIVATEAPP:
                    result = window->WmActivateApp(wparam, lparam);
                    break;

                case WM_ENDSESSION:
                    result = window->WmEndSession(wparam, lparam);
                    break;

                case WM_SETCURSOR:
                    result = window->WmSetCursor(wparam, lparam);
                    break;

                case WM_SYSCOMMAND:
                    result = window->WmSysCommand(wparam, lparam);
                    break;

                case WM_ERASEBKGND:
                    result = window->WmEraseBackground(wparam, lparam);
                    break;

                default:
                    break;
                }
            }
        }

        if (result.Handled)
        {
            return result.Result;
        }

        return DefWindowProcW(handle, message, wparam, lparam);
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmNcCreate(HWND window, LPARAM lparam)
    {
        CREATESTRUCTW* cs = std::bit_cast<CREATESTRUCTW*>(lparam);
        WindowImpl* self = static_cast<WindowImpl*>(cs->lpCreateParams);
        AE_ASSERT(self != nullptr);
        AE_ASSERT(self->m_Handle == nullptr);
        self->m_Handle = window;
        SetWindowLongPtrW(window, GWLP_USERDATA, std::bit_cast<LONG_PTR>(self));

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmClose(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;
        WindowCloseEventArgs e{};
        IApplicationEvents::GetCurrent()->OnWindowClose(*this, e);

        if (not e.Cancel)
        {
            // If was not canceled, destroy window and clear handle.
            this->m_IsClosed = true;
            DestroyWindow(this->m_Handle);
            this->m_Handle = nullptr;
        }

        return Interop::Windows::WindowMessageResult::WithResult(0);
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmChar(WPARAM wparam, LPARAM lparam)
    {
        if (IS_HIGH_SURROGATE(wparam))
        {
            this->m_CharacterHighSurrogate = static_cast<uint16_t>(wparam);
        }
        else
        {
            uint16_t const lowSurrogate = static_cast<uint16_t>(wparam);

            uint32_t character;

            if (IS_SURROGATE_PAIR(this->m_CharacterHighSurrogate, lowSurrogate))
            {
                character = Interop::Windows::DecodeSurrogatePair(this->m_CharacterHighSurrogate, lowSurrogate);
                this->m_CharacterHighSurrogate = 0;
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

            IApplicationEvents::GetCurrent()->OnCharacterReceived(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmDpiChanged(WPARAM wparam, LPARAM lparam)
    {
        DWORD const dpi = LOWORD(wparam);

        WindowDpiChangedEventArgs e{
            .Dpi = static_cast<float>(dpi),
            .Scale = static_cast<float>(dpi) / 96.0f,
        };

        RECT const& rc = *std::bit_cast<RECT const*>(lparam);

        SetWindowPos(
            this->m_Handle,
            nullptr,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        IApplicationEvents::GetCurrent()->OnWindowDpiChanged(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmPowerBroadcast(WPARAM wparam, LPARAM lparam)
    {
        (void)lparam;

        switch (wparam)
        {
        case PBT_APMSUSPEND:
            {
                IApplicationEvents::GetCurrent()->OnSystemSuspending();
                break;
            }

        case PBT_APMRESUMESUSPEND:
            {
                IApplicationEvents::GetCurrent()->OnSystemResuming();
                break;
            }

        default:
            {
                break;
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmDisplayChange(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;
        IApplicationEvents::GetCurrent()->OnDisplayChange();

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmMenuChar(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;

        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return Interop::Windows::WindowMessageResult::WithResult(MAKELRESULT(0, MNC_CLOSE));
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmGetDlgCode(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;
        return Interop::Windows::WindowMessageResult::WithResult(DLGC_WANTALLKEYS);
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmEnterSizeMove(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;
        this->m_Resizing = true;

        WindowEventArgs e{};
        IApplicationEvents::GetCurrent()->OnWindowResizeStarted(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmExitSizeMove(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;

        IApplicationEvents* events = IApplicationEvents::GetCurrent();
        this->m_Resizing = false;

        {
            WindowEventArgs e{};
            events->OnWindowResizeCompleted(*this, e);
        }

        {
            RECT rc;
            GetClientRect(this->m_Handle, &rc);
            WindowSizeChangedEventArgs e{
                .Size = Interop::Windows::ToSizeF(rc),
            };
            events->OnWindowSizeChanged(*this, e);
        }
        {
            RECT rc;
            GetWindowRect(this->m_Handle, &rc);
            WindowLocationChangedEventArgs e{
                .Location = Interop::Windows::ToPointF(rc),
            };
            events->OnWindowLocationChanged(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmSize(WPARAM wparam, LPARAM lparam)
    {
        if ((wparam != SIZE_MINIMIZED) && (!this->m_Resizing) && !IsIconic(this->m_Handle))
        {
            WindowSizeChangedEventArgs e{
                .Size{
                    .Width = static_cast<float>(LOWORD(lparam)),
                    .Height = static_cast<float>(HIWORD(lparam)),
                },
            };
            IApplicationEvents::GetCurrent()->OnWindowSizeChanged(*this, e);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmGetMinMaxInfo(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;

        MINMAXINFO& mmi = *std::bit_cast<MINMAXINFO*>(lparam);

        if (this->m_MinimumSize)
        {
            auto [width, height] = this->m_MinimumSize.value();

            mmi.ptMinTrackSize.x = static_cast<LONG>(width);
            mmi.ptMinTrackSize.y = static_cast<LONG>(height);
        }

        if (this->m_MaximumSize)
        {
            DWORD const dwStyle = GetWindowLongW(this->m_Handle, GWL_STYLE);
            DWORD const dwStyleEx = GetWindowLongW(this->m_Handle, GWL_EXSTYLE);

            RECT rcBorder{};
            AdjustWindowRectEx(&rcBorder, dwStyle, FALSE, dwStyleEx);

            auto [width, height] = this->m_MaximumSize.value();

            mmi.ptMaxTrackSize.x = static_cast<LONG>(width) + (rcBorder.right - rcBorder.left);
            mmi.ptMaxTrackSize.y = static_cast<LONG>(height) + (rcBorder.bottom - rcBorder.top);
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmNcCalcSize(WPARAM wparam, LPARAM lparam)
    {
        if ((this->m_Type == WindowType::Game) and (this->m_Mode != WindowMode::Windowed) and IsZoomed(this->m_Handle))
        {
            // Maximized fullscreen border-less game window has visible border in multiple displays
            // scenario. Limit this by adjusting window placement to just fit display - we are still
            // render over whole area anyway.

            if ((wparam != 0) and (lparam != 0))
            {
                WINDOWINFO wi{.cbSize = sizeof(wi)};
                GetWindowInfo(this->m_Handle, &wi);

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

                return Interop::Windows::WindowMessageResult::WithResult(WVR_VALIDRECTS);
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmActivate(WPARAM wparam, LPARAM lparam)
    {
        (void)lparam;
        UINT const reason = LOWORD(wparam);

        WindowActivatedEventArgs e{
            .Activated = (reason == WA_ACTIVE) or (reason == WA_CLICKACTIVE),
        };

        if (e.Activated)
        {
            GApplicationStatics->Input.NotifyWindowActivated(*this);
        }
        else
        {
            GApplicationStatics->Input.NotifyWindowDeactivated(*this);
        }

        IApplicationEvents::GetCurrent()->OnWindowActivated(*this, e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmActivateApp(WPARAM wparam, LPARAM lparam)
    {
        (void)lparam;

        if (wparam == 0)
        {
            GApplicationStatics->Input.NotifyWindowActivated(*this);
            GApplicationStatics->Input.NotifyApplicationActivated();
        }
        else
        {
            GApplicationStatics->Input.NotifyWindowDeactivated(*this);
            GApplicationStatics->Input.NotifyApplicationDeactivated();
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmEndSession(WPARAM wparam, LPARAM lparam)
    {
        UINT const reason = static_cast<UINT>(lparam);

        EndSessionEventArgs e{
            .LogOff = (reason & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF,
            .Shutdown = (wparam != FALSE),
            .Force = (reason & ENDSESSION_CRITICAL) == ENDSESSION_CRITICAL,
            .CloseApplication = (reason & ENDSESSION_CLOSEAPP) == ENDSESSION_CLOSEAPP,
        };

        IApplicationEvents::GetCurrent()->OnEndSession(e);

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmSetCursor(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        UINT const hitTest = LOWORD(lparam);

        if (hitTest == HTCLIENT)
        {
            if (GApplicationStatics->Input.IsTracking(this->m_Handle))
            {
                // Hide cursor if we are tracking it.
                ::SetCursor(nullptr);
                return Interop::Windows::WindowMessageResult::WithResult(TRUE);
            }

            if (this->m_CursorHandle)
            {
                // Set the cursor to the one we are using.
                ::SetCursor(*this->m_CursorHandle);
                return Interop::Windows::WindowMessageResult::WithResult(TRUE);
            }
        }

        return Interop::Windows::WindowMessageResult::Default();
    }

    Interop::Windows::WindowMessageResult WindowImpl::WmSysCommand(WPARAM wparam, LPARAM lparam)
    {
        (void)lparam;

        switch (wparam & 0xFFF0u)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            {
                // Prevent screen saver and monitor power events.
                return Interop::Windows::WindowMessageResult::WithResult(0);
            }

        case SC_KEYMENU:
            {
                if (this->m_Type == WindowType::Game)
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

    Interop::Windows::WindowMessageResult WindowImpl::WmEraseBackground(WPARAM wparam, LPARAM lparam)
    {
        (void)wparam;
        (void)lparam;
        // Not implemented yet.
        return Interop::Windows::WindowMessageResult::Default();
    }
}
