#pragma once
#include <bit>
#include <limits>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <algorithm>
#include <climits>

namespace Anemone::Bitwise
{
    template <typename T>
    constexpr T AlignUp(T size, T alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    template <typename T>
    constexpr T AlignDown(T size, T alignment)
    {
        return size & ~(alignment - 1);
    }

    template <typename T>
    constexpr bool IsAligned(T size, T alignment)
    {
        return (size & (alignment - 1)) == 0;
    }

    constexpr std::byte* AlignUp(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return ptr + (aligned - address);
    }

    constexpr std::byte* AlignDown(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return ptr + (aligned - address);
    }

    constexpr bool IsAligned(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        return (address & (alignment - 1)) == 0;
    }

    constexpr void* AlignUp(void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
    }

    constexpr void* AlignDown(void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
    }

    constexpr bool IsAligned(const void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        return (address & (alignment - 1)) == 0;
    }


    template <typename T, typename U>
    [[nodiscard]] bool Disjoint(T const* pt, size_t nt, U const* pu, size_t nu) noexcept
    {
        intptr_t const bt = reinterpret_cast<intptr_t>(pt);
        intptr_t const et = reinterpret_cast<intptr_t>(pt + nt);
        intptr_t const bu = reinterpret_cast<intptr_t>(pu);
        intptr_t const eu = reinterpret_cast<intptr_t>(pu + nu);

        return (et <= bu) || (eu <= bt);
    }

    template <typename T, typename U>
    [[nodiscard]] bool Disjoint(T const* pt, size_t nt, U const& u) noexcept
    {
        return Disjoint<T, U>(pt, nt, std::addressof(u), 1);
    }

    template <typename T, typename U>
    [[nodiscard]] bool Disjoint(T const& t, U const& u) noexcept
    {
        return Disjoint<T, U>(std::addressof(t), 1, std::addressof(u), 1);
    }

    template <typename T>
    [[nodiscard]] constexpr T DivideAndRoundUp(T dividend, T divisor)
    {
        return (dividend + divisor - 1) / divisor;
    }

    template <typename T>
    [[nodiscard]] constexpr T DivideAndRoundDown(T dividend, T divisor)
    {
        return dividend / divisor;
    }
}

namespace Anemone::Bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T BitRotateRight(T value, int bits)
        requires(std::is_unsigned_v<T>);

    template <typename T>
    [[nodiscard]] constexpr T BitRotateLeft(T value, int bits)
        requires(std::is_unsigned_v<T>)
    {
        unsigned int const digits = std::numeric_limits<T>::digits;

        bits = bits % digits;

        if (!bits)
        {
            return value;
        }

        if (bits < 0)
        {
            return BitRotateRight(value, -bits);
        }

        return (value << bits) | (value >> (digits - bits));
    }

    template <typename T>
    [[nodiscard]] constexpr T BitRotateRight(T value, int bits)
        requires(std::is_unsigned_v<T>)
    {
        unsigned int const digits = std::numeric_limits<T>::digits;

        bits = bits % digits;

        if (!bits)
        {
            return value;
        }

        if (bits < 0)
        {
            return BitRotateLeft(value, -bits);
        }

        return (value >> bits) | (value << (digits - bits));
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountLeadingZeros(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countl_zero(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountLeadingOnes(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countl_one(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountTrailingZeros(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countr_zero(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountTrailingOnes(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countr_one(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCount(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::popcount(value);
    }

    template <typename T>
    [[nodiscard]] constexpr size_t BitWidth(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::bit_width(value);
    }

    template <typename T>
    [[nodiscard]] constexpr bool IsPowerOf2(T value)
        requires(std::is_unsigned_v<T>)
    {
        return (value != 0) && ((value & (value - 1)) == 0);
    }

    template <typename T>
    [[nodiscard]] constexpr T BitCeil(T value)
        requires(std::is_unsigned_v<T>)
    {
        if (value == 0)
        {
            return 1;
        }

        return static_cast<T>(T{1} << (std::numeric_limits<T>::digits - BitCountLeadingZeros(static_cast<T>(value - 1))));
    }

    template <typename T>
    [[nodiscard]] constexpr T BitFloor(T value)
        requires(std::is_unsigned_v<T>)
    {
        if (value == 0)
        {
            return 0;
        }

        return static_cast<T>(T{1} << (std::numeric_limits<T>::digits - 1 - BitCountLeadingZeros(value)));
    }
}

namespace Anemone::Bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T ToBigEndian(T value)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap(value);
        }
        else
        {
            return value;
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T ToLittleEndian(T value)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return std::byteswap(value);
        }
        else
        {
            return value;
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T HostToNetwork(T value)
        requires(std::is_integral_v<T>)
    {
        return ToBigEndian(value);
    }

            template <typename T>
    [[nodiscard]] constexpr T NetworkToHost(T value)
        requires(std::is_integral_v<T>)
    {
        return ToBigEndian(value);
    }
}

namespace Anemone::Bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T LoadUnaligned(void const* source)
        requires(std::is_trivially_copyable_v<T>)
    {
        struct Storage
        {
            std::byte elements[sizeof(T)];
        };
        return std::bit_cast<T>(*static_cast<Storage const*>(source));
    }

    template <typename T>
    constexpr void StoreUnaligned(void* destination, T value)
        requires(std::is_trivially_copyable_v<T>)
    {
        struct Storage
        {
            std::byte elements[sizeof(T)];
        };

        (*static_cast<Storage*>(destination)) = std::bit_cast<Storage>(value);
    }
}

namespace Anemone::Bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T LoadUnalignedLittleEndian(void const* source)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return LoadUnaligned<T>(source);
        }
        else
        {
            return std::byteswap(LoadUnaligned<T>(source));
        }
    }

    template <typename T>
    constexpr void StoreUnalignedLittleEndian(void* destination, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreUnaligned<T>(destination, value);
        }
        else
        {
            StoreUnaligned<T>(destination, std::byteswap(value));
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T LoadUnalignedBigEndian(void const* source)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return LoadUnaligned<T>(source);
        }
        else
        {
            return std::byteswap(LoadUnaligned<T>(source));
        }
    }

    template <typename T>
    constexpr void StoreUnalignedBigEndian(void* destination, T value)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            StoreUnaligned<T>(destination, value);
        }
        else
        {
            StoreUnaligned<T>(destination, std::byteswap(value));
        }
    }
}

namespace Anemone::Bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T SignExtend(T value, int bits)
    {
        using U = std::make_unsigned_t<T>;
        size_t const shift = std::numeric_limits<T>::digits - bits;
        return static_cast<T>(static_cast<U>(value << shift) >> shift);
    }

    template <typename T>
    [[nodiscard]] constexpr T ZeroExtend(T value, int bits)
    {
        using U = std::make_unsigned_t<T>;
        size_t const shift = std::numeric_limits<T>::digits - bits;
        return static_cast<T>(static_cast<U>(value << shift) >> shift);
    }

    template <size_t HighBit, size_t LowBit, typename ResultType, typename ValueType>
    constexpr ResultType ExtractBits(ValueType value) noexcept
    {
        constexpr size_t bits_count = HighBit + 1 - LowBit;
        static_assert(HighBit >= LowBit);
        static_assert(HighBit < (sizeof(ValueType) * std::numeric_limits<std::byte>::digits));
        static_assert(bits_count <= (sizeof(ResultType) * std::numeric_limits<std::byte>::digits));

        ValueType const mask = (ValueType{1} << bits_count) - 1;
        return static_cast<ResultType>((value >> LowBit) & mask);
    }

    template <size_t Bit, typename ResultType, typename ValueType>
    constexpr ResultType extract_bit(ValueType value) noexcept
    {
        return extract_bits<Bit, Bit, ResultType, ValueType>(value);
    }
}

namespace Anemone::Bitwise
{
    struct TypeBuilder
    {
        size_t Size{};
        size_t Alignment{};

        constexpr void Add(size_t size, size_t alignment)
        {
            size_t const aligned = AlignUp(this->Size, alignment);
            this->Size = aligned + size;
            this->Alignment = std::max(this->Alignment, alignment);
        }
    };
}

namespace Anemone::Bitwise
{
    // Note: 64 bit integer division requires a 128 bit multiplication
    struct FastDivision final
    {
        uint32_t Multiplier;
        size_t Shift;

        static constexpr FastDivision Create(uint32_t divisor)
        {
            uint32_t const log2_divisor = static_cast<uint32_t>(std::countl_zero(divisor) ^ 31);

            if (std::has_single_bit(divisor))
            {
                return {1, log2_divisor};
            }
            else
            {
                uint32_t const width = log2_divisor + 32;
                uint64_t const max = uint64_t{1} << width;
                uint32_t const quotient = static_cast<uint32_t>(max / divisor);
                return {quotient + 1, width};
            }
        }

        [[nodiscard]] constexpr uint32_t Divide(uint32_t value) const noexcept
        {
            return static_cast<uint32_t>((static_cast<uint64_t>(value) * this->Multiplier) >> this->Shift);
        }
    };
}

namespace Anemone::BitwiseV2
{
    template <typename T>
    constexpr T shll(T x, int s)
    {
        return T(std::make_unsigned_t<T>(x) << s);
    }

    template <typename T>
    constexpr T shlr(T x, int s)
    {
        return T(std::make_unsigned_t<T>(x) >> s);
    }

    template <typename T>
    constexpr T shal(T x, int s)
    {
        return shll(x, s);
    }

    template <typename T>
    constexpr T shar(T x, int s)
    {
        return T(std::make_signed_t<T>(x) >> s);
    }

    template <typename T>
    constexpr T rotl(T x, int s)
    {
        return (x << s) | shlr(x, (sizeof(T) * CHAR_BIT - s));
    }

    template <typename T>
    constexpr T rotr(T x, int s)
    {
        return shlr(x, s) | (x << (sizeof(T) * CHAR_BIT - s));
    }

    // count trailing zeros
    template <typename T>
    constexpr int cntt0(T x)
    {
        constexpr int nbits = int(sizeof(T) * CHAR_BIT);

        if (x == 0)
        {
            return nbits;
        }

        T n = 0;

        if (sizeof(T) > 1)
        {
            if (sizeof(T) > 2)
            {
                if (sizeof(T) > 4)
                {
                    if ((x & T(0xFFFFFFFFUL)) == 0)
                    {
                        n = n + 32;
                        x = shlr(x, 32);
                    }
                }

                if ((x & T(0xFFFFUL)) == 0)
                {
                    n = n + 16;
                    x = shlr(x, 16);
                }
            }

            if ((x & T(0xFFUL)) == 0)
            {
                n = n + 8;
                x = shlr(x, 8);
            }
        }

        if ((x & T(0xFUL)) == 0)
        {
            n = n + 4;
            x = shlr(x, 4);
        }

        if ((x & T(0x3UL)) == 0)
        {
            n = n + 2;
            x = shlr(x, 2);
        }

        return static_cast<int>(n - (x & 1));
    }

    template <typename T>
    constexpr int cntl0(T x)
    {
        constexpr int nbits = int(sizeof(T) * CHAR_BIT);

        if (x == 0)
        {
            return nbits;
        }

        T n = 1;

        if (sizeof(T) > 1)
        {
            if (sizeof(T) > 2)
            {
                if (sizeof(T) > 4)
                {
                    if ((shlr(x, nbits - 32)) == 0)
                    {
                        n = n + 32;
                        x = x << 32;
                    }
                }

                if ((shlr(x, nbits - 16)) == 0)
                {
                    n = n + 16;
                    x = x << 16;
                }
            }

            if ((shlr(x, nbits - 8)) == 0)
            {
                n = n + 8;
                x = x << 8;
            }
        }

        if ((shlr(x, nbits - 4)) == 0)
        {
            n = n + 4;
            x = x << 4;
        }

        if ((shlr(x, nbits - 2)) == 0)
        {
            n = n + 2;
            x = x << 2;
        }

        n = n - (shlr(x, 31));

        return static_cast<int>(n);
    }

    template <typename T>
    constexpr int cntl1(T x)
    {
        return cntl0(~x);
    }

    template <typename T>
    constexpr int cntt1(T x)
    {
        return cntt0(~x);
    }

    template <typename T>
    constexpr int parity(T x)
    {
        x = x ^ shlr(x, 1);
        x = x ^ shlr(x, 2);
        x = x ^ shlr(x, 4);

        if (sizeof(T) > 1)
        {
            x = x ^ shlr(x, 8);

            if (sizeof(T) > 2)
            {
                x = x ^ shlr(x, 16);

                if (sizeof(T) > 4)
                {
                    x = x ^ shlr(x, 32);
                }
            }
        }

        return x;
    }
    template <typename T>
    constexpr int popcount(T x)
    {
        x = (x & T(0x5555555555555555UL)) + (shlr(x, 1) & T(0x5555555555555555UL));
        x = (x & T(0x3333333333333333UL)) + (shlr(x, 2) & T(0x3333333333333333UL));
        x = (x & T(0x0F0F0F0F0F0F0F0FUL)) + (shlr(x, 4) & T(0x0F0F0F0F0F0F0F0FUL));

        if (sizeof(T) > 1)
        {
            x = (x & T(0x00FF00FF00FF00FFUL)) + (shlr(x, 8) & T(0x00FF00FF00FF00FFUL));

            if (sizeof(T) > 2)
            {
                x = (x & T(0x0000FFFF0000FFFFUL)) + (shlr(x, 16) & T(0x0000FFFF0000FFFFUL));

                if (sizeof(T) > 4)
                {
                    x = (x & T(0x00000000FFFFFFFFUL)) + (shlr(x, 32) & T(0x00000000FFFFFFFFUL));
                }
            }
        }

        return x;
    }

    template <typename T>
    constexpr T rstls1b(T x)
    {
        return x & (x - 1);
    }

    template <typename T>
    constexpr T setls0b(T x)
    {
        return x | (x + 1);
    }

    template <typename T>
    constexpr T isols1b(T x)
    {
        return x & -x;
    }

    template <typename T>
    constexpr T isols0b(T x)
    {
        return (~x) & (x + 1);
    }

    template <typename T>
    constexpr T rstt1(T x)
    {
        return x & (x + 1);
    }

    template <typename T>
    constexpr T sett0(T x)
    {
        return x | (x - 1);
    }

    template <typename T>
    constexpr T maskt0(T x)
    {
        return (~x) & (x - 1);
    }

    template <typename T>
    constexpr T maskt1(T x)
    {
        return ~((~x) | (x + 1));
    }

    template <typename T>
    constexpr T maskt0ls1b(T x)
    {
        return (x - 1) ^ x;
    }

    template <typename T>
    constexpr T maskt1ls0b(T x)
    {
        return x ^ (x + 1);
    }

    template <typename T>
    constexpr T setbit(T x, int b)
    {
        return x | (T(1) << b);
    }

    template <typename T>
    constexpr T rstbit(T x, int b)
    {
        return x & ~(T(1) << b);
    }

    template <typename T>
    constexpr T flipbit(T x, int b)
    {
        return x ^ (T(1) << b);
    }

    template <typename T>
    constexpr bool testbit(T x, int b)
    {
        return x & (T(1) << b);
    }

    template <typename T>
    constexpr T rstbitsge(T x, int b)
    {
        return x & ((T(1) << b) - 1);
    }

    template <typename T>
    constexpr T rstbitsle(T x, int b)
    {
        return x & ~((T(1) << (b + 1)) - 1);
    }

    template <typename T>
    constexpr T setbitsge(T x, int b)
    {
        return x | ~((T(1) << b) - 1);
    }

    template <typename T>
    constexpr T setbitsle(T x, int b)
    {
        return x | ((T(1) << (b + 1)) - 1);
    }

    template <typename T>
    constexpr T flipbitsge(T x, int b)
    {
        return x ^ ~((T(1) << b) - 1);
    }

    template <typename T>
    constexpr T flipbitsle(T x, int b)
    {
        return x ^ ((T(1) << (b + 1)) - 1);
    }

    template <typename T>
    constexpr bool ispow2(T x)
    {
        return x > 0 && (x & (x - 1)) == 0;
    }

    template <typename T>
    constexpr T ceilp2(T x)
    {
        x = x - 1;
        x |= shlr(x, 1);
        x |= shlr(x, 2);
        x |= shlr(x, 4);

        if (sizeof(T) > 1)
        {
            x |= shlr(x, 8);

            if (sizeof(T) > 2)
            {
                x |= shlr(x, 16);

                if (sizeof(T) > 4)
                {
                    x |= shlr(x, 32);
                }
            }
        }

        return x + 1;
    }

    template <typename T>
    constexpr T floorp2(T x)
    {
        x |= shlr(x, 1);
        x |= shlr(x, 2);
        x |= shlr(x, 4);

        if (sizeof(T) > 1)
        {
            x |= shlr(x, 8);

            if (sizeof(T) > 2)
            {
                x |= shlr(x, 16);

                if (sizeof(T) > 4)
                {
                    x |= shlr(x, 32);
                }
            }
        }

        return x - shlr(x, 1);
    }

    template <typename T>
    constexpr bool is_aligned(T t, size_t a)
    {
        return ((t & (a - 1)) == 0);
    }

    inline bool is_aligned(void* t, size_t a)
    {
        return is_aligned(std::bit_cast<uintptr_t>(t), a);
    }

    template <typename T>
    constexpr T align_up(T val, size_t a)
    {
        return ((val + (a - 1)) & static_cast<size_t>(-static_cast<std::make_signed_t<size_t>>(a)));
    }

    inline void* align_up(void* val, size_t a)
    {
        return std::bit_cast<void*>(align_up(std::bit_cast<uintptr_t>(val), a));
    }

    template <typename T>
    constexpr T align_down(T val, size_t a)
    {
        return val & static_cast<size_t>(-static_cast<std::make_signed_t<size_t>>(a));
    }

    inline void* align_down(void* val, size_t a)
    {
        return std::bit_cast<void*>(align_down(std::bit_cast<uintptr_t>(val), a));
    }

    template <typename T>
    constexpr T outer_pshuffle(T x)
    {
        T t = 0;

        if (sizeof(T) > 4)
        {
            t = (x ^ shlr(x, 16)) & T(0x00000000FFFF0000UL);
            x = x ^ t ^ shll(t, 16);
        }

        if (sizeof(T) > 2)
        {
            t = (x ^ shlr(x, 8)) & T(0x0000FF000000FF00UL);
            x = x ^ t ^ shll(t, 8);
        }

        if (sizeof(T) > 1)
        {
            t = (x ^ shlr(x, 4)) & T(0x00F000F000F000F0UL);
            x = x ^ t ^ shll(t, 4);
        }

        t = (x ^ shlr(x, 2)) & T(0x0C0C0C0C0C0C0C0CUL);
        x = x ^ t ^ shll(t, 2);
        t = (x ^ shlr(x, 1)) & T(0x2222222222222222UL);
        x = x ^ t ^ shll(t, 1);

        return x;
    }

    template <typename T>
    constexpr T outer_punshuffle(T x)
    {
        T t = 0;
        t = (x ^ shlr(x, 1)) & T(0x2222222222222222UL);
        x = x ^ t ^ shll(t, 1);
        t = (x ^ shlr(x, 2)) & T(0x0C0C0C0C0C0C0C0CUL);
        x = x ^ t ^ shll(t, 2);

        if (sizeof(T) > 1)
        {
            t = (x ^ shlr(x, 4)) & T(0x00F000F000F000F0UL);
            x = x ^ t ^ shll(t, 4);
        }

        if (sizeof(T) > 2)
        {
            t = (x ^ shlr(x, 8)) & T(0x0000FF000000FF00UL);
            x = x ^ t ^ shll(t, 8);
        }

        if (sizeof(T) > 4)
        {
            t = (x ^ shlr(x, 16)) & T(0x00000000FFFF0000UL);
            x = x ^ t ^ shll(t, 16);
        }

        return x;
    }

    template <typename T>
    constexpr T inner_pshuffle(T x)
    {
        return outer_pshuffle(reverse_bits(x, sizeof(T) * CHAR_BIT / 2, 2));
    }

    template <typename T>
    constexpr T inner_punshuffle(T x)
    {
        return reverse_bits(outer_punshuffle(x), sizeof(T) * CHAR_BIT / 2, 2);
    }

    template <typename T>
    constexpr T deposit_bits(T x, T mask)
    {
        using S = std::make_signed_t<T>;

        T res = 0;

        for (T bb = 1; mask != 0; bb += bb)
        {
            if (x & bb)
            {
                res |= mask & static_cast<T>(-static_cast<S>(mask));
            }

            mask &= (mask - 1);
        }

        return res;
    }

    template <typename T>
    constexpr T extract_bits(T x, T mask)
    {
        using S = std::make_signed_t<T>;

        T res = 0;

        for (T bb = 1; mask != 0; bb += bb)
        {
            if (x & mask & static_cast<T>(-static_cast<S>(mask)))
            {
                res |= bb;
            }

            mask &= (mask - 1);
        }

        return res;
    }

    template <typename T>
    constexpr auto reverse_bits(T x, int subword_bits = 1, int group_subwords = 1)
        -> std::enable_if_t<std::is_unsigned_v<T>, T>
    {
        int group_sz = int(sizeof(T) * CHAR_BIT) / group_subwords;
        int k = group_sz - subword_bits;

        if (k & 1)
        {
            x = shll(x & T(0x5555555555555555UL), 1) | shlr(x & T(0xAAAAAAAAAAAAAAAAUL), 1);
        }

        if (k & 2)
        {
            x = shll(x & T(0x3333333333333333UL), 2) | shlr(x & T(0xCCCCCCCCCCCCCCCCUL), 2);
        }

        if (k & 4)
        {
            x = shll(x & T(0x0F0F0F0F0F0F0F0FUL), 4) | shlr(x & T(0xF0F0F0F0F0F0F0F0UL), 4);
        }

        if (sizeof(T) > 1 && k & 8)
        {
            x = shll(x & T(0x00FF00FF00FF00FFUL), 8) | shlr(x & T(0xFF00FF00FF00FF00UL), 8);
        }

        if (sizeof(T) > 2 && k & 16)
        {
            x = shll(x & T(0x0000FFFF0000FFFFUL), 16) | shlr(x & T(0xFFFF0000FFFF0000UL), 16);
        }

        if (sizeof(T) > 4 && k & 32)
        {
            x = shll(x & T(0x00000000FFFFFFFFUL), 32) | shlr(x & T(0xFFFFFFFF00000000UL), 32);
        }

        return x;
    }

    template <typename T>
    constexpr auto reverse_bits(T x, int subword_bits = 1, int group_subwords = 1)
        -> std::enable_if_t<std::is_signed_v<T>, T>
    {
        return T(reverse_bits(std::make_unsigned_t<T>(x), subword_bits, group_subwords));
    }

    template <typename T>
    constexpr T reverse_bytes(T x, int bytes_per_block = 1, int blocks_per_group = sizeof(T))
    {
        return reverse_bits(x, CHAR_BIT * bytes_per_block, blocks_per_group);
    }

}
