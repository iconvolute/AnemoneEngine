#include "AnemoneRuntime/App/Platform/Windows/Application.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Input/Platform/Windows/GameInput.hxx"
#include "AnemoneRuntime/Input/Platform/Windows/XInput.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<Application> GApplication{};

        constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;
    }

    void IApplication::Initialize(IApplicationEvents* events)
    {
        GApplication.Create(events);
    }

    void IApplication::Finalize()
    {
        GApplication.Destroy();
    }

    IApplication& IApplication::Get()
    {
        return *GApplication;
    }
}

namespace Anemone
{
    Application::Application(IApplicationEvents* events)
        : _events{events}
    {
        HINSTANCE const hInstance = GetModuleHandleW(nullptr);

        this->_appIcon = static_cast<HICON>(LoadImageW(
            hInstance,
            MAKEINTRESOURCE(IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        this->_cursorArrow = Interop::Windows::LoadSystemCursor(IDC_ARROW);
        this->_cursorArrowWait = Interop::Windows::LoadSystemCursor(IDC_APPSTARTING);
        this->_cursorText = Interop::Windows::LoadSystemCursor(IDC_IBEAM);
        this->_cursorSizeHorizontal = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        this->_cursorSizeVertical = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        this->_cursorSizeLeft = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        this->_cursorSizeTop = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        this->_cursorSizeRight = Interop::Windows::LoadSystemCursor(IDC_SIZEWE);
        this->_cursorSizeBottom = Interop::Windows::LoadSystemCursor(IDC_SIZENS);
        this->_cursorSizeTopLeft = Interop::Windows::LoadSystemCursor(IDC_SIZENWSE);
        this->_cursorSizeTopRight = Interop::Windows::LoadSystemCursor(IDC_SIZENESW);
        this->_cursorSizeBottomLeft = Interop::Windows::LoadSystemCursor(IDC_SIZENESW);
        this->_cursorSizeBottomRight = Interop::Windows::LoadSystemCursor(IDC_SIZENWSE);
        this->_cursorSizeAll = Interop::Windows::LoadSystemCursor(IDC_SIZEALL);
        this->_cursorCross = Interop::Windows::LoadSystemCursor(IDC_CROSS);
    }

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
        XInput::Get().Poll(*this->_events);
        GameInput::Get().Poll(*this->_events);
    }

    std::unique_ptr<IWindow> Application::CreateWindow(WindowType type, WindowMode mode)
    {
        return std::make_unique<Window>(this, type, mode);
    }

    HCURSOR Application::GetCursor(CursorType cursor) const
    {
        switch (cursor)
        {
        case CursorType::None:
            return {};

        case CursorType::Arrow:
            return this->_cursorArrow;

        case CursorType::ArrowWait:
            return this->_cursorArrowWait;

        case CursorType::Text:
            return this->_cursorText;

        case CursorType::SizeHorizontal:
            return this->_cursorSizeHorizontal;

        case CursorType::SizeVertical:
            return this->_cursorSizeVertical;

        case CursorType::SizeLeft:
            return this->_cursorSizeLeft;

        case CursorType::SizeTop:
            return this->_cursorSizeTop;

        case CursorType::SizeRight:
            return this->_cursorSizeRight;

        case CursorType::SizeBottom:
            return this->_cursorSizeBottom;

        case CursorType::SizeTopLeft:
            return this->_cursorSizeTopLeft;

        case CursorType::SizeTopRight:
            return this->_cursorSizeTopRight;

        case CursorType::SizeBottomLeft:
            return this->_cursorSizeBottomLeft;

        case CursorType::SizeBottomRight:
            return this->_cursorSizeBottomRight;

        case CursorType::SizeAll:
            return this->_cursorSizeAll;

        case CursorType::Cross:
            return this->_cursorCross;
        }

        return {};
    }
}
