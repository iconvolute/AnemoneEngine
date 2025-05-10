#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <string>

namespace Anemone::Clipboard
{
    RUNTIME_API void Clear();
    RUNTIME_API bool GetText(std::string& result);
    RUNTIME_API bool SetText(std::string_view value);
}
