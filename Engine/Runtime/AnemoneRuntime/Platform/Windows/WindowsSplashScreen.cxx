#include "AnemoneRuntime/Platform/Windows/WindowsSplashScreen.hxx"

namespace Anemone::Private
{
    namespace
    {
        struct WindowsSplashScreen final
        {
            HANDLE Thread{};
            ATOM WindowClass{};
            HWND Window{};
            HANDLE Sync{};
        };

        WindowsSplashScreen GSplashScreen{};

        LRESULT CALLBACK SplashScreenWndProc(
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

        DWORD WINAPI SplashScreenThread(LPVOID)
        {
            HMODULE hInstance = GetModuleHandleW(nullptr);

            WNDCLASSEXW wcex{};
            wcex.cbSize = sizeof(wcex);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = SplashScreenWndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;            
            wcex.hInstance = hInstance;
            wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
            wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
            wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            wcex.lpszMenuName = nullptr;
            wcex.lpszClassName = L"AnemoneSplashScreen";
            wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

            GSplashScreen.WindowClass = RegisterClassExW(&wcex);

            if (!GSplashScreen.WindowClass)
            {
                return 0;
            }

            int const width = 640;
            int const height = 480;

            int const centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
            int const centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

            int const positionX = centerX - width / 2;
            int const positionY = centerY - height / 2;

            GSplashScreen.Window = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                MAKEINTATOM(GSplashScreen.WindowClass),
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

            SetEvent(GSplashScreen.Sync);

            if (GSplashScreen.Window)
            {
                SetLayeredWindowAttributes(GSplashScreen.Window, 0, 0x44, LWA_ALPHA);

                ShowWindow(GSplashScreen.Window, SW_SHOW);
                UpdateWindow(GSplashScreen.Window);

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

            UnregisterClassW(MAKEINTATOM(GSplashScreen.WindowClass), hInstance);

            return 0;
        }
    }
}

namespace Anemone
{
    void SplashScreen::Show()
    {
        if (Private::GSplashScreen.Thread == nullptr)
        {
            Private::GSplashScreen.Sync = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            Private::GSplashScreen.Thread = CreateThread(
                nullptr,
                0,
                Private::SplashScreenThread,
                nullptr,
                0,
                nullptr);
        }
    }

    void SplashScreen::Hide()
    {
        if (Private::GSplashScreen.Thread != nullptr)
        {
            WaitForSingleObject(Private::GSplashScreen.Sync, INFINITE);

            if (Private::GSplashScreen.Window)
            {
                PostMessageW(Private::GSplashScreen.Window, WM_CLOSE, 0, 0);
            }

            WaitForSingleObject(Private::GSplashScreen.Thread, INFINITE);

            CloseHandle(Private::GSplashScreen.Thread);
            CloseHandle(Private::GSplashScreen.Sync);

            Private::GSplashScreen.Thread = nullptr;
        }
    }

    bool SplashScreen::IsVisible()
    {
        return Private::GSplashScreen.Thread != nullptr;
    }

    void SplashScreen::SetText(std::string_view text)
    {
        (void)text;
    }

    void SplashScreen::SetProgress(float progress)
    {
        (void)progress;
    }
}
