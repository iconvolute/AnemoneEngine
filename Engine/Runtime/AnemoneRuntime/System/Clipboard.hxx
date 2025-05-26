#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <string>

namespace Anemone
{
    class Clipboard final
    {
    public:
        RUNTIME_API static void Clear();
        RUNTIME_API static bool GetText(std::string& result);
        RUNTIME_API static bool SetText(std::string_view value);
    };
}
