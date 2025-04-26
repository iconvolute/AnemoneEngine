#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/System/Application.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/System/Platform/Windows/Input.hxx"
#include "AnemoneRuntime/System/Platform/Windows/XInput.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Internal::Windows
{
    extern HINSTANCE GInstanceHandle;

    struct ApplicationStatics final
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

        IntrusiveList<WindowImpl> WindowsCollection{};

        InputImpl Input{};
        XInputImpl XInput{};

        static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;

        ApplicationStatics();

        [[nodiscard]] HCURSOR GetCursor(CursorType cursor) const;
    };
}

namespace Anemone::Internal
{
    extern UninitializedObject<Windows::ApplicationStatics> GApplicationStatics;
}
