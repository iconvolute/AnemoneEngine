#pragma once

namespace Anemone::UI
{
    struct Size2F
    {
    public:
        float Width;
        float Height;

    public:
        [[nodiscard]] constexpr bool operator==(Size2F const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(Size2F const& other) const = default;

    public:
        constexpr bool IsEmpty() const;

        constexpr float GetAspectRatio() const;

        constexpr float GetInverseAspectRatio() const;
    };

    struct Size3F
    {
    public:
        float Width;
        float Height;
        float Depth;

    public:
        [[nodiscard]] constexpr bool operator==(Size3F const& other) const = default;
        [[nodiscard]] constexpr bool operator!=(Size3F const& other) const = default;

    public:
        constexpr bool IsEmpty() const;
    };
}

namespace Anemone::UI
{
    constexpr bool Size2F::IsEmpty() const
    {
        return (this->Width <= 0.0f) or (this->Height <= 0.0f);
    }

    constexpr float Size2F::GetAspectRatio() const
    {
        return this->Width / this->Height;
    }

    constexpr float Size2F::GetInverseAspectRatio() const
    {
        return this->Height / this->Width;
    }
}

namespace Anemone::UI
{
    constexpr bool Size3F::IsEmpty() const
    {
        return (this->Width <= 0.0f) or (this->Height <= 0.0f) or (this->Depth <= 0.0f);
    }
}
