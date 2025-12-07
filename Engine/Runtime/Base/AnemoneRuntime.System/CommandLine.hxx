#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

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
        ANEMONE_RUNTIME_BASE_API static void StaticInitialize(int argc, char** argv);
        ANEMONE_RUNTIME_BASE_API static void StaticFinalize();

        ANEMONE_RUNTIME_BASE_API static auto GetOption(
            std::string_view name) -> std::optional<std::string_view>;

        ANEMONE_RUNTIME_BASE_API static void GetOption(
            std::string_view name,
            std::vector<std::string_view>& values);

        ANEMONE_RUNTIME_BASE_API static void GetPositional(
            std::vector<std::string_view>& positional);

        ANEMONE_RUNTIME_BASE_API static void Parse(
            std::string_view commandLine,
            std::vector<std::string_view>& positional,
            std::vector<std::string_view>& switches,
            std::vector<std::pair<std::string_view, std::string_view>>& options);

        ANEMONE_RUNTIME_BASE_API static void Build(
            const char* const* argv,
            std::string& result);

        ANEMONE_RUNTIME_BASE_API static void Append(
            std::string& result,
            std::string_view part);

        template <typename Container>
        static std::string Combine(
            std::string_view app,
            Container const& container);
    };
}

namespace Anemone
{
    template <typename Container>
    std::string CommandLine::Combine(
        std::string_view app,
        Container const& container)
    {
        std::string result{app};

        for (std::string_view arg : container)
        {
            result.push_back(' ');
            CommandLine::Append(result, arg);
        }

        return result;
    }
}
