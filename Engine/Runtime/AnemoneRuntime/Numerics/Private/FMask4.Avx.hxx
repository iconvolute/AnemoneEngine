#pragma once
#include "AnemoneRuntime/Numerics/FMask4.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    inline FMask4A anemone_vectorcall FMask4A::Create(bool x, bool y, bool z, bool w)
    {
        return FMask4A{Private::Mask4F_Create(x, y, z, w)};
    }

    inline FMask4A anemone_vectorcall FMask4A::Splat(bool value)
    {
        return FMask4A{Private::Mask4F_Replicate(value)};
    }

    inline FMask4A anemone_vectorcall FMask4A::True()
    {
        return FMask4A{Private::Mask4F_True()};
    }

    inline FMask4A anemone_vectorcall FMask4A::False()
    {
        return FMask4A{Private::Mask4F_False()};
    }
}

// Operations
namespace Anemone::Numerics
{
    inline FMask4A anemone_vectorcall CompareEqual(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNotEqual(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall And(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_And(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall AndNot(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_AndNot(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall Or(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_Or(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall Xor(FMask4A a, FMask4A b)
    {
        return FMask4A{Private::Mask4F_Xor(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall Not(FMask4A a)
    {
        return FMask4A{Private::Mask4F_Not(a.Inner)};
    }

    inline FMask4A anemone_vectorcall Select(FMask4A mask, FMask4A whenFalse, FMask4A whenTrue)
    {
        return FMask4A{Private::Mask4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y, bool Z, bool W>
    FMask4A anemone_vectorcall Select(FMask4A whenFalse, FMask4A whenTrue)
    {
        return FMask4A{Private::Mask4F_Select<X, Y, Z, W>(whenFalse.Inner, whenTrue.Inner)};
    }

    inline bool anemone_vectorcall All(FMask4A a)
    {
        return Private::Mask4F_All4(a.Inner);
    }

    inline bool anemone_vectorcall Any(FMask4A a)
    {
        return Private::Mask4F_Any4(a.Inner);
    }

    inline bool anemone_vectorcall None(FMask4A a)
    {
        return Private::Mask4F_None4(a.Inner);
    }
}
