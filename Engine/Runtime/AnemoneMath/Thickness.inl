#include "AnemoneMath/Thickness.hxx"

namespace Anemone::Math
{
    constexpr ThicknessF ThicknessF::CreateUniform(float uniform)
    {
        return {uniform, uniform, uniform, uniform};
    }

    constexpr ThicknessF ThicknessF::Create(float left, float top, float right, float bottom)
    {
        return {left, top, right, bottom};
    }

    constexpr ThicknessF ThicknessF::Zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    constexpr float ThicknessF::Horizontal() const
    {
        return this->Left + this->Right;
    }

    constexpr float ThicknessF::Vertical() const
    {
        return this->Top + this->Bottom;
    }
}
