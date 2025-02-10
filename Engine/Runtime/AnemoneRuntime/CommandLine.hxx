#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>
#include <vector>
#include <utility>
#include <optional>

namespace Anemone
{
    class CommandLine final
    {
    private:
        static int _argc;
        static char** _argv;

    private:
        static std::string_view UnquoteToken(std::string_view s);
        static std::string_view ParseToken(std::string_view& s);

    public:
        RUNTIME_API static void Initialize(
            int argc,
            char* argv[]);

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
