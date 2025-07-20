#pragma once
#include <limits>
#include <cstdint>

namespace Anemone::Branchless
{
    constexpr uint32_t uint32_inc(uint32_t value)
    {
        return value + 1;
    }

    constexpr uint32_t uint32_dec(uint32_t value)
    {
        return value - 1;
    }

    constexpr uint32_t uint32_neg(uint32_t value)
    {
        return static_cast<uint32_t>(-static_cast<int32_t>(value));
    }

    constexpr uint32_t uint32_not(uint32_t value)
    {
        return ~value;
    }

    constexpr uint32_t uint32_and(uint32_t left, uint32_t right)
    {
        return left & right;
    }

    constexpr uint32_t uint32_andnot(uint32_t left, uint32_t right)
    {
        return left & ~right;
    }

    constexpr uint32_t uint32_or(uint32_t left, uint32_t right)
    {
        return left | right;
    }

    constexpr uint32_t uint32_xor(uint32_t left, uint32_t right)
    {
        return left ^ right;
    }

    constexpr uint32_t uint32_nor(uint32_t left, uint32_t right)
    {
        return ~(left | right);
    }

    constexpr uint32_t uint32_shll(uint32_t value, int shift)
    {
        return value << shift;
    }

    constexpr uint32_t uint32_shrl(uint32_t value, int shift)
    {
        return value >> shift;
    }

    constexpr uint32_t uint32_shla(uint32_t value, int shift)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(value) << shift);
    }

    constexpr uint32_t uint32_shra(uint32_t value, int shift)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(value) >> shift);
    }

    constexpr uint32_t uint32_ror(uint32_t value, int shift)
    {
        return uint32_shrl(value, shift) | uint32_shll(value, 32 - shift);
    }

    constexpr uint32_t uint32_rol(uint32_t value, int shift)
    {
        return uint32_shll(value, shift) | uint32_shrl(value, 32 - shift);
    }

    constexpr bool uint32_all(uint32_t value)
    {
        return (~value) == 0;
    }

    constexpr bool uint32_any(uint32_t value)
    {
        return value != 0;
    }

    constexpr bool uint32_none(uint32_t value)
    {
        return value == 0;
    }

    // Extends sign bit on whole value
    constexpr uint32_t uint32_sign_extend(uint32_t value)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(value) >> 31);
    }

    constexpr uint32_t uint32_select_mask(uint32_t mask, uint32_t left, uint32_t right)
    {
        return uint32_or(
            uint32_and(left, mask),
            uint32_andnot(right, mask));
    }

    constexpr uint32_t uint32_select_sign(uint32_t sign, uint32_t left, uint32_t right)
    {
        return uint32_select_mask(
            uint32_sign_extend(sign),
            left,
            right);
    }


    constexpr int uint32_popcount(uint32_t value)
    {
        value = (value & 0x55555555) + ((value >> 1) & 0x55555555);
        value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
        value = (value & 0x0F0F0F0F) + ((value >> 4) & 0x0F0F0F0F);
        value = (value & 0x00FF00FF) + ((value >> 8) & 0x00FF00FF);
        value = (value & 0x0000FFFF) + ((value >> 16) & 0x0000FFFF);

        return value;
    }

    constexpr int uint32_parity(uint32_t value)
    {
        value = value ^ (value >> 1);
        value = value ^ (value >> 2);
        value = value ^ (value >> 4);
        value = value ^ (value >> 8);
        value = value ^ (value >> 16);

        return value & 1;
    }

    constexpr uint32_t uint32_reverse(uint32_t value)
    {
        value = ((value >> 1) & 0x55555555u) | ((value << 1) & 0xAAAAAAAAu);
        value = ((value >> 2) & 0x33333333u) | ((value << 2) & 0xCCCCCCCCu);
        value = ((value >> 4) & 0x0F0F0F0Fu) | ((value << 4) & 0xF0F0F0F0u);
        value = ((value >> 8) & 0x00FF00FFu) | ((value << 8) & 0xFF00FF00u);
        value = ((value >> 16) & 0x0000FFFFu) | ((value << 16) & 0xFFFF0000u);
        return value;
    }

    constexpr uint64_t uint64_reverse(uint64_t value)
    {
        value = ((value >> 1u) & 0x5555555555555555u) | ((value << 1u) & 0xAAAAAAAAAAAAAAAAu);
        value = ((value >> 2u) & 0xCCCCCCCCCCCCCCCCu) | ((value << 2u) & 0x3333333333333333u);
        value = ((value >> 4u) & 0xF0F0F0F0F0F0F0F0u) | ((value << 4u) & 0x0F0F0F0F0F0F0F0Fu);
        value = ((value >> 8u) & 0x00FF00FF00FF00FFu) | ((value << 8u) & 0xFF00FF00FF00FF00u);
        value = ((value >> 16u) & 0x0000FFFF0000FFFFu) | ((value << 1u) & 0xFFFF0000FFFF0000u);
        value = ((value >> 32u) & 0xFFFFFFFF00000000u) | ((value << 3u) & 0x00000000FFFFFFFFu);
        return value;
    }

    constexpr uint32_t uint32_gray_to_binary(uint32_t value)
    {
        value ^= (value >> 16u);
        value ^= (value >> 8u);
        value ^= (value >> 4u);
        value ^= (value >> 2u);
        value ^= (value >> 1u);
        return value;
    }

    constexpr uint64_t uint64_gray_to_binary(uint64_t value)
    {
        value ^= (value >> 32u);
        value ^= (value >> 16u);
        value ^= (value >> 8u);
        value ^= (value >> 4u);
        value ^= (value >> 2u);
        value ^= (value >> 1u);
        return value;
    }


    constexpr uint32_t UInt32_Negate(uint32_t x)
    {
        return static_cast<uint32_t>(-static_cast<int32_t>(x));
    }

    constexpr uint32_t UInt32_Not(uint32_t x)
    {
        return ~x;
    }

    constexpr uint32_t UInt32_SignExtend(uint32_t x)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(x) >> 31);
    }

    constexpr uint32_t UInt32_Nor(uint32_t x, uint32_t y)
    {
        return ~(x | y);
    }

    constexpr uint32_t UInt32_And(uint32_t x, uint32_t y)
    {
        return x & y;
    }

    constexpr uint32_t UInt32_AndNot(uint32_t x, uint32_t y)
    {
        return x & ~y;
    }

    constexpr uint32_t UInt32_Or(uint32_t x, uint32_t y)
    {
        return x | y;
    }

    constexpr uint32_t UInt32_Xor(uint32_t x, uint32_t y)
    {
        return x ^ y;
    }

    constexpr uint32_t UInt32_Select(uint32_t mask, uint32_t whenTrue, uint32_t whenFalse)
    {
        return (mask & whenTrue) | (~mask & whenFalse);
    }

    constexpr uint32_t UInt32_SelectSign(uint32_t sign, uint32_t whenTrue, uint32_t whenFalse)
    {
        return UInt32_Select(UInt32_SignExtend(sign), whenTrue, whenFalse);
    }

    constexpr uint32_t UInt32_CompareLessThan(uint32_t x, uint32_t y)
    {
        return UInt32_SignExtend(x - y);
    }

    constexpr uint32_t UInt32_CompareGreaterThan(uint32_t x, uint32_t y)
    {
        return UInt32_SignExtend(y - x);
    }

    constexpr uint32_t UInt32_CompareGreaterEqual(uint32_t x, uint32_t y)
    {
        return ~UInt32_CompareLessThan(x, y);
    }

    constexpr uint32_t UInt32_CompareLessEqual(uint32_t x, uint32_t y)
    {
        return ~UInt32_CompareGreaterThan(x, y);
    }

    constexpr uint32_t UInt32_IsNotZero(uint32_t x)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(x) | -static_cast<int32_t>(x)) >> 31;
    }

    constexpr uint32_t UInt32_IsNotZeroBit(uint32_t x)
    {
        // returns 0
        return static_cast<uint32_t>(static_cast<int32_t>(x) | -static_cast<int32_t>(x)) >> 31;
    }

    constexpr uint32_t UInt32_IsZero(uint32_t x)
    {
        return ~UInt32_IsNotZero(x);
    }

    constexpr uint32_t UInt32_CompareEqual(uint32_t x, uint32_t y)
    {
        return ~UInt32_IsNotZero(x - y);
    }

    constexpr uint32_t UInt32_CompareNotEqual(uint32_t x, uint32_t y)
    {
        return UInt32_IsNotZero(x - y);
    }

    constexpr uint32_t UInt32_ShiftArithmeticRight(uint32_t x, uint32_t y)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(x) >> y);
    }

    constexpr uint32_t UInt32_ShiftLogicalRight(uint32_t x, uint32_t y)
    {
        return x >> y;
    }

    constexpr uint32_t UInt32_ShiftLogicalLeft(uint32_t x, uint32_t y)
    {
        return x << y;
    }

    constexpr uint32_t UInt32_Add(uint32_t x, uint32_t y)
    {
        return x + y;
    }

    constexpr uint32_t UInt32_Add(uint32_t x, uint32_t y, uint32_t& carry)
    {
        uint32_t const result = x + y;

        if (result < x)
        {
            ++carry;
        }

        return result;
    }

    constexpr uint32_t UInt32_Subtract(uint32_t x, uint32_t y)
    {
        return x - y;
    }

    constexpr uint32_t UInt32_Subtract(uint32_t x, uint32_t y, uint32_t& borrow)
    {
        if (x < y)
        {
            ++borrow;
        }

        return x - y;
    }

    constexpr uint32_t UInt32_Multiply(uint32_t x, uint32_t y)
    {
        return x * y;
    }

    constexpr uint32_t UInt32_Divide(uint32_t x, uint32_t y)
    {
        return x / y;
    }
}

namespace Anemone::Branchless
{
    constexpr size_t uint64_write_7bit_encoded(uint8_t* buffer, uint64_t value)
    {
        size_t count = 0;

        while (value >= 0x80u)
        {
            (*buffer++) = static_cast<uint8_t>(value | 0x80u);

            value >>= 7u;

            ++count;
        }

        (*buffer) = static_cast<uint8_t>(value);
        ++count;

        return count;
    }

    constexpr size_t int64_write_7bit_encoded(uint8_t* buffer, int64_t value)
    {
        // ZigZag encoding
        return uint64_write_7bit_encoded(buffer, (value << 1) ^ (value >> 63));
    }

    constexpr size_t uint64_read_7bit_encoded(uint64_t& result, const uint8_t* buffer)
    {
        result = 0;

        size_t shift = 0;
        uint8_t value = 0;

        do
        {
            if (shift == 63)
            {
                return 0;
            }

            value = (*buffer++);

            result |= (value & 0x7Fu) << shift;

            shift += 7;
        } while ((value & 0x80) != 0);

        return shift / 7;
    }

    constexpr size_t int64_read_7bit_encoded(int64_t& result, const uint8_t* buffer)
    {
        uint64_t temp;
        size_t count = uint64_read_7bit_encoded(temp, buffer);

        int64_t t = static_cast<int64_t>(temp);
        // ZigZag decoding
        result = (t >> 1) ^ (-(t & 1));

        return count;
    }

    constexpr size_t uint64_count_7bit_encoded(uint64_t value)
    {
        size_t count = 0;

        while (value >= 0x80u)
        {
            value >>= 7u;
            ++count;
        }

        return count;
    }
}
