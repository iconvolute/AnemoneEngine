#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <cstdint>
#include <compare>
#include <charconv>

#include <fmt/format.h>

namespace Anemone
{
    struct Version final
    {
        uint32_t Major{};
        uint32_t Minor{};
        uint32_t Patch{};
        uint32_t Tweak{};

        [[nodiscard]] friend auto operator<=>(Version const&, Version const&) = default;
    };

    [[nodiscard]] RUNTIME_API std::to_chars_result ToChars(char* first, char* last, Version const& version);
    [[nodiscard]] RUNTIME_API std::from_chars_result FromChars(char const* first, char const* last, Version& version);
}

template <>
struct fmt::formatter<Anemone::Version>
{
    constexpr auto parse(format_parse_context& context)
    {
        return context.begin();
    }

    auto format(Anemone::Version const& version, auto& context)
    {
        return format_to(context.out(), "{}.{}.{}.{}", version.Major, version.Minor, version.Patch, version.Tweak);
    }
};
