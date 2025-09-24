#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"

namespace Anemone
{
    struct WindowsClipboard final
    {
        UINT binaryClipboardFormat = 0;
    };
}
