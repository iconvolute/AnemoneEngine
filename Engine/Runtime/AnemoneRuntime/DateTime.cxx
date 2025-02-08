#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"

#include <array>
#include <iterator>

namespace Anemone::Private
{
    inline constexpr int64_t SecondsInMinute = 60;
    inline constexpr int64_t SecondsInHour = 60 * SecondsInMinute;
    inline constexpr int64_t SecondsInDay = 24 * SecondsInHour;

    inline constexpr int64_t DaysInYear = 365;
    inline constexpr int64_t DaysIn4Years = (DaysInYear * 4) + 1;
    inline constexpr int64_t DaysIn100Years = (DaysIn4Years * 25) - 1;
    inline constexpr int64_t DaysIn400Years = (DaysIn100Years * 4) + 1;

    inline constexpr std::array<int16_t, 13> DaysBeforeMonth365{
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    inline constexpr std::array<int16_t, 13> DaysBeforeMonth366{
        0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

    template <typename T>
    constexpr void NormalizeTimeField(T& value, T& carry, T base)
    {
        value = static_cast<T>(value - base);
        carry = static_cast<T>(carry + 1);
    }

    constexpr bool IsLeapYear(int32_t year)
    {
        return (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
    }

    constexpr std::optional<Duration> DateToSeconds(int32_t year, int32_t month, int32_t day)
    {
        if ((year >= 1) && (year <= 9999) && (month >= 1) && (month <= 12))
        {
            bool const leap = IsLeapYear(year);
            auto const& days_to_month = (leap ? DaysBeforeMonth366 : DaysBeforeMonth365);

            int32_t const days_in_month = days_to_month[static_cast<size_t>(month)] - days_to_month[static_cast<size_t>(month - 1)];

            if ((day >= 1) && (day <= days_in_month))
            {
                int32_t const y = year - 1;
                int32_t const n = (y * 365) + (y / 4) - (y / 100) + (y / 400) + days_to_month[static_cast<size_t>(month - 1)] + day - 1;
                return Duration{
                    .Seconds = n * SecondsInDay,
                    .Nanoseconds = 0,
                };
            }
        }

        return std::nullopt;
    }
}

namespace Anemone
{
    DateTime DateTime::Now()
    {
        return Environment::GetCurrentDateTime();
    }

    DateTime DateTime::UtcNow()
    {
        return Environment::GetCurrentDateTimeUtc();
    }

    std::optional<DateTime> DateTime::FromMembers(DateTimeMembers const& members)
    {
        if (std::optional<Duration> const datePart = Private::DateToSeconds(members.Year, members.Month, members.Day))
        {
            DateTime result{
                .Inner = *datePart,
            };

            int64_t const hours = members.Hour;
            int64_t const minutes = (hours * 60) + members.Minute;
            int64_t const seconds = (minutes * 60) + members.Second;

            result.Inner.Seconds += seconds;

            int32_t const milliseconds = members.Millisecond;
            int32_t const microseconds = (milliseconds * 1'000) + members.Microsecond;
            int32_t const nanoseconds = (microseconds * 1'000);

            result.Inner.Nanoseconds = nanoseconds;

            if (nanoseconds < 1'000'000'000)
            {
                return result;
            }
        }

        return std::nullopt;
    }

    DateTimeMembers DateTime::ToMembers() const
    {
        DateTimeMembers result; // NOLINT(cppcoreguidelines-pro-type-member-init)

        int32_t n = static_cast<int32_t>(this->Inner.Seconds / Private::SecondsInDay);

        result.DayOfWeek = (n + 1) % 7;

        int32_t const y400 = static_cast<int32_t>(n / Private::DaysIn400Years);

        n -= static_cast<int32_t>(y400 * Private::DaysIn400Years);

        int32_t y100 = static_cast<int32_t>(n / Private::DaysIn100Years);

        if (y100 == 4)
        {
            y100 = 3;
        }

        n -= static_cast<int32_t>(y100 * Private::DaysIn100Years);

        int32_t const y4 = static_cast<int32_t>(n / Private::DaysIn4Years);

        n -= static_cast<int32_t>(y4 * Private::DaysIn4Years);

        int32_t y1 = static_cast<int32_t>(n / Private::DaysInYear);

        if (y1 == 4)
        {
            y1 = 3;
        }

        result.Year = (y400 * 400) + (y100 * 100) + (y4 * 4) + y1 + 1;

        n -= static_cast<int32_t>(y1 * Private::DaysInYear);

        result.DayOfYear = n + 1;

        bool const leap = (y1 == 3 && (y4 != 24 || y100 == 3));

        auto const& daysToMonth = (leap ? Private::DaysBeforeMonth366 : Private::DaysBeforeMonth365);

        int32_t m = (n >> 5) + 1;

        while (n >= daysToMonth[static_cast<size_t>(m)])
        {
            ++m;
        }

        result.Month = m;

        result.Day = n - daysToMonth[static_cast<size_t>(m - 1)] + 1;

        // Time
        int64_t ns = this->Inner.Nanoseconds / 1000;

        result.Microsecond = static_cast<int32_t>(ns % 1'000);
        ns /= 1'000;
        result.Millisecond = static_cast<int32_t>(ns % 1'000);

        // Sub-second time
        int64_t t = this->Inner.Seconds;

        result.Second = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Minute = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Hour = static_cast<int32_t>(t % 24);

        return result;
    }

    bool ToString(std::string& result, DateTimeMembers const& value, std::string_view format)
    {
        bool const morning = value.Hour < 12;

        bool percent = false;

        fmt::memory_buffer buffer{};
        auto out = std::back_inserter(buffer);

        for (char const ch : format)
        {
            if (ch == '%')
            {
                if (percent)
                {
                    (*out++) = '%';
                    percent = false;
                }
                else
                {
                    percent = true;
                    continue;
                }
            }

            if (percent)
            {
                percent = false;

                switch (ch)
                {
                case 'a':
                    {
                        const char* ampm = morning ? "am" : "pm";

                        out = std::copy(ampm, ampm + 2, out);
                        break;
                    }

                case 'A':
                    {
                        const char* ampm = morning ? "AM" : "PM";

                        out = std::copy(ampm, ampm + 2, out);
                        break;
                    }

                case 'd':
                    {
                        out = fmt::format_to(out, "{:02}", value.Day);
                        break;
                    }

                case 'D':
                    {
                        out = fmt::format_to(out, "{:03}", value.DayOfYear);
                        break;
                    }

                case 'm':
                    {
                        out = fmt::format_to(out, "{:02}", value.Month);
                        break;
                    }

                case 'y':
                    {
                        out = fmt::format_to(out, "{:02}", value.Year % 100);
                        break;
                    }

                case 'Y':
                    {
                        out = fmt::format_to(out, "{}", value.Year);
                        break;
                    }

                case 'h':
                    {
                        out = fmt::format_to(out, "{:02}", value.Hour % 12);
                        break;
                    }
                case 'H':
                    {
                        out = fmt::format_to(out, "{:02}", value.Hour);
                        break;
                    }
                case 'M':
                    {
                        out = fmt::format_to(out, "{:02}", value.Minute);
                        break;
                    }
                case 'S':
                    {
                        out = fmt::format_to(out, "{:02}", value.Second);
                        break;
                    }
                case 's':
                    {
                        out = fmt::format_to(out, "{:03}", value.Millisecond);
                        break;
                    }
                default:
                    {
                        AE_ASSERT(false, "Unknown format specified: {:c}", ch);
                        (*out++) = ch;
                        break;
                    }
                }
            }
            else
            {
                (*out++) = ch;
            }
        }

        AE_ASSERT(percent == false, "Unterminated string. Percent isgn without format specifier");

        result.assign(buffer.data(), buffer.size());
        return true;
    }

    DateTimeOffset DateTimeOffset::Now()
    {
        return DateTimeOffset{
            .Local = Environment::GetCurrentDateTime(),
            .Bias = Environment::GetCurrentTimeZoneBias(),
        };
    }

    Duration DateTimeOffset::GetCurrentTimeZoneBias()
    {
        return Environment::GetCurrentTimeZoneBias();
    }
}
