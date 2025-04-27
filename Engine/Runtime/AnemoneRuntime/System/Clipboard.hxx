#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <string>

namespace Anemone
{
    struct Clipboard final
    {
        Clipboard() = delete;

        RUNTIME_API static void Clear();
        RUNTIME_API static bool GetText(std::string& result);
        RUNTIME_API static bool SetText(std::string_view value);
    };
}
