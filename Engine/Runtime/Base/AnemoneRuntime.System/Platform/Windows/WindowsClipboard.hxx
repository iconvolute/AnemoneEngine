#pragma once
#include "AnemoneRuntime.Interop/Windows/Headers.hxx"
#include "AnemoneRuntime.System/Clipboard.hxx"

namespace Anemone
{
    struct WindowsClipboard final
    {
        UINT binaryClipboardFormat = 0;

        static void Initialize();
        static void Finalize();
        static WindowsClipboard& Get();
    };
}
