#pragma once
#include "AnemoneRuntime/Platform/Base/BaseClipboard.hxx"

#include <string_view>
#include <string>

namespace Anemone
{
    struct LinuxClipboard final
    {
        static RUNTIME_API void Clear();
        static RUNTIME_API bool GetText(std::string& result);
        static RUNTIME_API bool SetText(std::string_view value);
    };

    using Clipboard = LinuxClipboard;
}
