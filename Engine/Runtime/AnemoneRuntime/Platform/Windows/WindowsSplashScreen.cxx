#include "AnemoneRuntime/Platform/Windows/WindowsSplashScreen.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"

namespace Anemone::Internal
{
    namespace
    {
        struct WindowsSplashScreenStatics final
        {
            HANDLE Thread{};
            ATOM WindowClass{};
            HWND Window{};
            HANDLE Sync{};
        };

        WindowsSplashScreenStatics GWindowsSplashScreenStatics{};

        LRESULT CALLBACK WindowsSplashScreenWndProc(
            HWND hwnd,
            UINT msg,
            WPARAM wparam,
            LPARAM lparam)
        {
            switch (msg)
            {
            case WM_PAINT:
                return DefWindowProcW(hwnd, msg, wparam, lparam);

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

        DWORD WINAPI WindowsSplashScreenThread(LPVOID)
        {
            HMODULE hInstance = GetModuleHandleW(nullptr);

            WNDCLASSEXW wcex{};
            wcex.cbSize = sizeof(wcex);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = WindowsSplashScreenWndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
            wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
            wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            wcex.lpszMenuName = nullptr;
            wcex.lpszClassName = L"AnemoneSplashScreen";
            wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

            GWindowsSplashScreenStatics.WindowClass = RegisterClassExW(&wcex);

            if (!GWindowsSplashScreenStatics.WindowClass)
            {
                return 0;
            }

            int const width = 640;
            int const height = 480;

            int const centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
            int const centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

            int const positionX = centerX - width / 2;
            int const positionY = centerY - height / 2;

            GWindowsSplashScreenStatics.Window = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                MAKEINTATOM(GWindowsSplashScreenStatics.WindowClass),
                L"Anemone",
                WS_POPUP,
                positionX,
                positionY,
                width,
                height,
                nullptr,
                nullptr,
                hInstance,
                nullptr);

            SetEvent(GWindowsSplashScreenStatics.Sync);

            if (GWindowsSplashScreenStatics.Window)
            {
                SetLayeredWindowAttributes(GWindowsSplashScreenStatics.Window, 0, 0x44, LWA_ALPHA);

                ShowWindow(GWindowsSplashScreenStatics.Window, SW_SHOW);
                UpdateWindow(GWindowsSplashScreenStatics.Window);

                while (true)
                {
                    MSG msg{};
                    if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
                    {
                        if (msg.message == WM_QUIT)
                        {
                            break;
                        }
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                }
            }

            UnregisterClassW(MAKEINTATOM(GWindowsSplashScreenStatics.WindowClass), hInstance);

            return 0;
        }
    }
}

namespace Anemone
{
    void WindowsSplashScreen::Show()
    {
        if (Internal::GWindowsSplashScreenStatics.Thread == nullptr)
        {
            Internal::GWindowsSplashScreenStatics.Sync = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            Internal::GWindowsSplashScreenStatics.Thread = CreateThread(
                nullptr,
                0,
                Internal::WindowsSplashScreenThread,
                nullptr,
                0,
                nullptr);
        }
    }

    void WindowsSplashScreen::Hide()
    {
        if (Internal::GWindowsSplashScreenStatics.Thread != nullptr)
        {
            WaitForSingleObject(Internal::GWindowsSplashScreenStatics.Sync, INFINITE);

            if (Internal::GWindowsSplashScreenStatics.Window)
            {
                PostMessageW(Internal::GWindowsSplashScreenStatics.Window, WM_CLOSE, 0, 0);
            }

            WaitForSingleObject(Internal::GWindowsSplashScreenStatics.Thread, INFINITE);

            CloseHandle(Internal::GWindowsSplashScreenStatics.Thread);
            CloseHandle(Internal::GWindowsSplashScreenStatics.Sync);

            Internal::GWindowsSplashScreenStatics.Thread = nullptr;
        }
    }

    bool WindowsSplashScreen::IsVisible()
    {
        return Internal::GWindowsSplashScreenStatics.Thread != nullptr;
    }

    void WindowsSplashScreen::SetText(std::string_view text)
    {
        (void)text;
    }

    void WindowsSplashScreen::SetProgress(float progress)
    {
        (void)progress;
    }
}
