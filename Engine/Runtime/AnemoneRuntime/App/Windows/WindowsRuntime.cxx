#include "AnemoneRuntime/App/Windows/WindowsRuntime.hxx"
#include "AnemoneRuntime/App/Windows/WindowsWindow.hxx"

namespace Anemone::App
{
    UninitializedObject<WindowsAppState> GWindowsAppState;
    UninitializedObject<WindowsInput> GWindowsInput;

    static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;

    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        GWindowsAppState.Create();
        GWindowsInput.Create();

        // Grab handle to module with current process.
        GWindowsAppState->Instance = GetModuleHandleW(nullptr);

        // Load main icon resource.
        GWindowsAppState->Icon = static_cast<HICON>(LoadImageW(
            GWindowsAppState->Instance,
            MAKEINTRESOURCEW(IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        HCURSOR const hArrowCursor = Platform::win32_LoadSystemCursor(IDC_ARROW);
        HCURSOR const hIBeamCursor = Platform::win32_LoadSystemCursor(IDC_IBEAM);
        HCURSOR const hWaitCursor = Platform::win32_LoadSystemCursor(IDC_WAIT);
        HCURSOR const hCrossCursor = Platform::win32_LoadSystemCursor(IDC_CROSS);
        HCURSOR const hSizeNWSECursor = Platform::win32_LoadSystemCursor(IDC_SIZENWSE);
        HCURSOR const hSizeNESWCursor = Platform::win32_LoadSystemCursor(IDC_SIZENESW);
        HCURSOR const hSizeWECursor = Platform::win32_LoadSystemCursor(IDC_SIZEWE);
        HCURSOR const hSizeNSCursor = Platform::win32_LoadSystemCursor(IDC_SIZENS);
        HCURSOR const hSizeAllCursor = Platform::win32_LoadSystemCursor(IDC_SIZEALL);


        WNDCLASSEXW wcex{
            .cbSize = sizeof(wcex),
            .style = CS_DBLCLKS,
            .lpfnWndProc = WindowsWindow::WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GWindowsAppState->Instance,
            .hIcon = GWindowsAppState->Icon,
            .hCursor = hArrowCursor,
            .hbrBackground = GetSysColorBrush(COLOR_WINDOW),
            .lpszMenuName = nullptr,
            .lpszClassName = L"AnemoneWindow",
            .hIconSm = GWindowsAppState->Icon,
        };

        GWindowsAppState->WindowClass = RegisterClassExW(&wcex);

        AE_ENSURE(GWindowsAppState->WindowClass != 0);

        GWindowsAppState->InitializeCursor(CursorType::None, nullptr);
        GWindowsAppState->InitializeCursor(CursorType::Arrow, hArrowCursor);
        GWindowsAppState->InitializeCursor(CursorType::IBeam, hIBeamCursor);
        GWindowsAppState->InitializeCursor(CursorType::Wait, hWaitCursor);
        GWindowsAppState->InitializeCursor(CursorType::Cross, hCrossCursor);
        GWindowsAppState->InitializeCursor(CursorType::SizeNWSE, hSizeNWSECursor);
        GWindowsAppState->InitializeCursor(CursorType::SizeNESW, hSizeNESWCursor);
        GWindowsAppState->InitializeCursor(CursorType::SizeWE, hSizeWECursor);
        GWindowsAppState->InitializeCursor(CursorType::SizeNS, hSizeNSCursor);
        GWindowsAppState->InitializeCursor(CursorType::SizeAll, hSizeAllCursor);
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

        AE_ENSURE(GWindowsAppState->WindowClass != 0);
        UnregisterClassW(
            MAKEINTATOM(GWindowsAppState->WindowClass),
            GWindowsAppState->Instance);

        GWindowsInput.Destroy();
        GWindowsAppState.Destroy();
    }
}
