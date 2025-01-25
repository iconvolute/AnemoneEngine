#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseClipboard.hxx"

#include <string_view>
#include <string>

namespace Anemone
{
    struct WindowsClipboard final
    {
        static void Clear();
        static bool GetText(std::string& result);
        static bool SetText(std::string_view value);
    };

    using Clipboard = WindowsClipboard;
}
