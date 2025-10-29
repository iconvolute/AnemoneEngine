#pragma once
#include "AnemoneInterop/Windows/Headers.hxx"
#include "AnemoneSystem/Clipboard.hxx"

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
