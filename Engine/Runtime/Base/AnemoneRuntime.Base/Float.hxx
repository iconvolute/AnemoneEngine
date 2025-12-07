#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <bit>
#include <cmath>
#include <array>
#include <algorithm>

namespace Anemone
{
    struct Half final
    {
        uint16_t Inner;
    };

    struct Float16
    {
        static constexpr uint16_t SignMask = 0x8000;
        static constexpr int SignShift = 15;
        static constexpr uint16_t ShiftedSignMask = SignMask >> SignShift;
        static constexpr uint16_t BiasedExponentMask = 0x7C00;
        static constexpr int BiasedExponentShift = 10;
        static constexpr int BiasedExponentLength = 5;
        static constexpr uint16_t ShiftedBiasedExponentMask = BiasedExponentMask >> BiasedExponentShift;
        static constexpr uint16_t TrailingSignificandMask = 0x03FF;
        static constexpr int MinSign = 0;
        static constexpr int MaxSign = 1;
        static constexpr int MinBiasedExponent = 0x00;
        static constexpr int MaxBiasedExponent = 0x1F;
        static constexpr int ExponentBias = 15;
        static constexpr int MinExponent = -14;
        static constexpr int MaxExponent = +15;
        static constexpr uint16_t MinTrailingSignificand = 0x0000;
        static constexpr uint16_t MaxTrailingSignificand = 0x03FF;
        static constexpr int TrailingSignificandLength = 10;
        static constexpr int SignificandLength = TrailingSignificandLength + 1;
        static constexpr uint16_t PositiveZeroBits = 0x0000;
        static constexpr uint16_t NegativeZeroBits = 0x8000;
        static constexpr uint16_t EpsilonBits = 0x0001;
        static constexpr uint16_t PositiveInfinityBits = 0x7C00;
        static constexpr uint16_t NegativeInfinityBits = 0xFC00;
        static constexpr uint16_t PositiveQNaNBits = 0x7E00;
        static constexpr uint16_t NegativeQNaNBits = 0xFE00;
        static constexpr uint16_t MinValueBits = 0xFBFF;
        static constexpr uint16_t MaxValueBits = 0x7BFF;
        static constexpr uint16_t PositiveOneBits = 0x3C00;
        static constexpr uint16_t NegativeOneBits = 0xBC00;
        static constexpr uint16_t SmallestNormalBits = 0x0400;
        static constexpr uint16_t CompareEpsilon = 0x1400;

        [[nodiscard]] static constexpr bool BitIsFinite(uint16_t bits)
        {
            return (~bits & PositiveInfinityBits) != 0;
        }

        [[nodiscard]] static constexpr bool IsFinite(Half value)
        {
            return BitIsFinite(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsInfinity(uint16_t bits)
        {
            return (bits & ~SignMask) == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsInfinity(Half value)
        {
            return BitIsInfinity(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaN(uint16_t bits)
        {
            return (bits & ~SignMask) > PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaN(Half value)
        {
            return BitIsNaN(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaNOrZero(uint16_t bits)
        {
            return ((bits - 1) & ~SignMask) >= PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaNOrZero(Half value)
        {
            return BitIsNaNOrZero(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegative(uint16_t bits)
        {
            return static_cast<int16_t>(bits) < 0;
        }

        [[nodiscard]] static constexpr bool IsNegative(Half value)
        {
            return BitIsNegative(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegativeInfinity(uint16_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNegativeInfinity(Half value)
        {
            return BitIsNegativeInfinity(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNormal(uint16_t bits)
        {
            return ((bits & ~SignMask) - SmallestNormalBits) < (PositiveInfinityBits - SmallestNormalBits);
        }

        [[nodiscard]] static constexpr bool IsNormal(Half value)
        {
            return BitIsNormal(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsPositiveInfinity(uint16_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsPositiveInfinity(Half value)
        {
            return BitIsPositiveInfinity(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsSubnormal(uint16_t bits)
        {
            return ((bits & ~SignMask) - 1) < MaxTrailingSignificand;
        }

        [[nodiscard]] static constexpr bool IsSubnormal(Half value)
        {
            return BitIsSubnormal(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsZero(uint16_t bits)
        {
            return (bits & ~SignMask) == 0;
        }

        [[nodiscard]] static constexpr bool IsZero(Half value)
        {
            return BitIsZero(std::bit_cast<uint16_t>(value));
        }

        [[nodiscard]] static constexpr uint16_t BitCompose(int exponent, uint16_t value)
        {
            uint16_t const exponentBits = static_cast<uint16_t>(static_cast<int16_t>(exponent + ExponentBias)) << BiasedExponentShift;
            return static_cast<uint16_t>(exponentBits | (value >> (BiasedExponentLength + 1)));
        }

        [[nodiscard]] static constexpr Half Compose(int exponent, uint16_t value)
        {
            return std::bit_cast<Half>(BitCompose(exponent, value));
        }

        [[nodiscard]] static constexpr uint16_t BitCopySign(uint16_t value, uint16_t sign)
        {
            return static_cast<uint16_t>((value & ~SignMask) | (sign & SignMask));
        }

        [[nodiscard]] static constexpr Half CopySign(Half value, Half sign)
        {
            return std::bit_cast<Half>(BitCopySign(std::bit_cast<uint16_t>(value), std::bit_cast<uint16_t>(sign)));
        }

        [[nodiscard]] static constexpr uint16_t BitAbs(uint16_t value)
        {
            return value & ~SignMask;
        }

        [[nodiscard]] static constexpr Half Abs(Half value)
        {
            return std::bit_cast<Half>(BitAbs(std::bit_cast<uint16_t>(value)));
        }
    };

    struct Float32
    {
        static constexpr uint32_t SignMask = 0x8000'0000;
        static constexpr int SignShift = 31;
        static constexpr uint32_t ShiftedSignMask = SignMask >> SignShift;
        static constexpr uint32_t BiasedExponentMask = 0x7F80'0000;
        static constexpr int BiasedExponentShift = 23;
        static constexpr int BiasedExponentLength = 8;
        static constexpr uint32_t ShiftedBiasedExponentMask = BiasedExponentMask >> BiasedExponentShift;
        static constexpr uint32_t TrailingSignificandMask = 0x007F'FFFF;
        static constexpr int MinSign = 0;
        static constexpr int MaxSign = 1;
        static constexpr int MinBiasedExponent = 0x00;
        static constexpr int MaxBiasedExponent = 0xFF;
        static constexpr int ExponentBias = 127;
        static constexpr int MinExponent = -126;
        static constexpr int MaxExponent = +127;
        static constexpr uint32_t MinTrailingSignificand = 0x0000'0000;
        static constexpr uint32_t MaxTrailingSignificand = 0x007F'FFFF;
        static constexpr int TrailingSignificandLength = 23;
        static constexpr int SignificandLength = TrailingSignificandLength + 1;
        static constexpr uint32_t PositiveZeroBits = 0x0000'0000;
        static constexpr uint32_t NegativeZeroBits = 0x8000'0000;
        static constexpr uint32_t EpsilonBits = 0x0000'0001;
        static constexpr uint32_t PositiveInfinityBits = 0x7F80'0000;
        static constexpr uint32_t NegativeInfinityBits = 0xFF80'0000;
        static constexpr uint32_t PositiveQNaNBits = 0x7FC0'0000;
        static constexpr uint32_t NegativeQNaNBits = 0xFFC0'0000;
        static constexpr uint32_t MinValueBits = 0xFF7F'FFFF;
        static constexpr uint32_t MaxValueBits = 0x7F7F'FFFF;
        static constexpr uint32_t PositiveOneBits = 0x3F80'0000;
        static constexpr uint32_t NegativeOneBits = 0xBF80'0000;
        static constexpr uint32_t SmallestNormalBits = 0x0080'0000;
        static constexpr uint32_t CompareEpsilon = 0x3400'0000;

        [[nodiscard]] static constexpr bool BitIsFinite(uint32_t bits)
        {
            return (~bits & PositiveInfinityBits) != 0;
        }

        [[nodiscard]] static constexpr bool IsFinite(float value)
        {
            return BitIsFinite(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsInfinity(uint32_t bits)
        {
            return (bits & ~SignMask) == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsInfinity(float value)
        {
            return BitIsInfinity(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaN(uint32_t bits)
        {
            return (bits & ~SignMask) > PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaN(float value)
        {
            return BitIsNaN(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaNOrZero(uint32_t bits)
        {
            return ((bits - 1) & ~SignMask) >= PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaNOrZero(float value)
        {
            return BitIsNaNOrZero(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegative(uint32_t bits)
        {
            return static_cast<int32_t>(bits) < 0;
        }

        [[nodiscard]] static constexpr bool IsNegative(float value)
        {
            return BitIsNegative(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegativeInfinity(uint32_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNegativeInfinity(float value)
        {
            return BitIsNegativeInfinity(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNormal(uint32_t bits)
        {
            return ((bits & ~SignMask) - SmallestNormalBits) < (PositiveInfinityBits - SmallestNormalBits);
        }

        [[nodiscard]] static constexpr bool IsNormal(float value)
        {
            return BitIsNormal(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsPositiveInfinity(uint32_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsPositiveInfinity(float value)
        {
            return BitIsPositiveInfinity(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsSubnormal(uint32_t bits)
        {
            return ((bits & ~SignMask) - 1) < MaxTrailingSignificand;
        }

        [[nodiscard]] static constexpr bool IsSubnormal(float value)
        {
            return BitIsSubnormal(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsZero(uint32_t bits)
        {
            return (bits & ~SignMask) == 0;
        }

        [[nodiscard]] static constexpr bool IsZero(float value)
        {
            return BitIsZero(std::bit_cast<uint32_t>(value));
        }

        [[nodiscard]] static constexpr uint32_t BitCompose(int exponent, uint32_t value)
        {
            uint32_t const exponentBits = static_cast<uint32_t>(static_cast<int32_t>(exponent + ExponentBias)) << BiasedExponentShift;
            return static_cast<uint32_t>(exponentBits | (value >> (BiasedExponentLength + 1)));
        }

        [[nodiscard]] static constexpr float Compose(int exponent, uint32_t value)
        {
            return std::bit_cast<float>(BitCompose(exponent, value));
        }

        [[nodiscard]] static constexpr uint32_t BitCopySign(uint32_t value, uint32_t sign)
        {
            return static_cast<uint32_t>((value & ~SignMask) | (sign & SignMask));
        }

        [[nodiscard]] static constexpr float CopySign(float value, float sign)
        {
            return std::bit_cast<float>(BitCopySign(std::bit_cast<uint32_t>(value), std::bit_cast<uint32_t>(sign)));
        }

        [[nodiscard]] static constexpr uint32_t BitAbs(uint32_t value)
        {
            return value & ~SignMask;
        }

        [[nodiscard]] static constexpr float Abs(float value)
        {
            return std::bit_cast<float>(BitAbs(std::bit_cast<uint32_t>(value)));
        }
    };

    struct Float64
    {
        static constexpr uint64_t SignMask = 0x8000'0000'0000'0000;
        static constexpr int SignShift = 63;
        static constexpr uint64_t ShiftedSignMask = SignMask >> SignShift;
        static constexpr uint64_t BiasedExponentMask = 0x7FF0'0000'0000'0000;
        static constexpr int BiasedExponentShift = 52;
        static constexpr int BiasedExponentLength = 11;
        static constexpr uint64_t ShiftedBiasedExponentMask = BiasedExponentMask >> BiasedExponentShift;
        static constexpr uint64_t TrailingSignificandMask = 0x000F'FFFF'FFFF'FFFF;
        static constexpr int MinSign = 0;
        static constexpr int MaxSign = 1;
        static constexpr int MinBiasedExponent = 0x0000;
        static constexpr int MaxBiasedExponent = 0x07FF;
        static constexpr int ExponentBias = 1023;
        static constexpr int MinExponent = -1022;
        static constexpr int MaxExponent = +1023;
        static constexpr uint64_t MinTrailingSignificand = 0x0000'0000'0000'0000;
        static constexpr uint64_t MaxTrailingSignificand = 0x000F'FFFF'FFFF'FFFF;
        static constexpr int TrailingSignificandLength = 52;
        static constexpr int SignificandLength = TrailingSignificandLength + 1;
        static constexpr uint64_t PositiveZeroBits = 0x0000'0000'0000'0000;
        static constexpr uint64_t NegativeZeroBits = 0x8000'0000'0000'0000;
        static constexpr uint64_t EpsilonBits = 0x0000'0000'0000'0001;
        static constexpr uint64_t PositiveInfinityBits = 0x7FF0'0000'0000'0000;
        static constexpr uint64_t NegativeInfinityBits = 0xFFF0'0000'0000'0000;
        static constexpr uint64_t PositiveQNaNBits = 0x7FF8'0000'0000'0000;
        static constexpr uint64_t NegativeQNaNBits = 0xFFF8'0000'0000'0000;
        static constexpr uint64_t MinValueBits = 0xFFEF'FFFF'FFFF'FFFF;
        static constexpr uint64_t MaxValueBits = 0x7FEF'FFFF'FFFF'FFFF;
        static constexpr uint64_t PositiveOneBits = 0x3FF0'0000'0000'0000;
        static constexpr uint64_t NegativeOneBits = 0xBFF0'0000'0000'0000;
        static constexpr uint64_t SmallestNormalBits = 0x0010'0000'0000'0000;
        static constexpr uint64_t CompareEpsilon = 0x3CB0'0000'0000'0000;

        [[nodiscard]] static constexpr bool BitIsFinite(uint64_t bits)
        {
            return (~bits & PositiveInfinityBits) != 0;
        }

        [[nodiscard]] static constexpr bool IsFinite(double value)
        {
            return BitIsFinite(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsInfinity(uint64_t bits)
        {
            return (bits & ~SignMask) == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsInfinity(double value)
        {
            return BitIsInfinity(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaN(uint64_t bits)
        {
            return (bits & ~SignMask) > PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaN(double value)
        {
            return BitIsNaN(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNaNOrZero(uint64_t bits)
        {
            return ((bits - 1) & ~SignMask) >= PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNaNOrZero(double value)
        {
            return BitIsNaNOrZero(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegative(uint64_t bits)
        {
            return static_cast<int64_t>(bits) < 0;
        }

        [[nodiscard]] static constexpr bool IsNegative(double value)
        {
            return BitIsNegative(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNegativeInfinity(uint64_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsNegativeInfinity(double value)
        {
            return BitIsNegativeInfinity(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsNormal(uint64_t bits)
        {
            return ((bits & ~SignMask) - SmallestNormalBits) < (PositiveInfinityBits - SmallestNormalBits);
        }

        [[nodiscard]] static constexpr bool IsNormal(double value)
        {
            return BitIsNormal(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsPositiveInfinity(uint64_t bits)
        {
            return bits == PositiveInfinityBits;
        }

        [[nodiscard]] static constexpr bool IsPositiveInfinity(double value)
        {
            return BitIsPositiveInfinity(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsSubnormal(uint64_t bits)
        {
            return ((bits & ~SignMask) - 1) < MaxTrailingSignificand;
        }

        [[nodiscard]] static constexpr bool IsSubnormal(double value)
        {
            return BitIsSubnormal(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr bool BitIsZero(uint64_t bits)
        {
            return (bits & ~SignMask) == 0;
        }

        [[nodiscard]] static constexpr bool IsZero(double value)
        {
            return BitIsZero(std::bit_cast<uint64_t>(value));
        }

        [[nodiscard]] static constexpr uint64_t BitCompose(int exponent, uint64_t value)
        {
            uint64_t const exponentBits = static_cast<uint64_t>(static_cast<int64_t>(exponent + ExponentBias)) << BiasedExponentShift;
            return static_cast<uint64_t>(exponentBits | (value >> (BiasedExponentLength + 1)));
        }

        [[nodiscard]] static constexpr double Compose(int exponent, uint64_t value)
        {
            return std::bit_cast<double>(BitCompose(exponent, value));
        }

        [[nodiscard]] static constexpr uint64_t BitCopySign(uint64_t value, uint64_t sign)
        {
            return static_cast<uint64_t>((value & ~SignMask) | (sign & SignMask));
        }

        [[nodiscard]] static constexpr double CopySign(double value, double sign)
        {
            return std::bit_cast<double>(BitCopySign(std::bit_cast<uint64_t>(value), std::bit_cast<uint64_t>(sign)));
        }

        [[nodiscard]] static constexpr uint64_t BitAbs(uint64_t value)
        {
            return value & ~SignMask;
        }

        [[nodiscard]] static constexpr double Abs(double value)
        {
            return std::bit_cast<double>(BitAbs(std::bit_cast<uint64_t>(value)));
        }
    };

    template <typename FixedT>
    constexpr FixedT FixedToFixed(FixedT value, size_t n, size_t p)
        requires(std::is_unsigned_v<FixedT>)
    {
        if (n > p)
        {
            value >>= (n - p);
        }
        else
        {
            if (value == (static_cast<FixedT>(1) << n) - 1)
            {
                value = static_cast<FixedT>((1 << p) - 1);
            }
            else
            {
                value = static_cast<FixedT>((value * (1 << p)) / ((1 << n) - 1));
            }
        }

        return value;
    }

    template <typename FixedT, typename FloatT>
    constexpr FixedT FloatToFixed(FloatT value, size_t bits)
        requires(std::is_unsigned_v<FixedT> and std::is_floating_point_v<FloatT>)
    {
        if (value < static_cast<FloatT>(0.0))
        {
            // Saturate below
            return {};
        }
        else if (value >= static_cast<FloatT>(1.0))
        {
            // Saturate above
            return static_cast<FixedT>((1 << bits) - 1);
        }

        // Convert
        return static_cast<FixedT>(value * static_cast<FloatT>(static_cast<FixedT>(1) << bits));
    }

    template <typename FloatT, typename FixedT>
    constexpr FloatT FixedToFloat(FixedT value, size_t bits)
        requires(std::is_unsigned_v<FixedT> and std::is_floating_point_v<FloatT>)
    {
        return static_cast<FloatT>(value) / static_cast<FloatT>((static_cast<FixedT>(1) << bits) - 1);
    }

    // Restore precise floating-point behavior. This code is based on result of comparison with NaN values.
#if ANEMONE_COMPILER_MSVC
#pragma float_control(precise, on, push)
#endif

    inline Half ToHalfBitwise(float value)
    {
        // constexpr uint32_t FloatSignMask = 0x8000'0000;
        constexpr uint32_t FloatMinExponent = 0x3880'0000u;
        constexpr uint32_t FloatExponent126 = 0x3f00'0000u;
        // constexpr uint32_t FloatBiasedExponentMask = 0x7f80'0000u;
        constexpr uint32_t FloatExponent13 = 0x0680'0000u;
        constexpr float MaxHalfValueBelowInfinity = 65520.0f;

        // constexpr uint16_t HalfBiasedExponentMask = 0x7C00;


        uint32_t bitValue = std::bit_cast<uint32_t>(value);
        uint32_t sign = (bitValue & Float32::SignMask) >> 16u;
        uint32_t realMask = static_cast<uint32_t>(-static_cast<int32_t>(not std::isnan(value)));
        value = std::abs(value);

        // Note:
        //      This code is based on yielding NaN value for this comparison, if value is NaN already.
        //      Do not change this to std::min or any other combination.

        value = (value > MaxHalfValueBelowInfinity)
            ? MaxHalfValueBelowInfinity
            : value;

        uint32_t exponentOffset0 = std::bit_cast<uint32_t>(std::max(value, std::bit_cast<float>(FloatMinExponent)));
        exponentOffset0 &= Float32::BiasedExponentMask;
        exponentOffset0 += FloatExponent13;
        value += std::bit_cast<float>(exponentOffset0);
        bitValue = std::bit_cast<uint32_t>(value);

        uint32_t maskedHalfExponentForNaN = ~realMask & static_cast<uint32_t>(Float16::BiasedExponentMask);
        bitValue -= FloatExponent126;
        uint32_t newExponent = bitValue >> 13;
        bitValue &= realMask;
        bitValue += newExponent;
        bitValue &= ~maskedHalfExponentForNaN;
        uint32_t signAndMaskExponent = maskedHalfExponentForNaN | sign;
        bitValue |= signAndMaskExponent;
        return std::bit_cast<Half>(static_cast<uint16_t>(bitValue));
    }

#if ANEMONE_COMPILER_MSVC
#pragma float_control(pop)
#endif

    inline float FromHalfBitwise(Half value)
    {
        constexpr uint32_t FloatExponentLowerBound = 0x3880'0000u;
        constexpr uint32_t FloatExponentOffset = 0x3800'0000u;
        constexpr int32_t HalfToSingleBitsMask = 0x0FFF'E000;

        int16_t valueInInt16Bits = std::bit_cast<int16_t>(value);
        uint32_t sign = static_cast<uint32_t>(static_cast<int32_t>(valueInInt16Bits)) & Float32::SignMask;
        uint32_t bitValueInProcess = static_cast<uint32_t>(valueInInt16Bits);
        uint32_t offsetExponent = bitValueInProcess & Float16::BiasedExponentMask;
        uint32_t subnormalMask = static_cast<uint32_t>(-static_cast<int32_t>(offsetExponent == 0u));
        int32_t infinityOrNaNMask = static_cast<int32_t>(offsetExponent == Float16::BiasedExponentMask);
        uint32_t maskedExponentLowerBound = subnormalMask & FloatExponentLowerBound;
        uint32_t offsetMaskedExponentLowerBound = FloatExponentOffset | maskedExponentLowerBound;
        bitValueInProcess <<= 13;
        offsetMaskedExponentLowerBound <<= infinityOrNaNMask;
        bitValueInProcess &= HalfToSingleBitsMask;
        bitValueInProcess += offsetMaskedExponentLowerBound;
        uint32_t absoluteValue = std::bit_cast<uint32_t>(std::bit_cast<float>(bitValueInProcess) - std::bit_cast<float>(maskedExponentLowerBound));
        return std::bit_cast<float>(absoluteValue | sign);
    }

    ANEMONE_RUNTIME_BASE_API Half ToHalf(float value);

    ANEMONE_RUNTIME_BASE_API float FromHalf(Half value);

    ANEMONE_RUNTIME_BASE_API std::array<Half, 4> ToHalf(std::array<float, 4> const& values);

    ANEMONE_RUNTIME_BASE_API std::array<float, 4> FromHalf(std::array<Half, 4> const& values);
}
