#pragma once
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <algorithm>
#include <string>
#include <bit>
#include <cmath>

namespace Anemone::Private
{
    constexpr uint64_t MultiplyHigh(uint64_t left, uint64_t right, uint64_t& lower)
    {
        uint64_t const ll = left & 0xFFFFFFFFu;
        uint64_t const lu = left >> 32u;
        uint64_t const rl = right & 0xFFFFFFFFu;
        uint64_t const ru = right >> 32u;
        uint64_t const ll_rl = ll * rl;
        uint64_t const ll_ru = ll * ru;
        uint64_t const lu_rl = lu * rl;
        uint64_t const lu_ru = lu * ru;
        uint64_t const m = (ll_rl >> 32u) + ll_ru + (lu_rl & 0xFFFFFFFFu);
        lower = (ll_rl & 0xFFFFFFFFu) | (m << 32u);
        return (m >> 32u) + (lu_rl >> 32u) + lu_ru;
    }
}

namespace Anemone
{
    struct UInt128 final
    {
    private:
        uint64_t _upper{};
        uint64_t _lower{};

    public:
        constexpr UInt128() = default;

        constexpr explicit UInt128(uint64_t upper, uint64_t lower)
            : _upper{upper}
            , _lower{lower}
        {
        }

        static constexpr UInt128 Make(signed long long value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed long value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed int value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed short value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed char value)
        {
            int64_t const lower = value; // NOLINT(bugprone-signed-char-misuse)
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(unsigned long long value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned long value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned int value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned short value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned char value)
        {
            return UInt128{0, value};
        }

    public:
        constexpr bool IsZero() const
        {
            return (this->_lower | this->_upper) == 0;
        }

        constexpr uint64_t GetLower() const
        {
            return this->_lower;
        }

        constexpr uint64_t GetUpper() const
        {
            return this->_upper;
        }

        constexpr uint64_t ToUInt64() const
        {
            return this->_lower;
        }

        static constexpr UInt128 Min()
        {
            return UInt128{0, 0};
        }

        static constexpr UInt128 Max()
        {
            return UInt128{std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()};
        }

        static constexpr int Compare(UInt128 left, UInt128 right)
        {
            if (left._upper != right._upper)
            {
                return (left._upper < right._upper) ? -1 : 1;
            }

            if (left._lower != right._lower)
            {
                return (left._lower < right._lower) ? -1 : 1;
            }

            return 0;
        }

        static constexpr bool Equals(UInt128 left, UInt128 right)
        {
            return (left._lower == right._lower) && (left._upper == right._upper);
        }

        constexpr auto operator<=>(UInt128 const& right) const = default;

    public:
        static constexpr size_t CountLeadingZero(UInt128 value)
        {
            if (value._upper != 0)
            {
                return 127 - std::countl_zero(value._upper);
            }

            return 63 - std::countl_zero(value._lower);
        }

    public:
        static constexpr bool CheckedAdd(UInt128& result, UInt128 left, UInt128 right)
        {
            result = Add(left, right);
            return (result._upper < left._upper); // overflow
        }

        static constexpr UInt128 Add(UInt128 left, UInt128 right)
        {
            uint64_t const lower = left._lower + right._lower;

            return UInt128{
                left._upper + right._upper + ((lower < left._lower) ? 1 : 0), // carry
                lower,
            };
        }

        static constexpr bool CheckedSubtract(UInt128& result, UInt128 left, UInt128 right)
        {
            result = Subtract(left, right);
            return (result._upper > left._upper); // underflow
        }

        static constexpr UInt128 Subtract(UInt128 left, UInt128 right)
        {
            uint64_t const lower = left._lower - right._lower;

            return UInt128{
                left._upper - right._upper - ((lower > left._lower) ? 1 : 0), // borrow
                lower,
            };
        }

        static constexpr bool CheckedMultiply(UInt128& result, UInt128 left, UInt128 right)
        {
            return not MultiplyHigh(left, right, result).IsZero();
        }

        static constexpr UInt128 Multiply(UInt128 left, UInt128 right)
        {
            uint64_t lower;
            uint64_t upper = Private::MultiplyHigh(left._lower, right._lower, lower);
            upper += (left._lower * right._upper) + (left._upper * right._lower);
            return UInt128{upper, lower};
        }

        static constexpr bool CheckedDivide(UInt128& quotient, UInt128& remainder, UInt128 left, UInt128 right)
        {
            if ((right._upper == 0) and (right._lower == 1))
            {
                // Fast-path for division by 1
                quotient = left;

                remainder._lower = 0;
                remainder._upper = 0;

                return false;
            }

            if ((left._upper == 0) and (right._upper == 0))
            {
                // 64-bit fast path
                quotient._lower = left._lower / right._lower;
                quotient._upper = 0;

                remainder._lower = left._lower % right._lower;
                remainder._upper = 0;

                // No overflow possible
                return false;
            }

            if (int const r = Compare(left, right); r == 0)
            {
                // left / right => 1 rem 0
                quotient._lower = 1;
                quotient._upper = 0;

                remainder._lower = 0;
                remainder._upper = 0;

                return false;
            }
            else if (r < 0)
            {
                quotient._lower = 0;
                quotient._upper = 0;

                remainder = left;

                return false;
            }

            size_t const shift = CountLeadingZero(left) - CountLeadingZero(right);

            if (shift == 128)
            {
                // Overflow
                return true;
            }

            UInt128 d = BitShiftLeft(right, shift);
            UInt128 q{};

            for (size_t i = 0; i <= shift; ++i)
            {
                q = BitShiftLeft(q, 1);

                if (left >= d)
                {
                    left = left - d;
                    q._lower |= 1;
                }

                d = BitShiftRightZeroExtend(d, 1);
            }

            remainder = left;
            quotient = q;

            return false;
        }

        [[nodiscard]] static constexpr UInt128 Divide(UInt128& remainder, UInt128 left, UInt128 right)
        {
            UInt128 result{};
            CheckedDivide(result, remainder, left, right);
            return result;
        }

        static constexpr UInt128 MultiplyHigh(UInt128 left, UInt128 right, UInt128& lower)
        {
            UInt128 const ll{0, left._lower};
            UInt128 const lu{0, left._upper};
            UInt128 const rl{0, right._lower};
            UInt128 const ru{0, right._upper};

            UInt128 const m = ll * rl;
            UInt128 const t = lu * rl + m._upper;
            UInt128 const tl = ll * ru + t._lower;
            lower._lower = m._lower;
            lower._upper = tl._lower;
            return lu * ru + t._upper + tl._upper;
        }

        [[nodiscard]] constexpr UInt128 operator+(UInt128 right) const
        {
            return Add(*this, right);
        }

        [[nodiscard]] constexpr UInt128 operator-(UInt128 right) const
        {
            return Subtract(*this, right);
        }

        [[nodiscard]] constexpr UInt128 operator*(UInt128 right) const
        {
            return Multiply(*this, right);
        }

        [[nodiscard]] constexpr UInt128 operator/(UInt128 right) const
        {
            UInt128 quotient;
            UInt128 remainder;
            CheckedDivide(quotient, remainder, *this, right);
            return quotient;
        }

        [[nodiscard]] constexpr UInt128 operator%(UInt128 right) const
        {
            UInt128 quotient;
            UInt128 remainder;
            CheckedDivide(quotient, remainder, *this, right);
            return remainder;
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned int right) const
        {
            return *this + UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned int right) const
        {
            return *this - UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned int right) const
        {
            return *this * UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned int right) const
        {
            return *this / UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned int right) const
        {
            return *this % UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned long right) const
        {
            return *this + UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned long right) const
        {
            return *this - UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned long right) const
        {
            return *this * UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned long right) const
        {
            return *this / UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned long right) const
        {
            return *this % UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned long long right) const
        {
            return *this + UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned long long right) const
        {
            return *this - UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned long long right) const
        {
            return *this * UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned long long right) const
        {
            return *this / UInt128{0, right};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned long long right) const
        {
            return *this % UInt128{0, right};
        }

    public:
        [[nodiscard]] static constexpr UInt128 BitCompl(UInt128 value)
        {
            return UInt128{
                ~value._upper,
                ~value._lower,
            };
        }

        [[nodiscard]] static constexpr UInt128 BitAnd(UInt128 left, UInt128 right)
        {
            return UInt128{
                left._upper & right._upper,
                left._lower & right._lower,
            };
        }

        [[nodiscard]] static constexpr UInt128 BitOr(UInt128 left, UInt128 right)
        {
            return UInt128{
                left._upper | right._upper,
                left._lower | right._lower,
            };
        }

        [[nodiscard]] static constexpr UInt128 BitXor(UInt128 left, UInt128 right)
        {
            return UInt128{
                left._upper ^ right._upper,
                left._lower ^ right._lower,
            };
        }

        [[nodiscard]] static constexpr UInt128 BitRotateLeft(UInt128 value, size_t bits)
        {
            return BitOr(
                BitShiftLeft(value, bits),
                BitShiftRightZeroExtend(value, 128 - bits));
        }

        [[nodiscard]] static constexpr UInt128 BitRotateRight(UInt128 value, size_t bits)
        {
            return BitOr(
                BitShiftRightZeroExtend(value, bits),
                BitShiftLeft(value, 128 - bits));
        }

        [[nodiscard]] static constexpr UInt128 BitShiftLeft(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const upper = value._lower << (shift - 64);
                return UInt128{upper, 0};
            }

            uint64_t const lower = value._lower << shift;
            uint64_t const upper = (value._upper << shift) | (value._lower >> (64 - shift));
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 BitShiftRightZeroExtend(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = value._upper >> (shift - 64);
                return UInt128{0, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = value._upper >> shift;
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 BitShiftRightSignExtend(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> (shift - 64));
                uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> 63);
                return UInt128{upper, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> shift);
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 ByteSwap(UInt128 value)
        {
            uint64_t const lower = std::byteswap(value._upper);
            uint64_t const upper = std::byteswap(value._lower);
            return UInt128{upper, lower};
        }

    public:
        [[nodiscard]] constexpr UInt128 operator~() const
        {
            return BitCompl(*this);
        }

        [[nodiscard]] constexpr UInt128 operator&(UInt128 right) const
        {
            return BitAnd(*this, right);
        }

        [[nodiscard]] constexpr UInt128 operator|(UInt128 right) const
        {
            return BitOr(*this, right);
        }

        [[nodiscard]] constexpr UInt128 operator^(UInt128 right) const
        {
            return BitXor(*this, right);
        }

        static inline constexpr signed char g_character_to_digit_mapping[] = {
            // clang-format off
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // [0-9]
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [A-Z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [a-z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            // clang-format on
        };
        static_assert(sizeof(g_character_to_digit_mapping) == 256);

        [[nodiscard]] static constexpr unsigned char digit_from_char(char ch) noexcept
        {
            return static_cast<unsigned char>(g_character_to_digit_mapping[static_cast<unsigned char>(ch)]);
        }

        static constexpr bool TryParse(UInt128& result, std::string_view value, unsigned radix = 10)
        {
            UInt128 cutoff;
            UInt128 cutlim;

            UInt128 const ubase{0, radix};
            CheckedDivide(cutoff, cutlim, Max(), ubase);

            UInt128 temp{};

            bool overflow = false;

            for (char const c : value)
            {
                UInt128 const digit{0, digit_from_char(c)};

                if (digit._lower >= ubase._lower)
                {
                    break;
                }

                if (int const r = Compare(temp, cutoff); (r < 0) || ((r == 0) && (cutlim >= digit)))
                {
                    temp = (temp * ubase) + digit;
                }
                else
                {
                    overflow = true;
                }
            }

            result = temp;
            return !overflow;
        }

        static constexpr std::optional<UInt128> Parse(std::string_view value, unsigned radix = 10)
        {
            UInt128 result;
            return TryParse(result, value, radix) ? std::optional{result} : std::nullopt;
        }

        static std::string ToStringHex(UInt128 value)
        {

            std::string result{};

            if (value.IsZero())
            {
                result.push_back('0');
            }
            else
            {
                while (not value.IsZero())
                {
                    constexpr char const* const digits = "0123456789ABCDEF";
                    result.push_back(digits[value._lower & 0xF]);
                    value = BitShiftRightZeroExtend(value, 4);
                }

                std::reverse(result.begin(), result.end());
            }

            return result;
        }

        static std::string ToString(UInt128 value)
        {
            // ReSharper disable once CppTooWideScope
            constexpr char const* const digits = "0123456789";

            constexpr UInt128 radix{0, 10};

            std::string result{};

            if (value.IsZero())
            {
                result.push_back('0');
            }
            else
            {
                UInt128 digit{0, 0};

                while (not value.IsZero())
                {
                    CheckedDivide(value, digit, value, radix);
                    result.push_back(digits[digit._lower]);
                }

                std::reverse(result.begin(), result.end());
            }

            return result;
        }

    public:
        static constexpr double ToDouble(UInt128 value)
        {
            // Algorithm borrowed from dotnet runtime.
            constexpr uint64_t TwoPow52Bits = 0x4330000000000000;
            constexpr uint64_t TwoPow76Bits = 0x44B0000000000000;
            constexpr uint64_t TwoPow104Bits = 0x4670000000000000;
            constexpr uint64_t TwoPow128Bits = 0x47F0000000000000;

            constexpr double TwoPow52 = std::bit_cast<double>(TwoPow52Bits);
            constexpr double TwoPow76 = std::bit_cast<double>(TwoPow76Bits);
            constexpr double TwoPow104 = std::bit_cast<double>(TwoPow104Bits);
            constexpr double TwoPow128 = std::bit_cast<double>(TwoPow128Bits);

            if (value._upper == 0)
            {
                return static_cast<double>(value._lower);
            }

            if ((value._upper >> 24) == 0) // value < (2^104)
            {
                double const lower = std::bit_cast<double>(TwoPow52Bits | (value._lower << 12) >> 12) - TwoPow52;
                double const upper = std::bit_cast<double>(TwoPow104Bits | BitShiftRightZeroExtend(value, 52).ToUInt64()) - TwoPow104;

                return lower + upper;
            }

            double const lower = std::bit_cast<double>(TwoPow76Bits | (BitShiftRightZeroExtend(value, 12).ToUInt64() >> 12) | (value._lower & 0xFFFFFF)) - TwoPow76;
            double const upper = std::bit_cast<double>(TwoPow128Bits | BitShiftRightZeroExtend(value, 76).ToUInt64()) - TwoPow128;

            return lower + upper;
        }

        static constexpr float ToFloat(UInt128 value)
        {
            return static_cast<float>(ToDouble(value));
        }

        static UInt128 FromDouble(double value)
        {
            AE_ASSERT(std::isfinite(value) && (value > static_cast<double>(-1)) && (value < std::ldexp(static_cast<double>(1), 128)));

            if (value >= std::ldexp(static_cast<double>(1), 64))
            {
                uint64_t const upper = static_cast<uint64_t>(std::ldexp(value, -64));
                uint64_t const lower = static_cast<uint64_t>(value - std::ldexp(static_cast<double>(upper), 64));

                return UInt128{upper, lower};
            }

            return UInt128{0, static_cast<uint64_t>(value)};
        }

        static UInt128 FromFloat(float value)
        {
            AE_ASSERT(std::isfinite(value) && (value > static_cast<double>(-1)) && (value < std::ldexp(static_cast<double>(1), 128)));

            if (value >= std::ldexp(static_cast<double>(1), 64))
            {
                uint64_t const upper = static_cast<uint64_t>(std::ldexp(value, -64));
                uint64_t const lower = static_cast<uint64_t>(value - std::ldexp(static_cast<double>(upper), 64));

                return UInt128{upper, lower};
            }

            return UInt128{0, static_cast<uint64_t>(value)};
        }
    };
}

namespace Anemone
{
    struct Int128
    {
    private:
        uint64_t _lower;
        uint64_t _upper;

    public:
        constexpr Int128()
            : _lower{}
            , _upper{}
        {
        }

        constexpr Int128(uint64_t upper, uint64_t lower)
            : _lower{lower}
            , _upper{upper}
        {
        }

    public:
        static constexpr Int128 Make(signed long long value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed long value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed int value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed short value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed char value)
        {
            int64_t const lower = static_cast<int>(value);
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(unsigned long long value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned long value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned int value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned short value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned char value)
        {
            return Int128{0, value};
        }

    public:
        constexpr uint64_t GetLower() const
        {
            return this->_lower;
        }

        constexpr uint64_t GetUpper() const
        {
            return this->_upper;
        }

        constexpr uint64_t ToUInt64() const
        {
            return this->_lower;
        }

        constexpr int64_t ToInt64() const
        {
            return static_cast<int64_t>(this->_lower);
        }

        constexpr std::optional<int64_t> TryToInt64() const
        {
            if (this->_upper == 0)
            {
                return static_cast<int64_t>(this->_lower);
            }

            return std::nullopt;
        }

    public:
        static constexpr Int128 Min()
        {
            return Int128{0x8000000000000000, 0x0000000000000000};
        }

        static constexpr Int128 Max()
        {
            return Int128{0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
        }

    public:
        static constexpr Int128 UncheckedAdd(Int128 left, Int128 right)
        {
            uint64_t const lower = left._lower + right._lower;
            uint64_t const carry = (lower < left._lower) ? 1u : 0u;
            uint64_t const upper = left._upper + right._upper + carry;
            return Int128{upper, lower};
        }

        static constexpr bool CheckedAdd(Int128& result, Int128 left, Int128 right)
        {
            result = UncheckedAdd(left, right);

            uint32_t const sign = static_cast<uint32_t>(left._upper >> 63);

            if (sign == static_cast<uint32_t>(right._upper >> 63))
            {
                if (sign != static_cast<uint32_t>(result._upper >> 63))
                {
                    return true;
                }
            }

            return false;
        }

        static constexpr Int128 UncheckedSubtract(Int128 left, Int128 right)
        {
            uint64_t const lower = left._lower - right._lower;
            uint64_t const borrow = (lower > left._lower) ? 1u : 0u;
            uint64_t const upper = left._upper - right._upper - borrow;
            return Int128{upper, lower};
        }

        static constexpr bool CheckedSubtract(Int128& result, Int128 left, Int128 right)
        {
            result = UncheckedSubtract(left, right);

            uint32_t const sign = static_cast<uint32_t>(left._upper >> 63);

            if (sign != static_cast<uint32_t>(right._upper >> 63))
            {
                if (sign != static_cast<uint32_t>(result._upper >> 63))
                {
                    return true;
                }
            }

            return false;
        }

        static constexpr Int128 UncheckedMultiply(Int128 left, Int128 right)
        {
            uint64_t lower;
            uint64_t upper = Private::MultiplyHigh(left._lower, right._lower, lower);
            upper += (left._lower * right._upper) + (left._upper * right._lower);
            return Int128{upper, lower};
        }

        static constexpr bool CheckedMultiply(Int128& result, Int128 left, Int128 right)
        {
            Int128 const upper = MultiplyHigh(left, right, result);

            return (!upper.IsZero() || result.IsNegative()) && (upper.IsZero() || !result.IsNegative());
        }

        static constexpr bool CheckedDivide(Int128& quotient, Int128& remainder, Int128 left, Int128 right)
        {
            if (((right._upper == UINT64_MAX) && (right._lower == UINT64_MAX)) && ((left._upper == 0x8000'0000'0000'0000) && (left._lower == 0)))
            {
                return true;
            }

            uint64_t const sign = (left._upper ^ right._upper) & uint64_t{1} << 63;

            bool const left_negative = left.IsNegative();

            if (left_negative)
            {
                left = UncheckedAdd(BitCompl(left), Int128::Make(1u));
            }

            if (right.IsNegative())
            {
                right = UncheckedAdd(BitCompl(right), Int128::Make(1));
            }

            UInt128 r;
            UInt128 q;

            if (UInt128::CheckedDivide(q, r, UInt128{left.GetUpper(), left.GetLower()}, UInt128{right.GetUpper(), right.GetLower()}))
            {
                return true;
            }

            if (sign != 0)
            {
                q = UInt128::Add(UInt128::BitCompl(q), UInt128::Make(1));
            }

            if (left_negative)
            {
                r = UInt128::Add(UInt128::BitCompl(r), UInt128::Make(1));
            }

            quotient._lower = q.GetLower();
            quotient._upper = q.GetUpper();
            remainder._lower = r.GetLower();
            remainder._upper = r.GetUpper();
            return false;
        }

        constexpr Int128 operator-() const
        {
            return Int128{0, 0} - *this;
        }

        constexpr Int128 operator-(Int128 const& right) const
        {
            return UncheckedSubtract(*this, right);
        }

        constexpr Int128 operator+() const
        {
            return *this;
        }

        constexpr Int128 operator+(Int128 const& right) const
        {
            return UncheckedAdd(*this, right);
        }

        constexpr Int128 operator*(Int128 const& right) const
        {
            return UncheckedMultiply(*this, right);
        }

        constexpr Int128 operator/(Int128 const& right) const
        {
            Int128 q;
            Int128 r;
            CheckedDivide(q, r, *this, right);
            return q;
        }

        constexpr Int128 operator%(Int128 const& right) const
        {
            Int128 q;
            Int128 r;
            CheckedDivide(q, r, *this, right);
            return r;
        }

        constexpr Int128& operator+=(Int128 const& right)
        {
            *this = UncheckedAdd(*this, right);
            return *this;
        }

        constexpr Int128& operator-=(Int128 const& right)
        {
            *this = UncheckedSubtract(*this, right);
            return *this;
        }

        constexpr Int128& operator*=(Int128 const& right)
        {
            *this = UncheckedMultiply(*this, right);
            return *this;
        }

        constexpr Int128& operator/=(Int128 const& right)
        {
            Int128 remainder;
            CheckedDivide(*this, remainder, *this, right);
            return *this;
        }

        constexpr Int128& operator%=(Int128 const& right)
        {
            Int128 quotient;
            CheckedDivide(quotient, *this, *this, right);
            return *this;
        }


    public:
        static inline constexpr signed char g_character_to_digit_mapping[] = {
            // clang-format off
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // [0-9]
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [A-Z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [a-z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            // clang-format on
        };
        static_assert(sizeof(g_character_to_digit_mapping) == 256);

        [[nodiscard]] static constexpr unsigned char digit_from_char(char ch) noexcept
        {
            return static_cast<unsigned char>(g_character_to_digit_mapping[static_cast<unsigned char>(ch)]);
        }

        [[nodiscard]] static constexpr bool TryParse(Int128& result, std::string_view value, unsigned radix = 10)
        {
            bool negative = false;

            auto next = value.begin();
            auto const last = value.end();

            if (next != last && *next == '-')
            {
                negative = true;
                ++next;
            }

            constexpr UInt128 umax = UInt128::Max();
            constexpr UInt128 smax = UInt128::BitShiftRightZeroExtend(umax, 1);
            constexpr UInt128 absSmin = UInt128::Add(smax, UInt128::Make(1));

            UInt128 cutoff;
            UInt128 cutlim;

            UInt128 const ubase = UInt128::Make(radix);

            if (negative)
            {
                cutoff = UInt128::Divide(cutlim, absSmin, ubase);
            }
            else
            {
                cutoff = UInt128::Divide(cutlim, umax, ubase);
            }

            UInt128 temp{};

            bool overflow = false;

            for (; next != last; ++next)
            {
                UInt128 const digit{0, digit_from_char(*next)};

                if (digit.GetLower() >= ubase.GetLower())
                {
                    break;
                }

                if (int const r = UInt128::Compare(temp, cutoff); (r < 0) || ((r == 0) && (cutlim >= digit)))
                {
                    temp = (temp * ubase) + digit;
                }
                else
                {
                    overflow = true;
                }
            }

            if (negative)
            {
                temp = UInt128::Subtract(UInt128{}, temp);
            }

            if ((next != last) || overflow)
            {
                return false;
            }

            result._lower = temp.GetLower();
            result._upper = temp.GetUpper();
            return true;
        }

        [[nodiscard]] static constexpr std::optional<Int128> Parse(std::string_view value, unsigned radix = 10)
        {
            Int128 result{};

            if (TryParse(result, value, radix))
            {
                return result;
            }

            return {};
        }

        static constexpr char g_digit_to_char_mapping[36] = {
            // clang-format off
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
            'u', 'v', 'w', 'x', 'y', 'z'
            // clang-format on
        };

        static_assert(sizeof(g_digit_to_char_mapping) == 36);

        static std::string ToString(Int128 value, unsigned radix = 10)
        {
            AE_ASSERT((radix >= 2) && (radix <= 36));
            UInt128 uvalue{value.GetUpper(), value.GetLower()};
            UInt128 ubase{0, radix};

            std::string result{};

            if (value.IsNegative())
            {
                result.push_back('-');

                uvalue = UInt128::Subtract(UInt128::Make(0), uvalue);
            }

            constexpr size_t buffer_size = 128 + 2;

            char buffer[buffer_size];
            char* const buffer_end = buffer + buffer_size;
            char* rend = buffer_end;

            switch (radix)
            {
            case 10:
                {
                    UInt128 digit{};
                    do
                    {
                        uvalue = UInt128::Divide(digit, uvalue, ubase);
                        (*--rend) = static_cast<char>('0' + digit.GetLower());
                    } while (!uvalue.IsZero());
                    break;
                }

            case 2:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b1));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 1);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 4:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b11));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 2);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 8:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b111));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 3);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 16:
                {
                    do
                    {
                        (*--rend) = g_digit_to_char_mapping[uvalue.GetLower() & 0b1111];
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 4);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 32:
                {
                    do
                    {
                        (*--rend) = g_digit_to_char_mapping[uvalue.GetLower() & 0b11111];
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 5);
                    } while (!uvalue.IsZero());
                    break;
                }

            default:
                {
                    UInt128 digit{};
                    do
                    {
                        uvalue = UInt128::Divide(digit, uvalue, ubase);
                        (*--rend) = g_digit_to_char_mapping[digit.GetLower()];
                    } while (!uvalue.IsZero());
                    break;
                }
            }

            result.append(rend, buffer_end);
            return result;
        }

    public:
        static constexpr Int128 MultiplyHigh(Int128 left, Int128 right, Int128& lower)
        {
            UInt128 ulower;
            UInt128 const upper = UInt128::MultiplyHigh(
                UInt128{
                    left.GetUpper(),
                    left.GetLower(),
                },
                UInt128{
                    right.GetUpper(),
                    right.GetLower(),
                },
                ulower);

            lower._lower = ulower.GetLower();
            lower._upper = ulower.GetUpper();

            return Int128{upper.GetUpper(), upper.GetLower()} - BitAnd(BitShiftRightSignExtend(left, 127), right) - BitAnd(BitShiftRightSignExtend(right, 127), left);
        }

        [[nodiscard]] constexpr bool IsNegative() const
        {
            return static_cast<int64_t>(this->_upper) < 0;
        }

        [[nodiscard]] constexpr bool IsZero() const
        {
            return (this->_lower | this->_upper) == 0;
        }

    public:
        [[nodiscard]] static constexpr Int128 BitCompl(Int128 right)
        {
            return Int128{
                ~right._upper,
                ~right._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitAnd(Int128 left, Int128 right)
        {
            return Int128{
                left._upper & right._upper,
                left._lower & right._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitOr(Int128 left, Int128 right)
        {
            return Int128{
                left._upper | right._upper,
                left._lower | right._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitXor(Int128 left, Int128 right)
        {
            return Int128{
                left._upper ^ right._upper,
                left._lower ^ right._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitRotateLeft(Int128 value, size_t bits)
        {
            return BitOr(
                BitShiftLeft(value, bits),
                BitShiftRightZeroExtend(value, 128 - bits));
        }

        [[nodiscard]] static constexpr Int128 BitRotateRight(Int128 value, size_t bits)
        {
            return BitOr(
                BitShiftRightZeroExtend(value, bits),
                BitShiftLeft(value, 128 - bits));
        }

        [[nodiscard]] static constexpr Int128 BitShiftLeft(Int128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const upper = value._lower << (shift - 64);
                return Int128{upper, 0};
            }

            uint64_t const lower = value._lower << shift;
            uint64_t const upper = (value._upper << shift) | (value._lower >> (64 - shift));
            return Int128{upper, lower};
        }

        [[nodiscard]] static constexpr Int128 BitShiftRightZeroExtend(Int128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = value._upper >> (shift - 64);
                return Int128{0, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = value._upper >> shift;
            return Int128{upper, lower};
        }

        [[nodiscard]] static constexpr Int128 BitShiftRightSignExtend(Int128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> (shift - 64));
                uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> 63);
                return Int128{upper, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> shift);
            return Int128{upper, lower};
        }

    public:
        [[nodiscard]] static constexpr int Compare(Int128 left, Int128 right)
        {
            if (left < right)
            {
                return -1;
            }

            if (left > right)
            {
                return 1;
            }

            return 0;
        }

        [[nodiscard]] static constexpr bool Equals(Int128 left, Int128 right)
        {
            return left == right;
        }

        [[nodiscard]] friend constexpr bool operator<(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower < right._lower));
            }

            return left.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator<=(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower <= right._lower));
            }

            return left.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator>(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower > right._lower));
            }

            return right.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator>=(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower >= right._lower));
            }

            return right.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator==(Int128 const& left, Int128 const& right)
        {
            return (left._lower == right._lower) && (left._upper == right._upper);
        }

        [[nodiscard]] friend constexpr bool operator!=(Int128 const& left, Int128 const& right)
        {
            return (left._lower != right._lower) || (left._upper != right._upper);
        }

    public:
        [[nodiscard]] static constexpr double ToDouble(Int128 value)
        {
            if (value.IsNegative())
            {
                value = -value;
                return -UInt128::ToDouble(UInt128{value._upper, value._lower});
            }

            return UInt128::ToDouble(UInt128{value._upper, value._lower});
        }

        [[nodiscard]] static constexpr Int128 FromDouble(double value)
        {
            bool const negative = value < 0.0;

            if (negative)
            {
                value = -value;
            }

            if (value >= 1.0)
            {
                uint64_t const bits = std::bit_cast<uint64_t>(value);
                Int128 result{
                    (bits << 12) >> 1 | 0x8000'0000'0000'0000,
                    0x0000'0000'0000'0000,
                };

                result = BitShiftRightSignExtend(result, 1023 + 128 - 1 - static_cast<int>(bits >> 52));

                if (negative)
                {
                    result = -result;
                }

                return result;
            }

            return Int128{0, 0};
        }

        [[nodiscard]] static constexpr float ToFloat(Int128 value)
        {
            return static_cast<float>(ToDouble(value));
        }
    };
}
