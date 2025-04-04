#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone
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

        WindowsInput Input{};

        static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;

        WindowsApplicationStatics();

        ~WindowsApplicationStatics();

        static WindowsApplicationStatics& Get();
    };
}
