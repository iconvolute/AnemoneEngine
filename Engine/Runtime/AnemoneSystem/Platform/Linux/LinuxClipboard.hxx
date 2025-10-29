#pragma once
#include "AnemoneSystem/Clipboard.hxx"

namespace Anemone
{
    struct LinuxClipboard final
    {
        static void Initialize();
        static void Finalize();
        static LinuxClipboard& Get();
    };
}
