#include "AnemoneRuntime/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone
{
    static UninitializedObject<WindowsApplicationStatics> GWindowsApplicationStatics;

    WindowsApplicationStatics::WindowsApplicationStatics()
    {
        HINSTANCE const hInstance = GetModuleHandleW(nullptr);

        this->ApplicationIconHandle = static_cast<HICON>(LoadImageW(
            hInstance,
            MAKEINTRESOURCE(WindowsApplicationStatics::IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        this->ArrowCursor = Interop::win32_LoadSystemCursor(IDC_ARROW);
        this->ArrowWaitCursor = Interop::win32_LoadSystemCursor(IDC_APPSTARTING);
        this->TextCursor = Interop::win32_LoadSystemCursor(IDC_IBEAM);

        this->SizeHorizontalCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        this->SizeVerticalCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        this->SizeLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        this->SizeTopCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        this->SizeRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        this->SizeBottomCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        this->SizeTopLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZENWSE);
        this->SizeTopRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZENESW);
        this->SizeBottomLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZENESW);
        this->SizeBottomRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZENWSE);
        this->SizeAllCursor = Interop::win32_LoadSystemCursor(IDC_SIZEALL);
        this->CrossCursor = Interop::win32_LoadSystemCursor(IDC_CROSS);

        // Register window class
        WNDCLASSEXW wc{
            .cbSize = sizeof(wc),
            .style = CS_DBLCLKS,
            .lpfnWndProc = WindowsWindow::WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = hInstance,
            .hIcon = this->ApplicationIconHandle,
            .hCursor = this->ArrowCursor,
            .hbrBackground = GetSysColorBrush(COLOR_WINDOW),
            .lpszMenuName = nullptr,
            .lpszClassName = L"AnemoneWindow",
            .hIconSm = this->ApplicationIconHandle,
        };

        this->MainWindowClass = RegisterClassExW(&wc);

        if (this->MainWindowClass == 0)
        {
            Debugger::ReportApplicationStop("Failed to register window class.");
        }
    }

    WindowsApplicationStatics::~WindowsApplicationStatics()
    {
        HINSTANCE const hInstance = GetModuleHandleW(nullptr);

        UnregisterClassW(
            MAKEINTATOM(this->MainWindowClass),
            hInstance);
    }

    WindowsApplicationStatics& WindowsApplicationStatics::Get()
    {
        return GWindowsApplicationStatics.Get();
    }
}

namespace Anemone
{
    void Application::Initialize()
    {
        GWindowsApplicationStatics.Create();
    }

    void Application::Finalize()
    {
        GWindowsApplicationStatics.Destroy();
    }

    void Application::ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        GWindowsApplicationStatics->Input.Poll();
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        return std::make_unique<WindowsWindow>(type);
    }
}
