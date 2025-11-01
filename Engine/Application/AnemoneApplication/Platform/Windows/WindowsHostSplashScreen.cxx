#include "AnemoneApplication/Platform/Windows/WindowsHostSplashScreen.hxx"
#include "AnemoneBase/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsHostSplashScreen> gWindowsHostSplashScreen{};
    }

    void HostSplashScreen::Initialize()
    {
        gWindowsHostSplashScreen.Create();
    }

    void HostSplashScreen::Finalize()
    {
        gWindowsHostSplashScreen.Destroy();
    }

    HostSplashScreen& HostSplashScreen::Get()
    {
        return *gWindowsHostSplashScreen;
    }

    WindowsHostSplashScreen::WindowsHostSplashScreen()
    {
        this->_sync.Attach(
            CreateEventW(
                nullptr,
                FALSE,
                FALSE,
                nullptr));

        this->_thread.Attach(
            CreateThread(
                nullptr,
                0,
                &WindowsHostSplashScreen::ThreadProc,
                this,
                0,
                nullptr));
    }

    WindowsHostSplashScreen::~WindowsHostSplashScreen()
    {
        WaitForSingleObject(this->_sync.Get(), INFINITE);

        if (this->_window)
        {
            PostMessageW(this->_window, WM_CLOSE, 0, 0);
        }

        WaitForSingleObject(this->_thread.Get(), INFINITE);

        this->_thread = {};
        this->_sync = {};
    }

    void WindowsHostSplashScreen::SetText(std::string_view text)
    {
        UniqueLock scope{this->_lock};
        Interop::Windows::WidenString(this->_text, text);

        InvalidateRect(this->_window, nullptr, FALSE);
    }

    void WindowsHostSplashScreen::SetProgress(float progress)
    {
        UniqueLock scope{this->_lock};
        this->_progress = progress;

        InvalidateRect(this->_window, nullptr, FALSE);
    }

    LRESULT CALLBACK WindowsHostSplashScreen::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        WindowsHostSplashScreen* self = nullptr;

        if (msg == WM_NCCREATE)
        {
            auto const create = std::bit_cast<LPCREATESTRUCTW>(lparam);
            self = static_cast<WindowsHostSplashScreen*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        else
        {
            self = std::bit_cast<WindowsHostSplashScreen*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        switch (msg)
        {
        case WM_PAINT:
            {
                PAINTSTRUCT ps{};
                HDC hdc = BeginPaint(hwnd, &ps);
                {
                    HDC cdc = CreateCompatibleDC(hdc);
                    {
                        RECT rect{};
                        GetClientRect(hwnd, &rect);
                        HBITMAP bmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
                        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(cdc, bmp));

                        FillRect(cdc, &rect, GetSysColorBrush(COLOR_APPWORKSPACE));

                        if (self)
                        {
                            HFONT SystemFontHandle = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
                            UniqueLock scope{self->_lock};

                            SelectObject(cdc, SystemFontHandle);
                            SetTextColor(cdc, RGB(0, 0, 0));
                            SetBkMode(cdc, TRANSPARENT);
                            DrawTextW(
                                cdc,
                                self->_text.c_str(),
                                -1,
                                &rect,
                                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                        }

                        BitBlt(
                            hdc,
                            rect.left,
                            rect.top,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            cdc,
                            0,
                            0,
                            0x00CC0020); // SRCCOPY
                        SelectObject(cdc, oldBmp);
                        DeleteObject(bmp);

                        DeleteDC(cdc);
                    }
                }
                EndPaint(hwnd, &ps);
                break;
            }

        case WM_ERASEBKGND:
            {
                return 1;
            }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_NCHITTEST:
            {
                LRESULT result = DefWindowProcW(hwnd, msg, wparam, lparam);

                if (result == HTCLIENT)
                {
                    result = HTCAPTION;
                }

                return result;
            }

        default:
            break;
        }

        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    DWORD WindowsHostSplashScreen::ThreadProc(LPVOID lpParameter)
    {
        WindowsHostSplashScreen* self = static_cast<WindowsHostSplashScreen*>(lpParameter);

        HMODULE hInstance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wcex{};
        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_NOCLOSE;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"AnemoneSplashScreen";
        wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

        self->_class = RegisterClassExW(&wcex);

        if (not self->_class)
        {
            return 0;
        }

        POINT ptMouse{};
        GetCursorPos(&ptMouse);

        RECT rcWorkArea{};

        HMONITOR const hMonitor = MonitorFromPoint(ptMouse, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{.cbSize = sizeof(MONITORINFO)};

        if (GetMonitorInfoW(hMonitor, &mi))
        {
            rcWorkArea = mi.rcWork;
        }
        else
        {
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
        }

        constexpr int sizeX = 640;
        constexpr int sizeY = 480;

        int const offsetX = ((rcWorkArea.right - rcWorkArea.left) - sizeX) / 2;
        int const offsetY = ((rcWorkArea.bottom - rcWorkArea.top) - sizeY) / 2;

        int const positionX = rcWorkArea.left + offsetX;
        int const positionY = rcWorkArea.top + offsetY;

        self->_window = CreateWindowExW(
            WS_EX_TOOLWINDOW,
            MAKEINTATOM(self->_class),
            L"Anemone",
            WS_POPUP,
            positionX,
            positionY,
            sizeX,
            sizeY,
            nullptr,
            nullptr,
            hInstance,
            self);

        SetEvent(self->_sync.Get());

        if (self->_window)
        {
            // SetLayeredWindowAttributes(self->_window, 0, 0x44, LWA_ALPHA);

            ShowWindow(self->_window, SW_SHOW);
            UpdateWindow(self->_window);

            MSG msg{};

            while (GetMessageW(&msg, nullptr, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        UnregisterClassW(MAKEINTATOM(self->_class), hInstance);

        return 0;
    }
}
