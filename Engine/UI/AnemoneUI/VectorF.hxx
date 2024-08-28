#pragma once
#include "AnemoneRuntime/Numerics/Functions.hxx"

#include <compare>

namespace Anemone::UI
{
    struct Vector2F final
    {
    public:
        float X;
        float Y;

        float GetLength() const
        {
            return Numerics::Sqrt((this->X * this->X) + (this->Y * this->Y));
        }

        constexpr float GetSquaredLength() const
        {
            return (this->X * this->X) + (this->Y * this->Y);
        }

        void Normalize()
        {
            float const axis = Numerics::Max(
                Numerics::Abs(this->X),
                Numerics::Abs(this->Y));

            this->X /= axis;
            this->Y /= axis;

            float const length = this->GetLength();

            this->X /= length;
            this->Y /= length;
        }

        constexpr void Negate()
        {
            this->X = -this->X;
            this->Y = -this->Y;
        }

        static constexpr float Cross(Vector2F const& left, Vector2F const& right)
        {
            return (left.X * right.Y) - (left.Y * right.X);
        }

        static constexpr float Dot(Vector2F const& left, Vector2F const& right)
        {
            return (left.X * right.X) + (left.Y * right.Y);
        }

        static float AngleBetween(Vector2F const& left, Vector2F const& right)
        {
            float const crossValue = (left.X * right.Y) - (right.X * left.Y);
            float const dotValue = (left.X * right.X) + (left.Y * right.Y);

            return atan2(crossValue, dotValue);
        }

        [[nodiscard]] constexpr auto operator<=>(Vector2F const& other) const = default;

        [[nodiscard]] constexpr Vector2F operator+(Vector2F const& other) const
        {
            return Vector2F{
                .X = this->X + other.X,
                .Y = this->Y + other.Y,
            };
        }
    };
}

namespace Anemone::UI
{
    struct Vector3F final
    {
        float X;
        float Y;
        float Z;

        [[nodiscard]] constexpr auto operator<=>(Vector3F const& other) const = default;
    };
}
