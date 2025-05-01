#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal::Windows
{
    HINSTANCE GInstanceHandle;

    UninitializedObject<ApplicationStatics> GApplicationStatics{};

    static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;
}

namespace Anemone::Internal::Windows
{
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

    HCURSOR GetCursorHandle(CursorType cursor)
    {
        return GApplicationStatics->GetCursor(cursor);
    }
}

namespace Anemone::Internal
{
    extern void InitializeApplicationResources()
    {
        Windows::GApplicationStatics.Create();

        HINSTANCE const hInstance = GetModuleHandleW(nullptr);
        Windows::GInstanceHandle = hInstance;

        Windows::GApplicationStatics->ApplicationIconHandle = static_cast<HICON>(LoadImageW(
            hInstance,
            MAKEINTRESOURCE(Windows::IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        Windows::GApplicationStatics->ArrowCursor = Interop::Windows::LoadSystemCursor(IDC_ARROW);
        Windows::GApplicationStatics->ArrowWaitCursor = Interop::Windows::LoadSystemCursor(IDC_APPSTARTING);
        Windows::GApplicationStatics->TextCursor = Interop::Windows::LoadSystemCursor(IDC_IBEAM);

        Windows::GApplicationStatics->SizeHorizontalCursor = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        Windows::GApplicationStatics->SizeVerticalCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        Windows::GApplicationStatics->SizeLeftCursor = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        Windows::GApplicationStatics->SizeTopCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        Windows::GApplicationStatics->SizeRightCursor = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        Windows::GApplicationStatics->SizeBottomCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        Windows::GApplicationStatics->SizeTopLeftCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENWSE);
        Windows::GApplicationStatics->SizeTopRightCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENESW);
        Windows::GApplicationStatics->SizeBottomLeftCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENESW);
        Windows::GApplicationStatics->SizeBottomRightCursor = Interop::Windows::LoadSystemCursor(IDC_SIZENWSE);
        Windows::GApplicationStatics->SizeAllCursor = Interop::Windows::LoadSystemCursor(IDC_SIZEALL);
        Windows::GApplicationStatics->CrossCursor = Interop::Windows::LoadSystemCursor(IDC_CROSS);
    }

    extern void FinalizeApplicationResources()
    {
        Windows::GApplicationStatics.Destroy();
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
        Internal::Windows::GApplicationStatics->XInput.Poll();
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        return std::make_unique<Internal::Windows::WindowImpl>(type, WindowMode::Windowed);
    }
}
