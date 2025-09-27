#pragma once
#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"

#include "AnemoneRuntime/Base/Intrusive.hxx"

namespace Anemone
{
    class WindowsHostWindow;

    class WindowsHostApplication final
        : public HostApplication
    {
    public:
        WindowsHostApplication(HostApplicationEvents& e);

        WindowsHostApplication(WindowsHostApplication const&) = delete;

        WindowsHostApplication(WindowsHostApplication&&) = delete;

        virtual ~WindowsHostApplication();

        WindowsHostApplication& operator=(WindowsHostApplication const&) = delete;

        WindowsHostApplication& operator=(WindowsHostApplication&&) = delete;

    public:
        static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;

        IntrusiveList<WindowsHostWindow> windows{};
        HostApplicationEvents* events{};

        HINSTANCE applicationModule{};
        HICON applicationIcon{};

        HCURSOR cursorArrow{};
        HCURSOR cursorArrowWait{};
        HCURSOR cursorText{};
        HCURSOR cursorSizeHorizontal{};
        HCURSOR cursorSizeVertical{};
        HCURSOR cursorSizeLeft{};
        HCURSOR cursorSizeTop{};
        HCURSOR cursorSizeRight{};
        HCURSOR cursorSizeBottom{};
        HCURSOR cursorSizeTopLeft{};
        HCURSOR cursorSizeTopRight{};
        HCURSOR cursorSizeBottomLeft{};
        HCURSOR cursorSizeBottomRight{};
        HCURSOR cursorSizeAll{};
        HCURSOR cursorCross{};

        [[nodiscard]] HCURSOR GetCursor(CursorType cursor) const;

        void ProcessMessages() override;

        Reference<HostWindow> MakeWindow(WindowType windowType, WindowMode windowMode) override;
    };
}
