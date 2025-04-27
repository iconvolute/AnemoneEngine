#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <compare>

namespace Anemone
{
    struct Instant
    {
    public:
        Duration Inner;

        [[nodiscard]] friend constexpr auto operator<=>(Instant const&, Instant const&) = default;

        [[nodiscard]] RUNTIME_API static Instant Now();

        [[nodiscard]] Duration QueryElapsed() const
        {
            Instant const current = Now();
            return Duration{current.Inner - this->Inner};
        }

        [[nodiscard]] Duration SinceEpoch() const
        {
            return this->Inner;
        }

        [[nodiscard]] Duration Since(Instant const& earlier) const
        {
            return Duration{this->Inner - earlier.Inner};
        }

        template <typename CallbackT>
        [[nodiscard]] Duration Measure(CallbackT&& callback) const
        {
            std::forward<CallbackT>(callback)();
            return this->QueryElapsed();
        }
    };

    [[nodiscard]] constexpr Instant operator+(Instant const& self, Duration const& other)
    {
        return Instant{self.Inner + other};
    }

    [[nodiscard]] constexpr Instant operator-(Instant const& self, Duration const& other)
    {
        return Instant{self.Inner - other};
    }

    [[nodiscard]] constexpr Duration operator-(Instant const& self, Instant const& other)
    {
        return self.Inner - other.Inner;
    }

    constexpr Instant& operator+=(Instant& self, Duration const& other)
    {
        self.Inner += other;
        return self;
    }

    constexpr Instant& operator-=(Instant& self, Duration const& other)
    {
        self.Inner -= other;
        return self;
    }
}
