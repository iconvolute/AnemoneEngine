#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseApplication.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Internal
{
    struct WindowsApplicationStatics final
    {
        IApplicationEvents* Events = nullptr;

        HICON ApplicationIconHandle{};

        HCURSOR ArrowCursor{};
        HCURSOR ArrowWaitCursor{};
        HCURSOR TextCursor{};
        HCURSOR SizeHorizontalCursor{};
        HCURSOR SizeVerticalCursor{};
        HCURSOR SizeLeftCursor{};
        HCURSOR SizeTopCursor{};
        HCURSOR SizeRightCursor{};
        HCURSOR SizeBottomCursor{};
        HCURSOR SizeTopLeftCursor{};
        HCURSOR SizeTopRightCursor{};
        HCURSOR SizeBottomLeftCursor{};
        HCURSOR SizeBottomRightCursor{};
        HCURSOR SizeAllCursor{};
        HCURSOR CrossCursor{};

        ATOM MainWindowClass{};

        IntrusiveList<Window> WindowsCollection{};
    };

    extern UninitializedObject<WindowsApplicationStatics> GWindowsApplicationStatics;
}

namespace Anemone
{
    struct WindowsApplication final
    {
        static RUNTIME_API void Initialize();
        static RUNTIME_API void Finalize();
        static RUNTIME_API void ProcessMessages();
        static RUNTIME_API void SetEvents(IApplicationEvents* events);

        static RUNTIME_API std::unique_ptr<Window> MakeWindow(WindowType type);
    };

    using Application = WindowsApplication;
}
