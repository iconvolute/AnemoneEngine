#include "AnemoneRuntime/Numerics/FMask2.hxx"
#include "AnemoneRuntime/Numerics/FMask3.hxx"
#include "AnemoneRuntime/Numerics/FMask4.hxx"

#include "AnemoneRuntime/Numerics/FVector2.hxx"
#include "AnemoneRuntime/Numerics/FVector3.hxx"
#include "AnemoneRuntime/Numerics/FVector4.hxx"

#include "AnemoneRuntime/Numerics/FQuaternion.hxx"
#include "AnemoneRuntime/Numerics/FMatrix4x4.hxx"

#include <type_traits>

namespace Anemone::Numerics
{
    static_assert(std::is_copy_constructible_v<FVector4>);
    static_assert(std::is_copy_assignable_v<FVector4>);

    static_assert(sizeof(FVector2) == (sizeof(float) * 2));
    static_assert(alignof(FVector2) == alignof(float));

    static_assert(sizeof(FVector3) == (sizeof(float) * 3));
    static_assert(alignof(FVector3) == alignof(float));

    static_assert(sizeof(FVector4) == (sizeof(float) * 4));
    static_assert(alignof(FVector4) == alignof(float));
}
