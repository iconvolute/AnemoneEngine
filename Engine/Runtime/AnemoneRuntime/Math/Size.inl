#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"

namespace Anemone::Math
{
    constexpr SizeF SizeF::CreateUniform(float uniform)
    {
        return {uniform, uniform};
    }

    constexpr SizeF SizeF::Create(float width, float height)
    {
        return {width, height};
    }

    constexpr SizeF SizeF::Zero()
    {
        return {0.0f, 0.0f};
    }

    constexpr bool SizeF::IsEmpty() const
    {
        return Numerics::IsNearZero(this->Width) && Numerics::IsNearZero(this->Height);
    }

    constexpr float SizeF::AspectRatio() const
    {
        return this->Width / this->Height;
    }

    constexpr float SizeF::InverseAspectRatio() const
    {
        return this->Height / this->Width;
    }
}
