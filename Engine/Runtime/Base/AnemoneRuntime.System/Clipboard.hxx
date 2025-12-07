#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Diagnostics/Error.hxx"

#include <string_view>
#include <string>
#include <expected>

namespace Anemone
{
    class Clipboard final
    {
    public:
        ANEMONE_RUNTIME_BASE_API static void Initialize();
        ANEMONE_RUNTIME_BASE_API static void Finalize();

    public:
        ANEMONE_RUNTIME_BASE_API static auto Clear() -> std::expected<void, Error>;
        ANEMONE_RUNTIME_BASE_API static auto GetText(std::string& result) -> std::expected<void, Error>;
        ANEMONE_RUNTIME_BASE_API static auto SetText(std::string_view value) -> std::expected<void, Error>;
    };
}
