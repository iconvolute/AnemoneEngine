#pragma once
#include <type_traits>
#include <compare>
#include <utility>

namespace Anemone
{
    template <typename T>
        requires(std::is_enum_v<T>)
    struct [[nodiscard]] Flags
    {
    public:
        using underlying_type = std::underlying_type_t<T>;
        using value_type = T;

        value_type Value;

    public:
        constexpr Flags() = default;

        constexpr Flags(T value)
            : Value{value}
        {
        }

        explicit constexpr Flags(underlying_type value)
            : Value{static_cast<value_type>(value)}
        {
        }

        [[nodiscard]] constexpr operator value_type() const
        {
            return Value;
        }

        [[nodiscard]] explicit constexpr operator underlying_type() const
        {
            return static_cast<underlying_type>(Value);
        }

        [[nodiscard]] constexpr Flags operator|(Flags other) const
        {
            return Flags{static_cast<value_type>(std::to_underlying(this->Value) | std::to_underlying(other.Value))};
        }

        [[nodiscard]] constexpr Flags operator&(Flags other) const
        {
            return Flags{static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(other.Value))};
        }

        [[nodiscard]] constexpr Flags operator^(Flags other) const
        {
            return Flags{static_cast<value_type>(std::to_underlying(this->Value) ^ std::to_underlying(other.Value))};
        }

        [[nodiscard]] constexpr Flags operator~() const
        {
            return Flags{static_cast<value_type>(~std::to_underlying(this->Value))};
        }

        constexpr Flags& operator|=(Flags other)
        {
            this->Value = static_cast<value_type>(std::to_underlying(this->Value) | std::to_underlying(other.Value));
            return *this;
        }

        constexpr Flags& operator&=(Flags other)
        {
            this->Value = static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(other.Value));
            return *this;
        }

        constexpr Flags& operator^=(Flags other)
        {
            this->Value = static_cast<value_type>(std::to_underlying(this->Value) ^ std::to_underlying(other.Value));
            return *this;
        }

        [[nodiscard]] constexpr bool operator!() const
        {
            return this->Value == value_type{};
        }

        [[nodiscard]] constexpr auto operator<=>(Flags const& other) const = default;

        [[nodiscard]] constexpr bool All(Flags other) const
        {
            return static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(other.Value)) == other.Value;
        }

        [[nodiscard]] constexpr bool Any(Flags other) const
        {
            return static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(other.Value)) != value_type{};
        }

        [[nodiscard]] constexpr bool None(Flags other) const
        {
            return static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(other.Value)) == value_type{};
        }

        [[nodiscard]] constexpr bool Has(value_type value) const
        {
            return static_cast<value_type>(std::to_underlying(this->Value) & std::to_underlying(value)) == value;
        }
    };
}
