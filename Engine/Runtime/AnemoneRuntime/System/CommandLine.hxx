#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <vector>
#include <utility>
#include <optional>
#include <string>

namespace Anemone
{
    class CommandLine final
    {
    public:
        RUNTIME_API static auto GetOption(
            std::string_view name) -> std::optional<std::string_view>;

        RUNTIME_API static void GetOption(
            std::string_view name,
            std::vector<std::string_view>& values);

        RUNTIME_API static void GetPositional(
            std::vector<std::string_view>& positional);

        RUNTIME_API static void Parse(
            std::string_view commandLine,
            std::vector<std::string_view>& positional,
            std::vector<std::string_view>& switches,
            std::vector<std::pair<std::string_view, std::string_view>>& options);

        RUNTIME_API static void Build(
            const char* const* argv,
            std::string& result);
    };
}
