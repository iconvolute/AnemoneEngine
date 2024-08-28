// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
// ReSharper disable CppPassValueParameterByConstReference

#pragma once
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics::Private
{
    template <size_t N>
    anemone_forceinline double DotProduct(SimdVector<double> a, SimdVector<double> b)
    {
        if constexpr (N == 1)
        {
            return a.Inner[0] * b.Inner[0];
        }
        else if constexpr (N == 2)
        {
            return a.Inner[0] * b.Inner[0] + a.Inner[1] * b.Inner[1];
        }
        else if constexpr (N == 3)
        {
            return a.Inner[0] * b.Inner[0] + a.Inner[1] * b.Inner[1] + a.Inner[2] * b.Inner[2];
        }
        else if constexpr (N == 4)
        {
            return a.Inner[0] * b.Inner[0] + a.Inner[1] * b.Inner[1] + a.Inner[2] * b.Inner[2] + a.Inner[3] * b.Inner[3];
        }
        else
        {
            std::unreachable();
        }
    }

    template <size_t N>
    anemone_forceinline double DotProduct(SimdVector<double> x)
    {
        if constexpr (N == 1)
        {
            return x.Inner[0] * x.Inner[0];
        }
        else if constexpr (N == 2)
        {
            return x.Inner[0] * x.Inner[0] + x.Inner[1] * x.Inner[1];
        }
        else if constexpr (N == 3)
        {
            return x.Inner[0] * x.Inner[0] + x.Inner[1] * x.Inner[1] + x.Inner[2] * x.Inner[2];
        }
        else if constexpr (N == 4)
        {
            return x.Inner[0] * x.Inner[0] + x.Inner[1] * x.Inner[1] + x.Inner[2] * x.Inner[2] + x.Inner[3] * x.Inner[3];
        }
        else
        {
            std::unreachable();
        }
    }
}
