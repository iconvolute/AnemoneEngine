#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneDiagnostics/Error.hxx"

#include <string_view>
#include <string>
#include <expected>

namespace Anemone
{
    class Clipboard final
    {
    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();

    public:
        RUNTIME_API static auto Clear() -> std::expected<void, Error>;
        RUNTIME_API static auto GetText(std::string& result) -> std::expected<void, Error>;
        RUNTIME_API static auto SetText(std::string_view value) -> std::expected<void, Error>;
    };
}
