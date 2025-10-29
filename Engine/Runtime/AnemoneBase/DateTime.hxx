#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Duration.hxx"

#include <cstdint>
#include <compare>
#include <fmt/format.h>

namespace Anemone
{
    enum class DateTimeFormat
    {
        Date,
        Time,
        DateTime,
        File,
        TimeStamp,
    };

    struct DateTimeMembers final
    {
        int32_t Year;
        int32_t Month;
        int32_t Day;
        int32_t Hour;
        int32_t Minute;
        int32_t Second;
        int32_t Millisecond;
        int32_t Microsecond;
        int32_t DayOfYear;
        int32_t DayOfWeek;
    };

    enum class DateTimeKind : uint32_t
    {
        Unknown,
        Local,
        Utc,
    };
}

namespace Anemone
{
    struct DateTime final
    {
        Duration Inner;

        [[nodiscard]] friend constexpr auto operator<=>(DateTime const& self, DateTime const& other) = default;

        [[nodiscard]] RUNTIME_API static DateTime Now();
        [[nodiscard]] RUNTIME_API static DateTime UtcNow();

        [[nodiscard]] RUNTIME_API static std::optional<DateTime> FromMembers(DateTimeMembers const& members);
        [[nodiscard]] RUNTIME_API DateTimeMembers ToMembers() const;
    };

    inline constexpr DateTime UnixEpoch{
        .Inner{
            .Seconds = 62135596800,
            .Nanoseconds = 0,
        },
    };

    [[nodiscard]] RUNTIME_API bool ToString(std::string& result, DateTimeMembers const& value, std::string_view format);

    [[nodiscard]] constexpr DateTime operator+(DateTime const& self, Duration const& other)
    {
        return DateTime{self.Inner + other};
    }

    [[nodiscard]] constexpr DateTime operator-(DateTime const& self, Duration const& other)
    {
        return DateTime{self.Inner - other};
    }

    [[nodiscard]] constexpr Duration operator-(DateTime const& self, DateTime const& other)
    {
        return self.Inner - other.Inner;
    }

    [[nodiscard]] constexpr DateTime operator+=(DateTime& self, Duration const& other)
    {
        self.Inner += other;
        return self;
    }

    [[nodiscard]] constexpr DateTime operator-=(DateTime& self, Duration const& other)
    {
        self.Inner -= other;
        return self;
    }

    [[nodiscard]] constexpr Duration SinceUnixEpoch(DateTime const& self)
    {
        return self.Inner - UnixEpoch.Inner;
    }
}

namespace Anemone
{
    struct DateTimeOffset final
    {
        DateTime Local;
        Duration Bias;

        [[nodiscard]] friend constexpr auto operator<=>(DateTimeOffset const& self, DateTimeOffset const& other) = default;

        [[nodiscard]] RUNTIME_API static DateTimeOffset Now();
        [[nodiscard]] RUNTIME_API static Duration GetCurrentTimeZoneBias();
    };
}

template <>
struct fmt::formatter<Anemone::DateTimeMembers>
{
private:
    Anemone::DateTimeFormat m_Format = Anemone::DateTimeFormat::DateTime;

public:
    constexpr auto parse(auto& context)
    {
        auto it = context.begin();

        if (it != context.end())
        {
            switch (*it)
            {
            case 'd':
                {
                    ++it;
                    this->m_Format = Anemone::DateTimeFormat::Date;
                    break;
                }

            case 't':
                {
                    ++it;
                    this->m_Format = Anemone::DateTimeFormat::Time;
                    break;
                }

            case 'D':
                {
                    ++it;
                    this->m_Format = Anemone::DateTimeFormat::DateTime;
                    break;
                }

            case 'T':
                {
                    ++it;
                    this->m_Format = Anemone::DateTimeFormat::TimeStamp;
                    break;
                }

            case 'F':
                {
                    ++it;
                    this->m_Format = Anemone::DateTimeFormat::File;
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        return it;
    }

    constexpr auto format(Anemone::DateTimeMembers const& value, auto& context) const
    {
        auto out = context.out();

        switch (this->m_Format)
        {
        case Anemone::DateTimeFormat::Date:
            {
                out = fmt::format_to(out, "{:04}-{:02}-{:02}",
                    value.Year,
                    value.Month,
                    value.Day);
                break;
            }

        case Anemone::DateTimeFormat::Time:
            {
                out = fmt::format_to(out, "{:02}:{:02}:{:02}",
                    value.Hour,
                    value.Minute,
                    value.Second);
                break;
            }

        case Anemone::DateTimeFormat::DateTime:
            {
                out = fmt::format_to(out, "{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second);
                break;
            }

        case Anemone::DateTimeFormat::File:
            {
                out = fmt::format_to(out, "{:04}_{:02}_{:02}_{:02}_{:02}_{:02}_{:03}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second,
                    value.Millisecond);
                break;
            }

        case Anemone::DateTimeFormat::TimeStamp:
            {
                out = fmt::format_to(out, "{:04}.{:02}.{:02}-{:02}.{:02}.{:02}.{:03}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second,
                    value.Millisecond);
                break;
            }
        }

        return out;
    }
};

template <>
struct fmt::formatter<Anemone::DateTime> : fmt::formatter<Anemone::DateTimeMembers>
{
public:
    auto format(Anemone::DateTime const& value, auto& context) const
    {
        return fmt::formatter<Anemone::DateTimeMembers>::format(value.ToMembers(), context);
    }
};
