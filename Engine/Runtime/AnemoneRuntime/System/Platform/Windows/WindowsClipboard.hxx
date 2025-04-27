#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct WindowsClipboardStatics final
    {
        UINT BinaryFormat = 0;

        WindowsClipboardStatics();
    };

    extern UninitializedObject<WindowsClipboardStatics> GClipboardStatics;
}
