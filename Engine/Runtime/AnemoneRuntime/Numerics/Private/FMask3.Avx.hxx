#pragma once
#include "AnemoneRuntime/Numerics/FMask3.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    inline FMask3A anemone_vectorcall FMask3A::Create(bool x, bool y, bool z)
    {
        return FMask3A{Private::Mask4F_Create(x, y, z)};
    }

    inline FMask3A anemone_vectorcall FMask3A::Splat(bool value)
    {
        return FMask3A{Private::Mask4F_Replicate(value)};
    }

    inline FMask3A anemone_vectorcall FMask3A::True()
    {
        return FMask3A{Private::Mask4F_True()};
    }

    inline FMask3A anemone_vectorcall FMask3A::False()
    {
        return FMask3A{Private::Mask4F_False()};
    }
}

// Operations
namespace Anemone::Numerics
{
    inline FMask3A anemone_vectorcall CompareEqual(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNotEqual(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall And(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_And(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall AndNot(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_AndNot(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall Or(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_Or(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall Xor(FMask3A a, FMask3A b)
    {
        return FMask3A{Private::Mask4F_Xor(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall Not(FMask3A a)
    {
        return FMask3A{Private::Mask4F_Not(a.Inner)};
    }

    inline FMask3A anemone_vectorcall Select(FMask3A mask, FMask3A whenFalse, FMask3A whenTrue)
    {
        return FMask3A{Private::Mask4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y, bool Z>
    FMask3A anemone_vectorcall Select(FMask3A whenFalse, FMask3A whenTrue)
    {
        return FMask3A{Private::Mask4F_Select<X, Y, Z, Y>(whenFalse.Inner, whenTrue.Inner)};
    }

    inline bool anemone_vectorcall All(FMask3A a)
    {
        return Private::Mask4F_All3(a.Inner);
    }

    inline bool anemone_vectorcall Any(FMask3A a)
    {
        return Private::Mask4F_Any3(a.Inner);
    }

    inline bool anemone_vectorcall None(FMask3A a)
    {
        return Private::Mask4F_None3(a.Inner);
    }
}
