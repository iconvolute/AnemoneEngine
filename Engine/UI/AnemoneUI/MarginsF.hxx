#pragma once


namespace Anemone::UI
{
    struct MarginsF
    {
    public:
        float Left;
        float Top;
        float Right;
        float Bottom;

    public:
        [[nodiscard]] constexpr bool operator==(MarginsF const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(MarginsF const& other) const = default;

    public:
        constexpr float GetHorizontal() const;
        constexpr float GetVertical() const;
    };
}

namespace Anemone::UI
{
    constexpr float MarginsF::GetHorizontal() const
    {
        return this->Left + this->Right;
    }

    constexpr float MarginsF::GetVertical() const
    {
        return this->Top + this->Bottom;
    }
}
