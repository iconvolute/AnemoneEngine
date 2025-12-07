#pragma once
#include <bit>
#include <limits>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <algorithm>
#include <climits>
#include <span>

namespace Anemone
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

    [[nodiscard]] constexpr std::byte* Adjust(
        std::byte* ptr,
        ptrdiff_t offset)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const adjusted = address + offset;
        return std::bit_cast<std::byte*>(adjusted);
    }

    [[nodiscard]] constexpr void* Adjust(
        void* ptr,
        ptrdiff_t offset)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const adjusted = address + offset;
        return std::bit_cast<void*>(adjusted);
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

namespace Anemone
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

    template <typename T>
    [[nodiscard]] constexpr T BitMsbToMask(T value)
        requires(std::is_unsigned_v<T>)
    {
        using S = std::make_signed_t<T>;
        return static_cast<T>(static_cast<S>(value) >> std::numeric_limits<S>::digits);
    }
}

namespace Anemone
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

namespace Anemone
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
    [[nodiscard]] constexpr T LoadAligned(void const* source)
        requires(std::is_trivially_copyable_v<T>)
    {
        return *static_cast<T const*>(source);
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

    template <typename T>
    constexpr void StoreAligned(void* destination, T value)
        requires(std::is_trivially_copyable_v<T>)
    {
        (*static_cast<T*>(destination)) = value;
    }
}

namespace Anemone
{
    template <typename T>
    [[nodiscard]] constexpr T UnsafeLoadUnalignedLittleEndian(void const* bytes)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return LoadUnaligned<T>(bytes);
        }
        else
        {
            return std::byteswap<T>(LoadUnaligned<T>(bytes));
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T UnsafeLoadAlignedLittleEndian(void const* bytes)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return LoadAligned<T>(bytes);
        }
        else
        {
            return std::byteswap<T>(LoadAligned<T>(bytes));
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T UnsafeLoadUnalignedBigEndian(void const* bytes)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap<T>(LoadUnaligned<T>(bytes));
        }
        else
        {
            return LoadUnaligned<T>(bytes);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T UnsafeLoadAlignedBigEndian(void const* bytes)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap<T>(LoadAligned<T>(bytes));
        }
        else
        {
            return LoadAligned<T>(bytes);
        }
    }

    template <typename T>
    constexpr void UnsafeStoreUnalignedLittleEndian(void* bytes, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreUnaligned<T>(bytes, value);
        }
        else
        {
            StoreUnaligned<T>(bytes, std::byteswap<T>(value));
        }
    }

    template <typename T>
    constexpr void UnsafeStoreAlignedLittleEndian(void* bytes, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreAligned<T>(bytes, value);
        }
        else
        {
            StoreAligned<T>(bytes, std::byteswap<T>(value));
        }
    }

    template <typename T>
    constexpr void UnsafeStoreUnalignedBigEndian(void* bytes, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreUnaligned<T>(bytes, std::byteswap<T>(value));
        }
        else
        {
            StoreUnaligned<T>(bytes, value);
        }
    }

    template <typename T>
    constexpr void UnsafeStoreAlignedBigEndian(void* bytes, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreAligned<T>(bytes, std::byteswap<T>(value));
        }
        else
        {
            StoreAligned<T>(bytes, value);
        }
    }
}

namespace Anemone
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

namespace Anemone
{
    struct MemoryLayout final
    {
        size_t Size{};
        size_t Alignment{};

        constexpr void AppendField(size_t size, size_t alignment)
        {
            size_t const aligned = AlignUp(this->Size, alignment);
            this->Size = aligned + size;
            this->Alignment = std::max(this->Alignment, alignment);
        }
    };
}

namespace Anemone
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

namespace Anemone
{
    template <typename T>
    constexpr T ShiftLogicalLeft(T x, int s)
    {
        return T(std::make_unsigned_t<T>(x) << s);
    }

    template <typename T>
    constexpr T ShiftLogicalRight(T x, int s)
    {
        return T(std::make_unsigned_t<T>(x) >> s);
    }

    template <typename T>
    constexpr T ShiftArithmeticLeft(T x, int s)
    {
        return ShiftLogicalLeft(x, s);
    }

    template <typename T>
    constexpr T ShiftArithmeticRight(T x, int s)
    {
        return T(std::make_signed_t<T>(x) >> s);
    }

    template <typename T>
    constexpr T RotateLeft(T x, int s)
    {
        return (x << s) | ShiftLogicalRight(x, (sizeof(T) * CHAR_BIT - s));
    }

    template <typename T>
    constexpr T RotateRight(T x, int s)
    {
        return ShiftLogicalRight(x, s) | (x << (sizeof(T) * CHAR_BIT - s));
    }

    // count trailing zeros
    template <typename T>
    constexpr int CountTrailingZeros(T x)
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
                        x = ShiftLogicalRight(x, 32);
                    }
                }

                if ((x & T(0xFFFFUL)) == 0)
                {
                    n = n + 16;
                    x = ShiftLogicalRight(x, 16);
                }
            }

            if ((x & T(0xFFUL)) == 0)
            {
                n = n + 8;
                x = ShiftLogicalRight(x, 8);
            }
        }

        if ((x & T(0xFUL)) == 0)
        {
            n = n + 4;
            x = ShiftLogicalRight(x, 4);
        }

        if ((x & T(0x3UL)) == 0)
        {
            n = n + 2;
            x = ShiftLogicalRight(x, 2);
        }

        return static_cast<int>(n - (x & 1));
    }

    template <typename T>
    constexpr int CountLeadingZeros(T x)
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
                    if ((ShiftLogicalRight(x, nbits - 32)) == 0)
                    {
                        n = n + 32;
                        x = x << 32;
                    }
                }

                if ((ShiftLogicalRight(x, nbits - 16)) == 0)
                {
                    n = n + 16;
                    x = x << 16;
                }
            }

            if ((ShiftLogicalRight(x, nbits - 8)) == 0)
            {
                n = n + 8;
                x = x << 8;
            }
        }

        if ((ShiftLogicalRight(x, nbits - 4)) == 0)
        {
            n = n + 4;
            x = x << 4;
        }

        if ((ShiftLogicalRight(x, nbits - 2)) == 0)
        {
            n = n + 2;
            x = x << 2;
        }

        n = n - (ShiftLogicalRight(x, 31));

        return static_cast<int>(n);
    }

    template <typename T>
    constexpr int CountLeadingOnes(T x)
    {
        return CountLeadingZeros(~x);
    }

    template <typename T>
    constexpr int CountTrailingOnes(T x)
    {
        return CountTrailingZeros(~x);
    }

    template <typename T>
    constexpr int Parity(T x)
    {
        x = x ^ ShiftLogicalRight(x, 1);
        x = x ^ ShiftLogicalRight(x, 2);
        x = x ^ ShiftLogicalRight(x, 4);

        if (sizeof(T) > 1)
        {
            x = x ^ ShiftLogicalRight(x, 8);

            if (sizeof(T) > 2)
            {
                x = x ^ ShiftLogicalRight(x, 16);

                if (sizeof(T) > 4)
                {
                    x = x ^ ShiftLogicalRight(x, 32);
                }
            }
        }

        return x;
    }

    template <typename T>
    constexpr int PopCount(T x)
    {
        x = (x & T(0x5555555555555555UL)) + (ShiftLogicalRight(x, 1) & T(0x5555555555555555UL));
        x = (x & T(0x3333333333333333UL)) + (ShiftLogicalRight(x, 2) & T(0x3333333333333333UL));
        x = (x & T(0x0F0F0F0F0F0F0F0FUL)) + (ShiftLogicalRight(x, 4) & T(0x0F0F0F0F0F0F0F0FUL));

        if (sizeof(T) > 1)
        {
            x = (x & T(0x00FF00FF00FF00FFUL)) + (ShiftLogicalRight(x, 8) & T(0x00FF00FF00FF00FFUL));

            if (sizeof(T) > 2)
            {
                x = (x & T(0x0000FFFF0000FFFFUL)) + (ShiftLogicalRight(x, 16) & T(0x0000FFFF0000FFFFUL));

                if (sizeof(T) > 4)
                {
                    x = (x & T(0x00000000FFFFFFFFUL)) + (ShiftLogicalRight(x, 32) & T(0x00000000FFFFFFFFUL));
                }
            }
        }

        return x;
    }

    template <typename T>
    constexpr T ResetLeastSignificantBit(T x)
    {
        return x & (x - 1);
    }

    template <typename T>
    constexpr T SetLeastSignificantBit(T x)
    {
        return x | (x + 1);
    }

    template <typename T>
    constexpr T IsolateLeastSignificantBit(T x)
    {
        return x & -x;
    }

    template <typename T>
    constexpr T isols0b(T x)
    {
        return (~x) & (x + 1);
    }

    template <typename T>
    constexpr T ResetTrailingOnes(T x)
    {
        return x & (x + 1);
    }

    template <typename T>
    constexpr T SetTrailingOnes(T x)
    {
        return x | (x - 1);
    }

    template <typename T>
    constexpr T MaskTrailingZeros(T x)
    {
        return (~x) & (x - 1);
    }

    template <typename T>
    constexpr T MaskTrailingOnes(T x)
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
    constexpr T SetBit(T x, int b)
    {
        return x | (T(1) << b);
    }

    template <typename T>
    constexpr T ResetBit(T x, int b)
    {
        return x & ~(T(1) << b);
    }

    template <typename T>
    constexpr T FlipBit(T x, int b)
    {
        return x ^ (T(1) << b);
    }

    template <typename T>
    constexpr bool TestBit(T x, int b)
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
        x |= ShiftLogicalRight(x, 1);
        x |= ShiftLogicalRight(x, 2);
        x |= ShiftLogicalRight(x, 4);

        if (sizeof(T) > 1)
        {
            x |= ShiftLogicalRight(x, 8);

            if (sizeof(T) > 2)
            {
                x |= ShiftLogicalRight(x, 16);

                if (sizeof(T) > 4)
                {
                    x |= ShiftLogicalRight(x, 32);
                }
            }
        }

        return x + 1;
    }

    template <typename T>
    constexpr T floorp2(T x)
    {
        x |= ShiftLogicalRight(x, 1);
        x |= ShiftLogicalRight(x, 2);
        x |= ShiftLogicalRight(x, 4);

        if (sizeof(T) > 1)
        {
            x |= ShiftLogicalRight(x, 8);

            if (sizeof(T) > 2)
            {
                x |= ShiftLogicalRight(x, 16);

                if (sizeof(T) > 4)
                {
                    x |= ShiftLogicalRight(x, 32);
                }
            }
        }

        return x - ShiftLogicalRight(x, 1);
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
            t = (x ^ ShiftLogicalRight(x, 16)) & T(0x00000000FFFF0000UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 16);
        }

        if (sizeof(T) > 2)
        {
            t = (x ^ ShiftLogicalRight(x, 8)) & T(0x0000FF000000FF00UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 8);
        }

        if (sizeof(T) > 1)
        {
            t = (x ^ ShiftLogicalRight(x, 4)) & T(0x00F000F000F000F0UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 4);
        }

        t = (x ^ ShiftLogicalRight(x, 2)) & T(0x0C0C0C0C0C0C0C0CUL);
        x = x ^ t ^ ShiftLogicalLeft(t, 2);
        t = (x ^ ShiftLogicalRight(x, 1)) & T(0x2222222222222222UL);
        x = x ^ t ^ ShiftLogicalLeft(t, 1);

        return x;
    }

    template <typename T>
    constexpr T outer_punshuffle(T x)
    {
        T t = 0;
        t = (x ^ ShiftLogicalRight(x, 1)) & T(0x2222222222222222UL);
        x = x ^ t ^ ShiftLogicalLeft(t, 1);
        t = (x ^ ShiftLogicalRight(x, 2)) & T(0x0C0C0C0C0C0C0C0CUL);
        x = x ^ t ^ ShiftLogicalLeft(t, 2);

        if (sizeof(T) > 1)
        {
            t = (x ^ ShiftLogicalRight(x, 4)) & T(0x00F000F000F000F0UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 4);
        }

        if (sizeof(T) > 2)
        {
            t = (x ^ ShiftLogicalRight(x, 8)) & T(0x0000FF000000FF00UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 8);
        }

        if (sizeof(T) > 4)
        {
            t = (x ^ ShiftLogicalRight(x, 16)) & T(0x00000000FFFF0000UL);
            x = x ^ t ^ ShiftLogicalLeft(t, 16);
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
            x = ShiftLogicalLeft(x & T(0x5555555555555555UL), 1) | ShiftLogicalRight(x & T(0xAAAAAAAAAAAAAAAAUL), 1);
        }

        if (k & 2)
        {
            x = ShiftLogicalLeft(x & T(0x3333333333333333UL), 2) | ShiftLogicalRight(x & T(0xCCCCCCCCCCCCCCCCUL), 2);
        }

        if (k & 4)
        {
            x = ShiftLogicalLeft(x & T(0x0F0F0F0F0F0F0F0FUL), 4) | ShiftLogicalRight(x & T(0xF0F0F0F0F0F0F0F0UL), 4);
        }

        if (sizeof(T) > 1 && k & 8)
        {
            x = ShiftLogicalLeft(x & T(0x00FF00FF00FF00FFUL), 8) | ShiftLogicalRight(x & T(0xFF00FF00FF00FF00UL), 8);
        }

        if (sizeof(T) > 2 && k & 16)
        {
            x = ShiftLogicalLeft(x & T(0x0000FFFF0000FFFFUL), 16) | ShiftLogicalRight(x & T(0xFFFF0000FFFF0000UL), 16);
        }

        if (sizeof(T) > 4 && k & 32)
        {
            x = ShiftLogicalLeft(x & T(0x00000000FFFFFFFFUL), 32) | ShiftLogicalRight(x & T(0xFFFFFFFF00000000UL), 32);
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

namespace Anemone
{
    [[nodiscard]] constexpr uint64_t ZigZagEncode(int64_t value)
    {
        uint64_t const u = static_cast<uint64_t>(value);
        uint64_t const mask = static_cast<uint64_t>(-static_cast<int64_t>(value < 0)); // 0 or ~0
        return (u << 1u) ^ mask;
    }

    [[nodiscard]] constexpr int64_t ZigZagDecode(
        uint64_t value)
    {
        uint64_t const mask = static_cast<uint64_t>(-static_cast<int64_t>(value & 1u));
        return static_cast<int64_t>((value >> 1u) ^ mask);
    }
}

namespace Anemone
{
    [[nodiscard]] constexpr size_t ULEB128Encode(uint64_t input, std::span<std::byte> output)
    {
        auto it = output.begin();
        auto end = output.end();

        while (it < end)
        {
            uint8_t byte = static_cast<uint8_t>(input & 0x7Fu);
            input >>= 7u;

            if (input != 0)
            {
                byte |= 0x80u;
            }

            *it++ = static_cast<std::byte>(byte);

            if (input == 0)
            {
                return static_cast<size_t>(it - output.begin());
            }
        }

        return 0;
    }

    [[nodiscard]] constexpr size_t ULEB128Decode(std::span<std::byte const> input, uint64_t& output)
    {
        uint64_t temp = 0;
        size_t shift = 0;

        auto it = input.begin();
        auto end = input.end();

        while (it < end)
        {
            uint8_t const byte = static_cast<uint8_t>(*it++);
            temp |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
            shift += 7u;

            if ((byte & 0x80u) == 0)
            {
                output = temp;
                return static_cast<size_t>(it - input.begin());
            }
        }

        output = 0;
        return 0;
    }

    [[nodiscard]] constexpr size_t SLEB128Encode(int64_t input, std::span<std::byte> output)
    {
        uint64_t encoded = ZigZagEncode(input);
        return ULEB128Encode(encoded, output);
    }

    [[nodiscard]] constexpr size_t SLEB128Decode(std::span<std::byte const> input, int64_t& output)
    {
        uint64_t encoded = 0;
        size_t count = ULEB128Decode(input, encoded);
        output = ZigZagDecode(encoded);
        return count;
    }
}
