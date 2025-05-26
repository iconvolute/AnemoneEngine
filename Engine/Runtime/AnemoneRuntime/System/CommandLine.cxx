#include "AnemoneRuntime/System/CommandLine.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

#include <algorithm>

namespace Anemone::Internal
{
    extern int GCommandLineArgC;
    extern char** GCommandLineArgV;

    int GCommandLineArgC = 0;
    char** GCommandLineArgV = nullptr;
}

namespace Anemone
{
    namespace
    {
        constexpr std::optional<std::string_view> ExtractTokenValue(std::string_view token, std::string_view name)
        {
            std::optional<std::string_view> result{};

            if (token.starts_with('-'))
            {
                // Option '-short' or '--long'
                token.remove_prefix(1);

                if (token.starts_with('-'))
                {
                    // Long option '--option'
                    token.remove_prefix(1);
                }

                if (token.starts_with(name))
                {
                    // Option matched.
                    token.remove_prefix(name.size());

                    if (token.starts_with(':') or token.starts_with('='))
                    {
                        // Matched '--option=value' or '--option:value'.
                        token.remove_prefix(1);
                        result = token;
                    }
                    else
                    {
                        // Override result.
                        result = std::string_view{};
                    }
                }
            }

            return result;
        }

        constexpr std::string_view UnquoteToken(std::string_view s)
        {
            if (s.starts_with('\"'))
            {
                // Starts with quote.
                s.remove_prefix(1);

                if (s.size() > 1)
                {
                    std::string_view::size_type end = s.size() - 1;

                    if ((s[end - 1] != '\\') and (s[end] == '"'))
                    {
                        // Ends with non-escaped quote.
                        s.remove_suffix(1);
                    }
                }
            }

            return s;
        }

        constexpr std::string_view ParseToken(std::string_view& s)
        {
            // Skip past whitespaces
            if (auto const start = s.find_first_not_of(" \t"); start != std::string_view::npos)
            {
                s.remove_prefix(start);
            }

            if (s.empty())
            {
                return {};
            }

            bool quoted = false;
            bool escaped = false;

            auto it = s.begin();
            auto const end = s.end();

            while (it != end)
            {
                char const ch = *it;

                if (quoted)
                {
                    // Inside quoted token. Check for escapes.
                    if (ch == '\\')
                    {
                        escaped = true;
                    }
                    else if (ch == '\"')
                    {
                        // Found quote character.
                        if (escaped)
                        {
                            // Escaped quote. Continue parsing.
                            escaped = false;
                        }
                        else
                        {
                            // Found closing quote. Break parsing.
                            // This means that we treat `--name="value"` as a single token.
                            // This also means that `--name="value""quoted"` is broken into two separate tokens.
                            ++it;
                            break;
                        }
                    }
                }
                else
                {
                    // Normal characters.
                    if (ch == ' ' or ch == '\t')
                    {
                        // Found whitespace. Break parsing.
                        break;
                    }

                    if (ch == '\"')
                    {
                        // Found opening quote.
                        quoted = true;
                    }
                }

                ++it;
            }

            std::string_view result{s.begin(), it};
            s.remove_prefix(result.size());
            return result;
        }
    }
}


namespace Anemone
{
    auto CommandLine::GetOption(
        std::string_view name) -> std::optional<std::string_view>
    {
        std::optional<std::string_view> result{};

        for (int i = 1; i < Internal::GCommandLineArgC; ++i)
        {
            std::string_view token{Internal::GCommandLineArgV[i]};

            if (auto value = ExtractTokenValue(token, name); value.has_value())
            {
                result = *value;

                // NOTE: do not break this loop - we explicitly want last value provided by the user.
            }
        }

        return result;
    }

    void CommandLine::GetOption(
        std::string_view name,
        std::vector<std::string_view>& values)
    {
        for (int i = 1; i < Internal::GCommandLineArgC; ++i)
        {
            std::string_view token{Internal::GCommandLineArgV[i]};

            if (auto value = ExtractTokenValue(token, name))
            {
                values.emplace_back(*value);
            }
        }
    }

    void CommandLine::GetPositional(
        std::vector<std::string_view>& positional)
    {
        for (int i = 1; i < Internal::GCommandLineArgC; ++i)
        {
            std::string_view token{Internal::GCommandLineArgV[i]};

            if (token.starts_with('-'))
            {
                // Not a positional argument.
                continue;
            }

            positional.emplace_back(token);
        }
    }

    void CommandLine::Parse(
        std::string_view commandLine,
        std::vector<std::string_view>& positional,
        std::vector<std::string_view>& switches,
        std::vector<std::pair<std::string_view, std::string_view>>& options)
    {
        while (not commandLine.empty())
        {
            if (auto token = ParseToken(commandLine); not token.empty())
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
                        std::string_view const name = token.substr(0, separator);
                        std::string_view value = token.substr(separator + 1);

                        // Remove quote from value.
                        value = UnquoteToken(value);

                        options.emplace_back(name, value);
                    }
                    else
                    {
                        // Token itself is an option.
                        switches.emplace_back(token);
                    }
                }
                else
                {
                    // Positional parameter.
                    positional.emplace_back(UnquoteToken(token));
                }
            }
        }
    }

    void CommandLine::Build(const char* const* argv, std::string& result)
    {
        Interop::string_buffer<char, 256> buffer{};

        size_t const estimatedLength = [&]() -> size_t
        {
            size_t length = 0;

            for (const char* const* it = argv; *it; ++it)
            {
                // Escaping may require more space than the original string. In worst case, every character needs to be escaped.
                // This is why we need to double the size of the buffer.
                size_t const argumentLength = std::char_traits<char>::length(*it);

                // Every argument is quoted.
                size_t constexpr quoteLength = 2uz;

                // For spacing between arguments.
                size_t constexpr spaceLength = 1uz;
                length += argumentLength + quoteLength + spaceLength;
            }

            return length;
        }();

        // Reserve space for the command line. It's okay to overallocate.
        buffer.resize_for_override(estimatedLength);

        // Write to the buffer.
        char* out = buffer.data();

        for (const char* const* it = argv; *it; ++it)
        {
            if (it != argv)
            {
                (*out++) = ' ';
            }

            std::string_view arg = *it;
            bool quoted = false;

            if (arg.empty() or (arg.find_first_of(" \f\n\r\t\v") != std::string_view::npos))
            {
                quoted = true;
            }

            if (quoted)
            {
                (*out++) = '"';
            }

            size_t backslashes = 0;

            for (char ch : arg)
            {
                if (ch == '\\')
                {
                    ++backslashes;
                }
                else if (ch == '"')
                {
                    if (backslashes)
                    {
                        for (size_t i = 0; i < 2 * backslashes; ++i)
                        {
                            (*out++) = '\\';
                        }

                        backslashes = 0;
                    }

                    (*out++) = '\\';
                    (*out++) = ch;
                }
                else
                {
                    if (backslashes)
                    {
                        for (size_t i = 0; i < backslashes; ++i)
                        {
                            (*out++) = '\\';
                        }

                        backslashes = 0;
                    }

                    (*out++) = ch;
                }
            }

            if (backslashes)
            {
                if (quoted)
                {
                    backslashes *= 2;
                }

                for (size_t i = 0; i < backslashes; ++i)
                {
                    (*out++) = '\\';
                }
            }

            if (quoted)
            {
                (*out++) = '"';
            }
        }

        size_t const length = out - buffer.data();
        buffer.trim(length);
        result = buffer.as_view();
    }
}
