#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/System/Application.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/System/Platform/Windows/Input.hxx"
#include "AnemoneRuntime/System/Platform/Windows/XInput.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"

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

        [[nodiscard]] HCURSOR GetCursor(CursorType cursor) const;
    };

    extern HCURSOR GetCursorHandle(CursorType cursor);

    extern UninitializedObject<ApplicationStatics> GApplicationStatics;
}
