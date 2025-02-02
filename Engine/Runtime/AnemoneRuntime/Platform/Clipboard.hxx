#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>
#include <string>

namespace Anemone
{
    struct Clipboard final
    {
        static void Clear();
        static bool GetText(std::string& result);
        static bool SetText(std::string_view value);
    };
}
