#pragma once
#include "AnemoneRuntime.Base/Float.hxx"

#include <cfloat>
#include <bit>

namespace Anemone
{
    // = e
    template <typename T>
    inline constexpr T E = static_cast<T>(2.71828182845904523536028747135266249775724709369996);

    // = log2(e)
    template <typename T>
    inline constexpr T Log2E = static_cast<T>(1.44269504088896340735992468100189213742664595415299);

    // = log10(e)
    template <typename T>
    inline constexpr T Log10E = static_cast<T>(0.43429448190325182765112891891660508229439700580367);

    // = π
    template <typename T>
    inline constexpr T Pi = static_cast<T>(3.14159265358979323846264338327950288419716939937510);

    // = 1/π
    template <typename T>
    inline constexpr T InvPi = static_cast<T>(0.31830988618379067153776752674502872406891929148091);

    // = 2π
    template <typename T>
    inline constexpr T Pi2 = static_cast<T>(6.28318530717958647692528676655900576839433879875021);

    // = 1/2π
    template <typename T>
    inline constexpr T InvPi2 = static_cast<T>(0.15915494309189533576888376337251436203445964574045);

    // = ln(2)
    template <typename T>
    inline constexpr T Ln2 = static_cast<T>(0.69314718055994530941723212145817656807550013436026);

    // = ln(10)
    template <typename T>
    inline constexpr T Ln10 = static_cast<T>(2.30258509299404568401799145468436420760110148862877);

    // = (1+√5)/2
    template <typename T>
    inline constexpr T Phi = static_cast<T>(1.61803398874989484820458683436563811772030917980576);

    // = Euler-Mascheroni constant
    template <typename T>
    inline constexpr T Gamma = static_cast<T>(0.57721566490153286060651209008240243104215933593992);

    // = √2
    template <typename T>
    inline constexpr T Sqrt2 = static_cast<T>(1.41421356237309504880168872420969807856967187537695);

    // = √3
    template <typename T>
    inline constexpr T Sqrt3 = static_cast<T>(1.73205080756887729352744634150587236694280525381038);

    // = √5
    template <typename T>
    inline constexpr T Sqrt5 = static_cast<T>(2.23606797749978969640917366873127623544061835961153);

    // = 1/√2
    template <typename T>
    inline constexpr T InvSqrt2 = static_cast<T>(0.70710678118654752440084436210484903928483593768847);

    // = 1/√3
    template <typename T>
    inline constexpr T InvSqrt3 = static_cast<T>(0.57735026918962576450914878050195745564760175127013);

    // = 1/√5
    template <typename T>
    inline constexpr T InvSqrt5 = static_cast<T>(0.44721359549995793928183473374625524708812367192231);

    // = 1/√π
    template <typename T>
    inline constexpr T InvSqrtPi = static_cast<T>(0.56418958354775628694807945156077258584405062932899);

    // = π/2
    template <typename T>
    inline constexpr T PiOver2 = static_cast<T>(1.57079632679489661923132169163975144209858469968755);

    // = π/3
    template <typename T>
    inline constexpr T PiOver3 = static_cast<T>(1.04719755119659774615421446109316762806572313312504);

    // = π/4
    template <typename T>
    inline constexpr T PiOver4 = static_cast<T>(0.78539816339744830961566084581987572104929234984378);

    // = π/6
    template <typename T>
    inline constexpr T PiOver6 = static_cast<T>(0.52359877559829887307710723054658381403286156656252);

    // = π/8
    template <typename T>
    inline constexpr T PiOver8 = static_cast<T>(0.39269908169872415480783042290993786052464617492189);
}

namespace Anemone::Internal
{
    template <typename T>
    inline constexpr T Factor_RevolutionsToDegrees = T(360);

    template <typename T>
    inline constexpr T Factor_RevolutionsToRadians = Pi2<T>;

    template <typename T>
    inline constexpr T Factor_DegreesToRevolutions = T(1) / T(360);

    template <typename T>
    inline constexpr T Factor_DegreesToRadians = Pi<T> / T(180);

    template <typename T>
    inline constexpr T Factor_RadiansToRevolutions = T(1) / Pi2<T>;

    template <typename T>
    inline constexpr T Factor_RadiansToDegrees = T(180) / Pi<T>;

    template <typename T, size_t Lanes, size_t Alignment = 16>
    struct VectorConstant
    {
        alignas(Alignment) T Elements[Lanes];

        template <typename U>
        U const* As() const
        {
            return reinterpret_cast<U const*>(this->Elements);
        }
    };

    // https://gist.github.com/publik-void/067f7f2fef32dbe5c27d6e215f824c91#sin-rel-error-minimized-degree-11

    inline constexpr float F32_SinC0 = +0.99999999997884898600402426033768998f;
    inline constexpr float F32_SinC1 = -0.166666666088260696413164261885310067f;
    inline constexpr float F32_SinC2 = +0.00833333072055773645376566203656709979f;
    inline constexpr float F32_SinC3 = -0.000198408328232619552901560108010257242f;
    inline constexpr float F32_SinC4 = +2.75239710746326498401791551303359689e-6f;
    inline constexpr float F32_SinC5 = -2.3868346521031027639830001794722295e-8f;

    inline constexpr VectorConstant<float, 4> F32x4_SinC0123{F32_SinC0, F32_SinC1, F32_SinC2, F32_SinC3};
    inline constexpr VectorConstant<float, 4> F32x4_SinC45nn{F32_SinC4, F32_SinC5, 0.0f, 0.0f};

    // https://gist.github.com/publik-void/067f7f2fef32dbe5c27d6e215f824c91#cos-rel-error-minimized-degree-10
    inline constexpr float F32_CosC0 = +0.99999999901810067632218592152414676f;
    inline constexpr float F32_CosC1 = -0.49999998049253581064488831264724178f;
    inline constexpr float F32_CosC2 = +0.0416665985274352494970529831079268818f;
    inline constexpr float F32_CosC3 = -0.00138879697151174993540500936074733546f;
    inline constexpr float F32_CosC4 = +0.00002474324689798977846771995314323317f;
    inline constexpr float F32_CosC5 = -2.57924183182520559803981154578763508e-7f;

    // 'echo "print(taylorform(tan(x),11, 0, [-pi,pi],relative));" | sollya'
    inline constexpr float F32_TanC0 = +1.0f;
    inline constexpr float F32_TanC1 = +0.33333333333333333333333333333333333333333333333334f;
    inline constexpr float F32_TanC2 = +0.133333333333333333333333333333333333333333333333333f;
    inline constexpr float F32_TanC3 = +5.3968253968253968253968253968253968253968253968254e-2f;
    inline constexpr float F32_TanC4 = +2.1869488536155202821869488536155202821869488536156e-2f;
    inline constexpr float F32_TanC5 = +8.8632355299021965688632355299021965688632355299022e-3f;

    // 'echo "print(taylorform(tan(x),15, 0, [-pi/2,pi/2],relative));" | sollya'
    inline constexpr float F32_ATanC0 = +1.0f;
    inline constexpr float F32_ATanC1 = -0.33333333333333333333333333333333333333333333333334f;
    inline constexpr float F32_ATanC2 = +0.2f;
    inline constexpr float F32_ATanC3 = -0.14285714285714285714285714285714285714285714285714f;
    inline constexpr float F32_ATanC4 = +0.111111111111111111111111111111111111111111111111113f;
    inline constexpr float F32_ATanC5 = -9.0909090909090909090909090909090909090909090909093e-2f;
    inline constexpr float F32_ATanC6 = +7.692307692307692307692307692307692307692307692308e-2f;
    inline constexpr float F32_ATanC7 = -6.6666666666666666666666666666666666666666666666666e-2f;


    inline constexpr VectorConstant<float, 4> F32x4_CosC0123{F32_CosC0, F32_CosC1, F32_CosC2, F32_CosC3};
    inline constexpr VectorConstant<float, 4> F32x4_CosC45nn{F32_CosC4, F32_CosC5, 0.0f, 0.0f};

    inline constexpr VectorConstant<float, 4> F32x4_PositiveUnitX{1.0f, 0.0f, 0.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_PositiveUnitY{0.0f, 1.0f, 0.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_PositiveUnitZ{0.0f, 0.0f, 1.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_PositiveUnitW{0.0f, 0.0f, 0.0f, 1.0f};

    inline constexpr VectorConstant<double, 4> F64x4_PositiveUnitX{1.0, 0.0, 0.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_PositiveUnitY{0.0, 1.0, 0.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_PositiveUnitZ{0.0, 0.0, 1.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_PositiveUnitW{0.0, 0.0, 0.0, 1.0};

    inline constexpr VectorConstant<float, 4> F32x4_NegativeUnitX{-1.0f, 0.0f, 0.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_NegativeUnitY{0.0f, -1.0f, 0.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_NegativeUnitZ{0.0f, 0.0f, -1.0f, 0.0f};
    inline constexpr VectorConstant<float, 4> F32x4_NegativeUnitW{0.0f, 0.0f, 0.0f, -1.0f};

    inline constexpr VectorConstant<double, 4> F64x4_NegativeUnitX{-1.0, 0.0, 0.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_NegativeUnitY{0.0, -1.0, 0.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_NegativeUnitZ{0.0, 0.0, -1.0, 0.0};
    inline constexpr VectorConstant<double, 4> F64x4_NegativeUnitW{0.0, 0.0, 0.0, -1.0};

    inline constexpr VectorConstant<float, 4> F32x4_Negate_Xnnn{-1.0f, 1.0f, 1.0f, 1.0f};
    inline constexpr VectorConstant<float, 4> F32x4_Negate_nXnn{1.0f, -1.0f, 1.0f, 1.0f};
    inline constexpr VectorConstant<float, 4> F32x4_Negate_nnXn{1.0f, 1.0f, -1.0f, 1.0f};
    inline constexpr VectorConstant<float, 4> F32x4_Negate_nnnX{1.0f, 1.0f, 1.0f, -1.0f};

    inline constexpr VectorConstant<float, 4> F32x4_PositiveOne{1.0f, 1.0f, 1.0f, 1.0f};
    inline constexpr VectorConstant<float, 4> F32x4_NegativeOne{-1.0f, -1.0f, -1.0f, -1.0f};

    inline constexpr VectorConstant<float, 4> F32x4_Pi{Pi<float>, Pi<float>, Pi<float>, Pi<float>};
    inline constexpr VectorConstant<float, 4> F32x4_Pi2{Pi2<float>, Pi2<float>, Pi2<float>, Pi2<float>};
    inline constexpr VectorConstant<float, 4> F32x4_InvPi{InvPi<float>, InvPi<float>, InvPi<float>, InvPi<float>};
    inline constexpr VectorConstant<float, 4> F32x4_InvPi2{InvPi2<float>, InvPi2<float>, InvPi2<float>, InvPi2<float>};
    inline constexpr VectorConstant<float, 4> F32x4_PiOver2{PiOver2<float>, PiOver2<float>, PiOver2<float>, PiOver2<float>};

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_XXXn{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_Xnnn{0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_nXnn{0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_nnXn{0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_nnnX{0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_XXnn{0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_XnXn{0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_nXnX{0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_XnXX{0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF};
    inline constexpr VectorConstant<uint32_t, 4> F32x4_SelectMask_XXXX{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    inline constexpr VectorConstant<uint32_t, 4> F32x4_ExponentMask_XXXX{
        Float32::BiasedExponentMask,
        Float32::BiasedExponentMask,
        Float32::BiasedExponentMask,
        Float32::BiasedExponentMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_MantissaMask_XXXX{
        Float32::SignMask | Float32::BiasedExponentMask,
        Float32::SignMask | Float32::BiasedExponentMask,
        Float32::SignMask | Float32::BiasedExponentMask,
        Float32::SignMask | Float32::BiasedExponentMask,
    };


    inline constexpr VectorConstant<uint32_t, 4> F32x4_PositiveInfinity_XXXX{
        Float32::PositiveInfinityBits,
        Float32::PositiveInfinityBits,
        Float32::PositiveInfinityBits,
        Float32::PositiveInfinityBits,
    };

    inline constexpr VectorConstant<uint64_t, 4> F64x4_PositiveInfinity_XXXX{
        Float64::PositiveInfinityBits,
        Float64::PositiveInfinityBits,
        Float64::PositiveInfinityBits,
        Float64::PositiveInfinityBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeInfinity_XXXX{
        Float32::NegativeInfinityBits,
        Float32::NegativeInfinityBits,
        Float32::NegativeInfinityBits,
        Float32::NegativeInfinityBits,
    };

    inline constexpr VectorConstant<uint64_t, 4> F64x4_NegativeInfinity_XXXX{
        Float64::NegativeInfinityBits,
        Float64::NegativeInfinityBits,
        Float64::NegativeInfinityBits,
        Float64::NegativeInfinityBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_PositiveQNaN_XXXX{
        Float32::PositiveQNaNBits,
        Float32::PositiveQNaNBits,
        Float32::PositiveQNaNBits,
        Float32::PositiveQNaNBits,
    };

    inline constexpr VectorConstant<uint64_t, 4> F64x4_PositiveQNaN_XXXX{
        Float64::PositiveQNaNBits,
        Float64::PositiveQNaNBits,
        Float64::PositiveQNaNBits,
        Float64::PositiveQNaNBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeQNaN_XXXX{
        Float32::NegativeQNaNBits,
        Float32::NegativeQNaNBits,
        Float32::NegativeQNaNBits,
        Float32::NegativeQNaNBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_PositiveMinNormalValue_XXXX{
        Float32::SmallestNormalBits,
        Float32::SmallestNormalBits,
        Float32::SmallestNormalBits,
        Float32::SmallestNormalBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeMinNormalValue_XXXX{
        Float32::SmallestNormalBits | Float32::SignMask,
        Float32::SmallestNormalBits | Float32::SignMask,
        Float32::SmallestNormalBits | Float32::SignMask,
        Float32::SmallestNormalBits | Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_PositiveMaxValue_XXXX{
        Float32::MaxValueBits,
        Float32::MaxValueBits,
        Float32::MaxValueBits,
        Float32::MaxValueBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeMaxValue_XXXX{
        Float32::MaxValueBits | Float32::SignMask,
        Float32::MaxValueBits | Float32::SignMask,
        Float32::MaxValueBits | Float32::SignMask,
        Float32::MaxValueBits | Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_EpsilonValue_XXXX{
        Float32::EpsilonBits,
        Float32::EpsilonBits,
        Float32::EpsilonBits,
        Float32::EpsilonBits,
    };

    inline constexpr VectorConstant<float, 4> F32x4_Epsilon_XXXX{
        std::numeric_limits<float>::epsilon(),
        std::numeric_limits<float>::epsilon(),
        std::numeric_limits<float>::epsilon(),
        std::numeric_limits<float>::epsilon(),
    };

    inline constexpr VectorConstant<double, 4> F64x4_Epsilon_XXXX{
        std::numeric_limits<double>::epsilon(),
        std::numeric_limits<double>::epsilon(),
        std::numeric_limits<double>::epsilon(),
        std::numeric_limits<double>::epsilon(),
    };

    inline constexpr VectorConstant<float, 4> F32x4_PositiveZero_XXXX{
        0.0f,
        0.0f,
        0.0f,
        0.0f,
    };

    inline constexpr VectorConstant<uint64_t, 4> F64x4_PositiveZero_XXXX{
        Float64::PositiveZeroBits,
        Float64::PositiveZeroBits,
        Float64::PositiveZeroBits,
        Float64::PositiveZeroBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeZero_XXXX{
        Float32::NegativeZeroBits,
        Float32::NegativeZeroBits,
        Float32::NegativeZeroBits,
        Float32::NegativeZeroBits,
    };

    inline constexpr VectorConstant<uint64_t, 4> F64x4_NegativeZero_XXXX{
        Float64::NegativeZeroBits,
        Float64::NegativeZeroBits,
        Float64::NegativeZeroBits,
        Float64::NegativeZeroBits,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_NegativeZero_XXXn{
        Float32::NegativeZeroBits,
        Float32::NegativeZeroBits,
        Float32::NegativeZeroBits,
        0,
    };

    inline constexpr VectorConstant<float, 4> F32x4_PositivePi_XXXX = {Pi<float>, Pi<float>, Pi<float>, Pi<float>};
    inline constexpr VectorConstant<float, 4> F32x4_NegativePi_XXXX = {-Pi<float>, -Pi<float>, -Pi<float>, -Pi<float>};

    inline constexpr VectorConstant<float, 4> F32x4_PositivePi2_XXXX = {Pi2<float>, Pi2<float>, Pi2<float>, Pi2<float>};
    inline constexpr VectorConstant<float, 4> F32x4_NegativePi2_XXXX = {-Pi2<float>, -Pi2<float>, -Pi2<float>, -Pi2<float>};

    inline constexpr VectorConstant<uint32_t, 4> F32x4_AbsMask_XXXX{
        ~Float32::SignMask,
        ~Float32::SignMask,
        ~Float32::SignMask,
        ~Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_XXXX{
        Float32::SignMask,
        Float32::SignMask,
        Float32::SignMask,
        Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_Xnnn{
        Float32::SignMask,
        0,
        0,
        0,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nXnn{
        0,
        Float32::SignMask,
        0,
        0,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nnXn{
        0,
        0,
        Float32::SignMask,
        0,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nnnX{
        0,
        0,
        0,
        Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nnXX{
        0,
        0,
        Float32::SignMask,
        Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nXnX{
        0,
        Float32::SignMask,
        0,
        Float32::SignMask,
    };

    inline constexpr VectorConstant<uint32_t, 4> F32x4_SignMask_nXXn{
        0,
        Float32::SignMask,
        Float32::SignMask,
        0,
    };

    inline constexpr VectorConstant<int32_t, 4> I32x4_MoveMaskShifts = {-31, -30, -29, -28};
    inline constexpr VectorConstant<int32_t, 4> I64x4_MoveMaskShifts = {-63, -62, -61, -60};
}
