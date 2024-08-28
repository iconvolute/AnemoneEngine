#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    struct FMask2A final
    {
        Private::SimdMask4F Inner;

        static FMask2A anemone_vectorcall Create(bool x, bool y);
        static FMask2A anemone_vectorcall Splat(bool value);
        static FMask2A anemone_vectorcall True();
        static FMask2A anemone_vectorcall False();
    };
}

// Operations
namespace Anemone::Numerics
{
    FMask2A anemone_vectorcall CompareEqual(FMask2A a, FMask2A b);
    FMask2A anemone_vectorcall CompareNotEqual(FMask2A a, FMask2A b);

    FMask2A anemone_vectorcall And(FMask2A a, FMask2A b);
    FMask2A anemone_vectorcall AndNot(FMask2A a, FMask2A b);
    FMask2A anemone_vectorcall Or(FMask2A a, FMask2A b);
    FMask2A anemone_vectorcall Xor(FMask2A a, FMask2A b);
    FMask2A anemone_vectorcall Not(FMask2A a);

    FMask2A anemone_vectorcall Select(FMask2A mask, FMask2A whenFalse, FMask2A whenTrue);

    template <bool X, bool Y>
    FMask2A anemone_vectorcall Select(FMask2A whenFalse, FMask2A whenTrue);

    bool anemone_vectorcall All(FMask2A a);
    bool anemone_vectorcall Any(FMask2A a);
    bool anemone_vectorcall None(FMask2A a);
}

#include <AnemoneCore/Numerics/Private/FMask2.Avx.hxx>
