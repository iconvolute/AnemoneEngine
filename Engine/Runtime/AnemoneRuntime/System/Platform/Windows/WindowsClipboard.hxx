#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    struct WindowsClipboardStatics final
    {
        UINT BinaryFormat = 0;

        WindowsClipboardStatics();
    };

    extern UninitializedObject<WindowsClipboardStatics> GClipboardStatics;
}
