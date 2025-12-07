#include "AnemoneRuntime.Math/Size.hxx"
#include "AnemoneRuntime.Math/Functions.hxx"

namespace Anemone
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
        return IsNearZero(this->Width) && IsNearZero(this->Height);
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
