#include "AnemoneRuntime/CommandLine.hxx"

#include <algorithm>

namespace Anemone
{
    int CommandLine::_argc = 0;
    char** CommandLine::_argv = nullptr;

    void CommandLine::Initialize(int argc, char* argv[])
    {
        CommandLine::_argc = argc;
        CommandLine::_argv = argv;
    }

    auto CommandLine::GetOption(std::string_view name) -> std::optional<std::string_view>
    {
        std::optional<std::string_view> result{};

        for (int i = 1; i < CommandLine::_argc; ++i)
        {
            std::string_view option{CommandLine::_argv[i]};

            if (option.starts_with('-'))
            {
                // Option '-short' or '--long'
                option.remove_prefix(1);

                if (option.starts_with('-'))
                {
                    // Long option '--option'
                    option.remove_prefix(1);
                }

                if (option.starts_with(name))
                {
                    // Option matched.
                    option.remove_prefix(name.size());

                    if (option.starts_with(':') or option.starts_with('='))
                    {
                        // Matched '--option=value' or '--option:value'.
                        option.remove_prefix(1);
                        result = option;
                    }
                    else
                    {
                        // Override result.
                        result = std::string_view{};
                    }
                }
            }
        }

        return result;
    }

    void CommandLine::GetPositional(std::vector<std::string_view>& positional)
    {
        for (int i = 1; i < CommandLine::_argc; ++i)
        {
            std::string_view option{CommandLine::_argv[i]};

            if (option.starts_with('-'))
            {
                // Not a positional argument.
                continue;
            }

            positional.emplace_back(option);
        }
    }
}

namespace Anemone
{
    // Specification:
    //
    // Named parameters are using '-' or '--' as prefix. Value separator is '=' or ':'.
    // Positional parameters are not prefixed.
    // If command line argument is not a named parameter, it is considered a positional parameter.
    //
    // Example:
    // `positional1 --name1=value1 positional2 --name2=value2 -name3:value3 positional3 positional4'

    void CommandlineParser::Parse(int argc, const char* const* argv)
    {
        for (int i = 0; i < argc; ++i)
        {
            this->HandleToken(argv[i]);
        }
    }

    void CommandlineParser::Parse(std::string_view commandLine)
    {
        while (not commandLine.empty())
        {
            if (auto token = ParseToken(commandLine); not token.empty())
            {
                this->HandleToken(token);
            }
        }
    }

    auto CommandlineParser::GetPositional() const -> std::span<std::string_view const>
    {
        return this->m_Positional;
    }

    auto CommandlineParser::GetValue(std::string_view name) const -> std::optional<std::string_view>
    {
        auto it = std::find_if(this->m_Named.begin(), this->m_Named.end(), [&](auto const& nameValue)
        {
            return nameValue.first == name;
        });

        if (it != this->m_Named.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

    auto CommandlineParser::GetValues(std::string_view name) const -> std::vector<std::string_view>
    {
        std::vector<std::string_view> values;

        for (auto const& [key, value] : this->m_Named)
        {
            if (key == name)
            {
                values.emplace_back(value);
            }
        }

        return values;
    }

    auto CommandlineParser::GetNamed() const -> std::span<std::pair<std::string_view, std::string_view> const>
    {
        return this->m_Named;
    }

    auto CommandlineParser::ParseToken(std::string_view& commandLine) -> std::string_view
    {
        // Skip whitespaces
        if (auto const start = commandLine.find_first_not_of(" \t"); start != std::string_view::npos)
        {
            // Remove whitespaces.
            commandLine.remove_prefix(start);
        }

        if (commandLine.empty())
        {
            // No more tokens.
            return {};
        }

        if (commandLine[0] != '"')
            [[likely]]
        {
            // Normal token. Find next whitespace and return token.
            auto const end = commandLine.find_first_of(" \t");

            if (end == std::string_view::npos)
            {
                // No more tokens.
                return std::exchange(commandLine, {});
            }

            // Found whitespace.
            auto token = commandLine.substr(0, end);

            // Remove token and whitespace.
            commandLine.remove_prefix(end);
            return token;
        }

        // Quoted string. Find closing quote or return rest of the commandLine as token.
        commandLine.remove_prefix(1);
        auto const end = commandLine.find('"');

        if (end == std::string_view::npos)
        {
            // No closing quote. Treat rest of the string just as plain token.
            return std::exchange(commandLine, {});
        }

        // Found closing quote.
        auto token = commandLine.substr(0, end);

        // Remove token and closing quote.
        commandLine.remove_prefix(end + 1);
        return token;
    }

    void CommandlineParser::HandleToken(std::string_view token)
    {
        if (token.starts_with('-'))
        {
            token.remove_prefix(1);

            if (token.starts_with('-'))
            {
                token.remove_prefix(1);
            }

            // Named parameter prefixed with '-' or '--'.
            auto separator = token.find_first_of("=:");

            if (separator != std::string_view::npos)
            {
                this->m_Named.emplace_back(token.substr(0, separator), token.substr(separator + 1));
            }
            else
            {
                this->m_Named.emplace_back(token, "");
            }
        }
        else
        {
            // Positional parameter.
            this->m_Positional.emplace_back(token);
        }
    }
}
