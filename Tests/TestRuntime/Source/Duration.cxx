#include <catch_amalgamated.hpp>

#include "AnemoneRuntime/Base/Duration.hxx"

TEST_CASE("Duration - Construction")
{
    using namespace Anemone;

    SECTION("From Seconds")
    {
        SECTION("+1 second")
        {
            Duration const duration = Duration::FromSeconds(1);
            CHECK(duration.Seconds == int64_t{1});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("-1 second")
        {
            Duration const duration = Duration::FromSeconds(-1);
            CHECK(duration.Seconds == int64_t{-1});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("0 seconds")
        {
            Duration const duration = Duration::FromSeconds(0);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("From INT64_MAX")
        {
            Duration const duration = Duration::FromSeconds(std::numeric_limits<int64_t>::max());
            CHECK(duration.Seconds == std::numeric_limits<int64_t>::max());
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("From INT64_MIN")
        {
            Duration const duration = Duration::FromSeconds(std::numeric_limits<int64_t>::min());
            CHECK(duration.Seconds == std::numeric_limits<int64_t>::min());
            CHECK(duration.Nanoseconds == int64_t{0});
        }
    }

    SECTION("From Milliseconds")
    {
        SECTION("+1 millisecond")
        {
            Duration const duration = Duration::FromMilliseconds(1);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{1'000'000});
        }

        SECTION("-1 millisecond")
        {
            Duration const duration = Duration::FromMilliseconds(-1);
            CHECK(duration.Seconds == int64_t{-1});
            CHECK(duration.Nanoseconds == int64_t{999'000'000});
        }

        SECTION("0 milliseconds")
        {
            Duration const duration = Duration::FromMilliseconds(0);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("From INT64_MAX")
        {
            Duration const duration = Duration::FromMilliseconds(std::numeric_limits<int64_t>::max());
            CHECK(duration.Seconds == int64_t{9223372036854775});
            CHECK(duration.Nanoseconds == int64_t{807'000'000});
        }

        SECTION("From INT64_MIN")
        {
            Duration const duration = Duration::FromMilliseconds(std::numeric_limits<int64_t>::min());
            CHECK(duration.Seconds == int64_t{-9223372036854776});
            CHECK(duration.Nanoseconds == int64_t{192'000'000});
        }
    }

    SECTION("From Microseconds")
    {
        SECTION("+1 microsecond")
        {
            Duration const duration = Duration::FromMicroseconds(1);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{1'000});
        }

        SECTION("-1 microsecond")
        {
            Duration const duration = Duration::FromMicroseconds(-1);
            CHECK(duration.Seconds == int64_t{-1});
            CHECK(duration.Nanoseconds == int64_t{999'999'000});
        }

        SECTION("0 microseconds")
        {
            Duration const duration = Duration::FromMicroseconds(0);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("From INT64_MAX")
        {
            Duration const duration = Duration::FromMicroseconds(std::numeric_limits<int64_t>::max());
            CHECK(duration.Seconds == int64_t{9223372036854});
            CHECK(duration.Nanoseconds == int64_t{775'807'000});
        }

        SECTION("From INT64_MIN")
        {
            Duration const duration = Duration::FromMicroseconds(std::numeric_limits<int64_t>::min());
            CHECK(duration.Seconds == int64_t{-9223372036855});
            CHECK(duration.Nanoseconds == int64_t{224'192'000});
        }
    }

    SECTION("From Nanoseconds")
    {
        SECTION("+1 nanosecond")
        {
            Duration const duration = Duration::FromNanoseconds(1);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{1});
        }

        SECTION("-1 nanosecond")
        {
            Duration const duration = Duration::FromNanoseconds(-1);
            CHECK(duration.Seconds == int64_t{-1});
            CHECK(duration.Nanoseconds == int64_t{999'999'999});
        }

        SECTION("0 nanoseconds")
        {
            Duration const duration = Duration::FromNanoseconds(0);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{0});
        }

        SECTION("From INT64_MAX")
        {
            Duration const duration = Duration::FromNanoseconds(std::numeric_limits<int64_t>::max());
            CHECK(duration.Seconds == int64_t{9223372036});
            CHECK(duration.Nanoseconds == int64_t{854'775'807});
        }

        SECTION("From INT64_MIN")
        {
            Duration const duration = Duration::FromNanoseconds(std::numeric_limits<int64_t>::min());
            CHECK(duration.Seconds == int64_t{-9223372037});
            CHECK(duration.Nanoseconds == int64_t{145'224'192});
        }
    }

    SECTION("From Frequency")
    {
        SECTION("1 Hz")
        {
            Duration const duration = Duration::FromFrequency(1);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{1'000'000'000});
        }

        SECTION("2 Hz")
        {
            Duration const duration = Duration::FromFrequency(2);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{500'000'000});
        }

        SECTION("10 Hz")
        {
            Duration const duration = Duration::FromFrequency(10);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{100'000'000});
        }

        SECTION("100 Hz")
        {
            Duration const duration = Duration::FromFrequency(100);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{10'000'000});
        }

        SECTION("1 KHz")
        {
            Duration const duration = Duration::FromFrequency(1'000);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{1'000'000});
        }

        SECTION("5 KHz")
        {
            Duration const duration = Duration::FromFrequency(5'000);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{200'000});
        }

        SECTION("125 KHz")
        {
            Duration const duration = Duration::FromFrequency(12'500);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{80'000});
        }

        SECTION("1.2 MHz")
        {
            Duration const duration = Duration::FromFrequency(1'200'000);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{833});
        }

        SECTION("251 MHz")
        {
            Duration const duration = Duration::FromFrequency(251'000'000);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{3});
        }

        SECTION("2.137 GHz")
        {
            Duration const duration = Duration::FromFrequency(2'137'000'000);
            CHECK(duration.Seconds == int64_t{0});
            CHECK(duration.Nanoseconds == int64_t{0});
        }
    }

    SECTION("From Members")
    {
        DurationMembers members{
            .Days = 1,
            .Hours = 2,
            .Minutes = 3,
            .Seconds = 4,
            .Milliseconds = 5,
            .Microseconds = 6,
            .Nanoseconds = 7,
            .Negative = false,
        };

        std::optional<Duration> duration = Duration::FromMembers(members);
        REQUIRE(duration);
        CHECK(duration->Seconds == 1 * 24 * 60 * 60 + 2 * 60 * 60 + 3 * 60 + 4);
        CHECK(duration->Nanoseconds == (5 * 1'000'000) + (6 * 1'000) + (7));
    }

    SECTION("To Members")
    {
        // 2 days, 1 hours, 3 minutes, 7 seconds, 1337 nanoseconds

        int64_t actual = 2; // days
        actual *= 24;
        actual += 1; // hours
        actual *= 60;
        actual += 3; // minutes
        actual *= 60;
        actual += 7; // seconds
        actual *= 1'000; // miliseconds
        actual *= 1'000; // microseconds
        actual *= 1'000; // nanoseconds
        actual += 1337;

        Duration const duration = Duration::FromNanoseconds(actual);

        auto members = duration.ToMembers();

        CHECK(members.Days == 2);
        CHECK(members.Hours == 1);
        CHECK(members.Minutes == 3);
        CHECK(members.Seconds == 7);
        CHECK(members.Milliseconds == 0);
        CHECK(members.Microseconds == 1);
        CHECK(members.Nanoseconds == 337);
        CHECK(members.Negative == false);
    }

    SECTION("To Members - INT64_MAX nanoseconds")
    {
        Duration const duration = Duration::FromNanoseconds(std::numeric_limits<int64_t>::max());

        auto members = duration.ToMembers();

        CHECK(members.Days == 106751);
        CHECK(members.Hours == 23);
        CHECK(members.Minutes == 47);
        CHECK(members.Seconds == 16);
        CHECK(members.Milliseconds == 854);
        CHECK(members.Microseconds == 775);
        CHECK(members.Nanoseconds == 807);
        CHECK(members.Negative == false);
    }

    SECTION("Near Equal")
    {
        Duration d1{.Seconds = 1, .Nanoseconds = 100'000};
        Duration d2{.Seconds = 1, .Nanoseconds = 500'000};

        CHECK_FALSE(IsNearEqual(d1, d2, Duration::FromNanoseconds(399'999)));
        CHECK(IsNearEqual(d1, d2, Duration::FromNanoseconds(400'000)));
        CHECK(IsNearEqual(d1, d2, Duration::FromNanoseconds(400'001)));
    }

    SECTION("Normalization")
    {
        Duration d1{.Seconds = 1, .Nanoseconds = 1'000'000'001};
        d1.Normalize();
        CHECK(d1.Seconds == 2);
        CHECK(d1.Nanoseconds == 1);

        Duration d2{.Seconds = -1, .Nanoseconds = -1'000'000'001};
        d2.Normalize();
        CHECK(d2.Seconds == -3);
        CHECK(d2.Nanoseconds == 999'999'999);
    }
}
