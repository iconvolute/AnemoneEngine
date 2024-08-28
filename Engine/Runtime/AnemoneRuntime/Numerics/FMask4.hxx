#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    struct FMask4A final
    {
        Private::SimdMask4F Inner;

        static FMask4A anemone_vectorcall Create(bool x, bool y, bool z, bool w);
        static FMask4A anemone_vectorcall Splat(bool value);
        static FMask4A anemone_vectorcall True();
        static FMask4A anemone_vectorcall False();
    };
}

// Operations
namespace Anemone::Numerics
{
    FMask4A anemone_vectorcall CompareEqual(FMask4A a, FMask4A b);
    FMask4A anemone_vectorcall CompareNotEqual(FMask4A a, FMask4A b);

    FMask4A anemone_vectorcall And(FMask4A a, FMask4A b);
    FMask4A anemone_vectorcall AndNot(FMask4A a, FMask4A b);
    FMask4A anemone_vectorcall Or(FMask4A a, FMask4A b);
    FMask4A anemone_vectorcall Xor(FMask4A a, FMask4A b);
    FMask4A anemone_vectorcall Not(FMask4A a);

    FMask4A anemone_vectorcall Select(FMask4A mask, FMask4A whenFalse, FMask4A whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    FMask4A anemone_vectorcall Select(FMask4A whenFalse, FMask4A whenTrue);

    bool anemone_vectorcall All(FMask4A a);
    bool anemone_vectorcall Any(FMask4A a);
    bool anemone_vectorcall None(FMask4A a);
}

#include "AnemoneRuntime/Numerics/Private/FMask4.Avx.hxx"
