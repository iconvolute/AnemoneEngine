#include "AnemoneRuntime/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsPlatform.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    UninitializedObject<WindowsApplicationStatics> GWindowsApplicationStatics;

    static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;
}

namespace Anemone
{
    void WindowsApplication::Initialize()
    {
        Internal::GWindowsApplicationStatics.Create();
        Internal::GWindowsInputStatics.Create();


        Internal::GWindowsApplicationStatics->ApplicationIconHandle = static_cast<HICON>(LoadImageW(
            Internal::GWindowsPlatformStatics->InstanceHandle,
            MAKEINTRESOURCE(Internal::IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        Internal::GWindowsApplicationStatics->ArrowCursor = Interop::win32_LoadSystemCursor(IDC_ARROW);
        Internal::GWindowsApplicationStatics->ArrowWaitCursor = Interop::win32_LoadSystemCursor(IDC_APPSTARTING);
        Internal::GWindowsApplicationStatics->TextCursor = Interop::win32_LoadSystemCursor(IDC_IBEAM);

        Internal::GWindowsApplicationStatics->SizeHorizontalCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        Internal::GWindowsApplicationStatics->SizeVerticalCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        Internal::GWindowsApplicationStatics->SizeLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        Internal::GWindowsApplicationStatics->SizeTopCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        Internal::GWindowsApplicationStatics->SizeRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        Internal::GWindowsApplicationStatics->SizeBottomCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        Internal::GWindowsApplicationStatics->SizeTopLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZENWSE);
        Internal::GWindowsApplicationStatics->SizeTopRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZENESW);
        Internal::GWindowsApplicationStatics->SizeBottomLeftCursor = Interop::win32_LoadSystemCursor(IDC_SIZENESW);
        Internal::GWindowsApplicationStatics->SizeBottomRightCursor = Interop::win32_LoadSystemCursor(IDC_SIZENWSE);
        Internal::GWindowsApplicationStatics->SizeAllCursor = Interop::win32_LoadSystemCursor(IDC_SIZEALL);
        Internal::GWindowsApplicationStatics->CrossCursor = Interop::win32_LoadSystemCursor(IDC_CROSS);

        // Register window class
        WNDCLASSEXW wc{
            .cbSize = sizeof(wc),
            .style = CS_DBLCLKS,
            .lpfnWndProc = WindowsWindow::WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = Internal::GWindowsPlatformStatics->InstanceHandle,
            .hIcon = Internal::GWindowsApplicationStatics->ApplicationIconHandle,
            .hCursor = Internal::GWindowsApplicationStatics->ArrowCursor,
            .hbrBackground = GetSysColorBrush(COLOR_WINDOW),
            .lpszMenuName = nullptr,
            .lpszClassName = L"AnemoneWindow",
            .hIconSm = Internal::GWindowsApplicationStatics->ApplicationIconHandle,
        };

        Internal::GWindowsApplicationStatics->MainWindowClass = RegisterClassExW(&wc);

        if (Internal::GWindowsApplicationStatics->MainWindowClass == 0)
        {
            Debugger::ReportApplicationStop("Failed to register window class.");
        }
    }

    void WindowsApplication::Finalize()
    {
        UnregisterClassW(
            MAKEINTATOM(Internal::GWindowsApplicationStatics->MainWindowClass),
            Internal::GWindowsPlatformStatics->InstanceHandle);

        Internal::GWindowsInputStatics.Destroy();
        Internal::GWindowsApplicationStatics.Destroy();
    }

    void WindowsApplication::ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        Internal::GWindowsInputStatics->Poll();
    }

    void WindowsApplication::SetEvents(IApplicationEvents* events)
    {
        Internal::GWindowsApplicationStatics->Events = events;
    }

    std::unique_ptr<Window> WindowsApplication::MakeWindow(WindowType type)
    {
        return std::make_unique<WindowsWindow>(type);
    }
}
