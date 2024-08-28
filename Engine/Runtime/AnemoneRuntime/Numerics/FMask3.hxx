#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    struct FMask3A final
    {
        Private::SimdMask4F Inner;

        static FMask3A anemone_vectorcall Create(bool x, bool y, bool z);
        static FMask3A anemone_vectorcall Splat(bool value);
        static FMask3A anemone_vectorcall True();
        static FMask3A anemone_vectorcall False();
    };
}

// Operations
namespace Anemone::Numerics
{
    FMask3A anemone_vectorcall CompareEqual(FMask3A a, FMask3A b);
    FMask3A anemone_vectorcall CompareNotEqual(FMask3A a, FMask3A b);

    FMask3A anemone_vectorcall And(FMask3A a, FMask3A b);
    FMask3A anemone_vectorcall AndNot(FMask3A a, FMask3A b);
    FMask3A anemone_vectorcall Or(FMask3A a, FMask3A b);
    FMask3A anemone_vectorcall Xor(FMask3A a, FMask3A b);
    FMask3A anemone_vectorcall Not(FMask3A a);

    FMask3A anemone_vectorcall Select(FMask3A mask, FMask3A whenFalse, FMask3A whenTrue);

    template <bool X, bool Y, bool Z>
    FMask3A anemone_vectorcall Select(FMask3A whenFalse, FMask3A whenTrue);

    bool anemone_vectorcall All(FMask3A a);
    bool anemone_vectorcall Any(FMask3A a);
    bool anemone_vectorcall None(FMask3A a);
}

#include "AnemoneRuntime/Numerics/Private/FMask3.Avx.hxx"
