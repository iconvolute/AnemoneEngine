#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    UninitializedObject<Windows::ApplicationStatics> GApplicationStatics;
}

namespace Anemone::Internal::Windows
{
    HINSTANCE GInstanceHandle;
    
    ApplicationStatics::ApplicationStatics()
    {
        HINSTANCE const hInstance = GetModuleHandleW(nullptr);
        GInstanceHandle = hInstance;

        this->ApplicationIconHandle = static_cast<HICON>(LoadImageW(
            hInstance,
            MAKEINTRESOURCE(ApplicationStatics::IDI_MAIN_ICON),
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
    }

    HCURSOR ApplicationStatics::GetCursor(CursorType cursor) const
    {
        switch (cursor)
        {
        default:
        case CursorType::None:
            return {};

        case CursorType::Arrow:
            return this->ArrowCursor;

        case CursorType::ArrowWait:
            return this->ArrowWaitCursor;

        case CursorType::Text:
            return this->TextCursor;

        case CursorType::SizeHorizontal:
            return this->SizeHorizontalCursor;

        case CursorType::SizeVertical:
            return this->SizeVerticalCursor;

        case CursorType::SizeLeft:
            return this->SizeLeftCursor;

        case CursorType::SizeTop:
            return this->SizeTopCursor;

        case CursorType::SizeRight:
            return this->SizeRightCursor;

        case CursorType::SizeBottom:
            return this->SizeBottomCursor;

        case CursorType::SizeTopLeft:
            return this->SizeTopLeftCursor;

        case CursorType::SizeTopRight:
            return this->SizeTopRightCursor;

        case CursorType::SizeBottomLeft:
            return this->SizeBottomLeftCursor;

        case CursorType::SizeBottomRight:
            return this->SizeBottomRightCursor;

        case CursorType::SizeAll:
            return this->SizeAllCursor;

        case CursorType::Cross:
            return this->CrossCursor;
        }
    }
}

namespace Anemone
{
    void Application::ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        Internal::GApplicationStatics->XInput.Poll();
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        return std::make_unique<Internal::Windows::WindowImpl>(type, WindowMode::Windowed);
    }
}
