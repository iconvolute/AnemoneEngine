#pragma once
#include "AnemoneInterop/Headers.hxx"

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
        RUNTIME_API static void StaticInitialize(int argc, char** argv);
        RUNTIME_API static void StaticFinalize();

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

        RUNTIME_API static void Append(
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
