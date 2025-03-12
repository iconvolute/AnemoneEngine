#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Internal
{
    struct WindowsApplicationStatics final
    {
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
