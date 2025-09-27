#include "AnemoneApplication/Platform/Windows/WindowsHostApplication.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"

#include "AnemoneApplication/Platform/Windows/WindowsHostInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsGameInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsXInput.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsHostApplication> gWindowsHostApplication{};
    }

    void HostApplication::Initialize(HostApplicationEvents& events)
    {
        gWindowsHostApplication.Create(events);
        WindowsHostInput::Initialize();
        WindowsGameInput::Initialize();
        WindowsXInput::Initialize();
    }

    void HostApplication::Finalize()
    {
        WindowsXInput::Finalize();
        WindowsGameInput::Finalize();
        WindowsHostInput::Finalize();
        gWindowsHostApplication.Destroy();
    }

    HostApplication& HostApplication::Get()
    {
        return *gWindowsHostApplication;
    }

    WindowsHostApplication::WindowsHostApplication(HostApplicationEvents& e)
        : events{&e}
    {
        using namespace Interop::Windows;

        this->applicationModule = GetModuleHandleW(nullptr);

        this->applicationIcon = LoadIconW(this->applicationModule, MAKEINTRESOURCEW(IDI_MAIN_ICON));

        this->cursorArrow = LoadCursorW(nullptr, IDC_ARROW);
        this->cursorArrowWait = LoadCursorW(nullptr, IDC_APPSTARTING);
        this->cursorText = LoadCursorW(nullptr, IDC_IBEAM);
        this->cursorSizeHorizontal = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeVertical = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeLeft = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeTop = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeRight = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeBottom = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeTopLeft = LoadCursorW(nullptr, IDC_SIZENWSE);
        this->cursorSizeTopRight = LoadCursorW(nullptr, IDC_SIZENESW);
        this->cursorSizeBottomLeft = LoadCursorW(nullptr, IDC_SIZENESW);
        this->cursorSizeBottomRight = LoadCursorW(nullptr, IDC_SIZENWSE);
        this->cursorSizeAll = LoadCursorW(nullptr, IDC_SIZEALL);
        this->cursorCross = LoadCursorW(nullptr, IDC_CROSS);
    }

    WindowsHostApplication::~WindowsHostApplication() = default;

    HCURSOR WindowsHostApplication::GetCursor(CursorType cursor) const
    {
        switch (cursor)
        {
        case CursorType::None:
            return {};

        case CursorType::Arrow:
            return this->cursorArrow;

        case CursorType::ArrowWait:
            return this->cursorArrowWait;

        case CursorType::Text:
            return this->cursorText;

        case CursorType::SizeHorizontal:
            return this->cursorSizeHorizontal;

        case CursorType::SizeVertical:
            return this->cursorSizeVertical;

        case CursorType::SizeLeft:
            return this->cursorSizeLeft;

        case CursorType::SizeTop:
            return this->cursorSizeTop;

        case CursorType::SizeRight:
            return this->cursorSizeRight;

        case CursorType::SizeBottom:
            return this->cursorSizeBottom;

        case CursorType::SizeTopLeft:
            return this->cursorSizeTopLeft;

        case CursorType::SizeTopRight:
            return this->cursorSizeTopRight;

        case CursorType::SizeBottomLeft:
            return this->cursorSizeBottomLeft;

        case CursorType::SizeBottomRight:
            return this->cursorSizeBottomRight;

        case CursorType::SizeAll:
            return this->cursorSizeAll;

        case CursorType::Cross:
            return this->cursorCross;
        }

        return {};
    }

    void WindowsHostApplication::ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        WindowsXInput::Get().Poll(*this->events);
        WindowsGameInput::Get().Poll(*this->events);
    }

    Reference<HostWindow> WindowsHostApplication::MakeWindow(WindowType windowType, WindowMode windowMode)
    {
        return MakeReference<WindowsHostWindow>(*this, windowType, windowMode);
    }
}
