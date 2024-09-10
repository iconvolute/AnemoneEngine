#include "AnemoneRuntime/Math/Margins.hxx"

namespace Anemone::Math
{
    constexpr MarginsF CreateUniform(float uniform)
    {
        return {uniform, uniform, uniform, uniform};
    }

    constexpr MarginsF Create(float left, float top, float right, float bottom)
    {
        return {left, top, right, bottom};
    }

    constexpr MarginsF Zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    constexpr float MarginsF::Horizontal() const
    {
        return this->Left + this->Right;
    }

    constexpr float MarginsF::Vertical() const
    {
        return this->Top + this->Bottom;
    }
}
