#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include <cstdint>
#include <compare>
#include <optional>
#include <string>

#include <fmt/format.h>

namespace Anemone::Internal
{
    static constexpr int64_t NanosecondsInSecond = 1'000'000'000;
    static constexpr int64_t MicrosecondsInSecond = 1'000'000;
    static constexpr int64_t MillisecondsInSecond = 1'000;
}

namespace Anemone
{
    struct DurationMembers final
    {
    public:
        int64_t Days;
        int32_t Hours;
        int32_t Minutes;
        int32_t Seconds;
        int32_t Milliseconds;
        int32_t Microseconds;
        int32_t Nanoseconds;
        bool Negative;
    };

    [[nodiscard]] RUNTIME_API bool ToString(std::string& result, DurationMembers const& value, std::string_view format);

    struct Duration final
    {
    public:
        int64_t Seconds;
        int64_t Nanoseconds;

    public:
        [[nodiscard]] friend constexpr auto operator<=>(Duration const& self, Duration const& other) = default;

    public:
        [[nodiscard]] constexpr int64_t ToNanoseconds() const
        {
            return (this->Seconds * Internal::NanosecondsInSecond) + this->Nanoseconds;
        }

        [[nodiscard]] constexpr int64_t ToMicroseconds() const
        {
            return (this->Seconds * Internal::MicrosecondsInSecond) + (this->Nanoseconds / 1'000);
        }

        [[nodiscard]] constexpr int64_t ToMilliseconds() const
        {
            return (this->Seconds * Internal::MillisecondsInSecond) + (this->Nanoseconds / 1'000'000);
        }

        [[nodiscard]] constexpr int64_t ToSeconds() const
        {
            return this->Seconds;
        }

        [[nodiscard]] static constexpr Duration FromNanoseconds(int64_t value)
        {
            Duration result{.Seconds = value / 1'000'000'000, .Nanoseconds = value % 1'000'000'000};
            result.Normalize();
            return result;
        }

        [[nodiscard]] static constexpr Duration FromMicroseconds(int64_t value)
        {
            Duration result{.Seconds = value / 1'000'000, .Nanoseconds = (value % 1'000'000) * 1'000};
            result.Normalize();
            return result;
        }

        [[nodiscard]] static constexpr Duration FromMilliseconds(int64_t value)
        {
            Duration result{.Seconds = value / 1'000, .Nanoseconds = (value % 1'000) * 1'000'000};
            result.Normalize();
            return result;
        }

        [[nodiscard]] static constexpr Duration FromSeconds(int64_t value)
        {
            return {.Seconds = value, .Nanoseconds = 0};
        }

        constexpr void Normalize()
        {
            this->Seconds += (this->Nanoseconds / Internal::NanosecondsInSecond);

            if ((this->Nanoseconds %= Internal::NanosecondsInSecond) < 0)
            {
                this->Nanoseconds += Internal::NanosecondsInSecond;
                --this->Seconds;
            }
        }

        [[nodiscard]] constexpr bool IsNegative() const
        {
            if (this->Seconds == 0)
            {
                return this->Nanoseconds < 0;
            }

            return this->Seconds < 0;
        }

        [[nodiscard]] RUNTIME_API static Duration New(int64_t seconds, int64_t nanoseconds);

        [[nodiscard]] RUNTIME_API static std::optional<Duration> FromMembers(DurationMembers const& members);

        [[nodiscard]] RUNTIME_API DurationMembers ToMembers() const;
    };

    [[nodiscard]] constexpr Duration operator+(Duration const& self, Duration const& other)
    {
        Duration result{
            .Seconds = self.Seconds + other.Seconds,
            .Nanoseconds = self.Nanoseconds + other.Nanoseconds,
        };
        result.Normalize();
        return result;
    }

    [[nodiscard]] constexpr Duration operator-(Duration const& self, Duration const& other)
    {
        Duration result{
            .Seconds = self.Seconds - other.Seconds,
            .Nanoseconds = self.Nanoseconds - other.Nanoseconds,
        };
        result.Normalize();
        return result;
    }

    [[nodiscard]] constexpr Duration operator-(Duration const& self)
    {
        Duration result{
            .Seconds = -self.Seconds,
            .Nanoseconds = -self.Nanoseconds,
        };

        result.Normalize();
        return result;
    }

    constexpr Duration operator+=(Duration& self, Duration const& other)
    {
        self = self + other;
        return self;
    }

    constexpr Duration operator-=(Duration& self, Duration const& other)
    {
        self = self - other;
        return self;
    }
}

template <>
struct fmt::formatter<Anemone::DurationMembers>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::DurationMembers const& value, auto& context) const
    {
        auto out = context.out();

        if (value.Negative)
        {
            (*out++) = '-';
        }

        if (value.Days != 0)
        {
            out = fmt::format_to(out, "{}.", value.Days);
        }

        out = fmt::format_to(out, "{:02}:{:02}:{:02}.{:03}",
            value.Hours,
            value.Minutes,
            value.Seconds,
            value.Milliseconds);

        return out;
    }
};

template <>
struct fmt::formatter<Anemone::Duration> : fmt::formatter<Anemone::DurationMembers>
{
    constexpr auto format(Anemone::Duration const& value, auto& context) const
    {
        return fmt::formatter<Anemone::DurationMembers>::format(value.ToMembers(), context);
    }
};
