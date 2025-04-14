#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>
#include <vector>
#include <utility>
#include <optional>

namespace Anemone
{
    struct CommandLine final
    {
        CommandLine() = delete;

        RUNTIME_API static std::optional<std::string_view> GetOption(
            std::string_view name);

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
    };
}
