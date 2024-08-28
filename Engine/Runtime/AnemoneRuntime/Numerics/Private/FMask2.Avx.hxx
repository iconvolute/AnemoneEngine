#pragma once
#include "AnemoneRuntime/Numerics/FMask2.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    inline FMask2A anemone_vectorcall FMask2A::Create(bool x, bool y)
    {
        return FMask2A{Private::Mask4F_Create(x, y)};
    }

    inline FMask2A anemone_vectorcall FMask2A::Splat(bool value)
    {
        return FMask2A{Private::Mask4F_Replicate(value)};
    }

    inline FMask2A anemone_vectorcall FMask2A::True()
    {
        return FMask2A{Private::Mask4F_True()};
    }

    inline FMask2A anemone_vectorcall FMask2A::False()
    {
        return FMask2A{Private::Mask4F_False()};
    }
}

// Operations
namespace Anemone::Numerics
{
    inline FMask2A anemone_vectorcall CompareEqual(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNotEqual(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall And(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_And(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall AndNot(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_AndNot(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall Or(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_Or(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall Xor(FMask2A a, FMask2A b)
    {
        return FMask2A{Private::Mask4F_Xor(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall Not(FMask2A a)
    {
        return FMask2A{Private::Mask4F_Not(a.Inner)};
    }

    inline FMask2A anemone_vectorcall Select(FMask2A mask, FMask2A whenFalse, FMask2A whenTrue)
    {
        return FMask2A{Private::Mask4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y>
    FMask2A anemone_vectorcall Select(FMask2A whenFalse, FMask2A whenTrue)
    {
        return FMask2A{Private::Mask4F_Select<X,Y,X,Y>(whenFalse.Inner, whenTrue.Inner)};
    }

    inline bool anemone_vectorcall All(FMask2A a)
    {
        return Private::Mask4F_All2(a.Inner);
    }

    inline bool anemone_vectorcall Any(FMask2A a)
    {
        return Private::Mask4F_Any2(a.Inner);
    }

    inline bool anemone_vectorcall None(FMask2A a)
    {
        return Private::Mask4F_None2(a.Inner);
    }
}
