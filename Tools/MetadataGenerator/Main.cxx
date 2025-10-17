#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <map>
#include <functional>
#include <optional>
#include <expected>

// Minimalistic parser for custom directives in C++ source files.
// Reacts only for specific preprocessor-like directives in the form of UPPERCASE_IDENTIFIER(...).
// Directive must be the first non-whitespace characters on the line.
// It must be followed by '(' character and end with ')' character.
// Everything between '(' and ')' is treated as parameters (comma-separated).
// Line must end with ')' and any trailing characters are ignored.

struct ParsedDirective final
{
    std::string_view Name{};
    std::string_view Arguments{};
};

struct LineParser final
{
    std::string_view Content;

    static constexpr bool IsDirectiveCharacter(char c)
    {
        return (('A' <= c) and (c <= 'Z')) or (('0' <= c) and (c <= '9')) or (c == '_');
    }

    static constexpr bool IsWhitespaceCharacter(char c)
    {
        return (c == ' ') or (c == '\t');
    }

    template <typename Predicate>
    constexpr void SkipUntil(Predicate predicate)
    {
        std::string_view::size_type n = 0;

        for (; n < this->Content.size(); ++n)
        {
            char const c = this->Content[n];

            if (not predicate(c))
            {
                break;
            }
        }

        this->Content.remove_prefix(n);
    }

    template <typename Predicate>
    constexpr std::string_view ConsumeUntil(Predicate predicate)
    {
        std::string_view::size_type n = 0;

        for (; n < this->Content.size(); ++n)
        {
            char const c = this->Content[n];

            if (not predicate(c))
            {
                break;
            }
        }

        std::string_view result = this->Content.substr(0, n);

        this->Content.remove_prefix(n);

        return result;
    }

    constexpr bool ConsumeCharacter(char c)
    {
        if (not this->Content.starts_with(c))
        {
            return false;
        }

        this->Content.remove_prefix(1);
        return true;
    }

    constexpr void SkipWhitespace()
    {
        SkipUntil(IsWhitespaceCharacter);
    }

    constexpr std::string_view ConsumeDirectiveIdentifier()
    {
        return ConsumeUntil(IsDirectiveCharacter);
    }

    // returns:
    constexpr std::optional<std::string_view> ConsumeBalancedTokenSequence(char first, char last)
    {
        if (this->Content.starts_with(first))
        {
            this->Content.remove_prefix(1); // Remove the opening character.

            std::string_view::size_type depth = 1;
            std::string_view::size_type position = 0;

            for (; position < this->Content.size(); ++position)
            {
                char const c = this->Content[position];

                if (c == first)
                {
                    ++depth;
                }
                else if (c == last)
                {
                    --depth;
                    if (depth == 0)
                    {
                        break;
                    }
                }
            }

            if (depth != 0)
            {
                return {};
            }

            std::string_view result = this->Content.substr(0, position);
            this->Content.remove_prefix(position + 1); // Remove the closing character.
            return result;
        }

        return {};
    }

    constexpr std::optional<std::string_view> ConsumeDirectiveParameters()
    {
        return this->ConsumeBalancedTokenSequence('(', ')');
    }
};

struct PropertyDescriptor final
{
    std::string Name{};
};

struct TypeDescriptor final
{
    std::string Name{};
    std::string BaseType{};

    std::vector<std::unique_ptr<PropertyDescriptor>> Properties{};

    void GenerateCode(std::ofstream& output)
    {
        std::println(output, "// Type: {}", this->Name);
        for (const auto& property : this->Properties)
        {
            std::println(output, "//   Property: {}", property->Name);
        }
        std::println(output);
    }
};

struct ParserContext
{
    std::string ModuleName{};

    std::vector<std::unique_ptr<TypeDescriptor>> Types{};

    std::unique_ptr<TypeDescriptor> CurrentType{};

    std::vector<std::string> Function_Module_GeneratedInitialize{};
    std::vector<std::string> Function_Module_GeneratedFinalize{};

    void Generate(std::ofstream& output)
    {
        std::println(output, "// This file is auto-generated. Do not edit.");
        std::println(output, "void {}_GeneratedInitialize()", this->ModuleName);
        std::println(output, "{{");
        for (const auto& line : Function_Module_GeneratedInitialize)
        {
            std::println(output, "    {}", line);
        }

        for (auto& type : this->Types)
        {
            std::println(output, "    extern void GeneratedCode_TypeInitializer_{}();", type->Name);
            std::println(output, "    GeneratedCode_TypeInitializer_{}();", type->Name);
        }

        std::println(output, "}}");
        std::println(output);

        std::println(output, "void {}_GeneratedFinalize()", this->ModuleName);
        std::println(output, "{{");
        for (const auto& line : Function_Module_GeneratedFinalize)
        {
            std::println(output, "    {}", line);
        }
        std::println(output, "}}");
        std::println(output);

        for (auto& type : this->Types)
        {
            type->GenerateCode(output);
        }
    }
};

void AeBeginType(ParserContext& context, std::string_view arguments)
{
    context.CurrentType = std::make_unique<TypeDescriptor>();
    context.CurrentType->Name = arguments;
}

void AeProperty(ParserContext& context, std::string_view arguments)
{
    context.CurrentType->Properties.push_back(std::make_unique<PropertyDescriptor>(std::string{arguments}));
}

void AeEndType(ParserContext& context, std::string_view arguments)
{
    context.Types.push_back(std::move(context.CurrentType));
}

std::map<std::string_view, std::function<void(ParserContext& context, std::string_view line)>> const gDirectives{
    {"AE_BEGIN_TYPE", AeBeginType},
    {"AE_END_TYPE", AeEndType},
    {"AE_PROPERTY", AeProperty},
};

void Parse(ParserContext& context, std::ifstream& input)
{
    std::string line;
    while (std::getline(input, line))
    {
        LineParser parser{line};

        std::string_view directiveName = parser.ConsumeDirectiveIdentifier();

        if (directiveName.empty())
        {
            // No directive found on this line.
            continue;
        }

        auto it = gDirectives.find(directiveName);
        if (it == gDirectives.end())
        {
            // Unknown directive.
            continue;
        }

        // We found a directive - parse arguments.
        std::optional<std::string_view> arguments = parser.ConsumeDirectiveParameters();

        if (not arguments)
        {
            std::println(stderr, "Error: Malformed directive: {}", line);
            break;
        }

        it->second(context, arguments.value());
    }
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::println(stdout, "Usage: MetadataGenerator <module name> <input file> <output file>");
        return 1;
    }

    ParserContext context{};
    context.ModuleName = argv[1];

    std::filesystem::recursive_directory_iterator it{argv[2]};
    std::filesystem::recursive_directory_iterator end;

    std::ofstream output{argv[3]};


    while (it != end)
    {
        if (it->is_regular_file())
        {
            std::filesystem::path path = it->path();

            if (path.extension() != ".hxx" && path.extension() != ".cxx")
            {
                ++it;
                continue;
            }

            std::ifstream input{path};

            if (!input)
            {
                std::cerr << "Error opening files\n";
                return 1;
            }


            Parse(context, input);
        }
        ++it;
    }

    context.Generate(output);
    output.flush();

    return 0;
}
