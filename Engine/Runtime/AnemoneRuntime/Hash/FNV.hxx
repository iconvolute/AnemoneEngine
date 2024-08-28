#pragma once
#include <cstdint>
#include <string_view>
#include <span>
#include <array>

namespace Anemone::Hash
{
    struct FNV1A32 final
    {
        using ValueType = uint32_t;

        static constexpr ValueType InitialValue{0x811c9dc5u};
        static constexpr ValueType Multiplier{0x1000193u};

        ValueType State{InitialValue};

        constexpr FNV1A32& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->State ^= static_cast<unsigned char>(c);
                this->State *= Multiplier;
            }

            return *this;
        }

        constexpr FNV1A32& Update(std::span<std::byte const> value)
        {
            for (std::byte const c : value)
            {
                this->State ^= static_cast<unsigned char>(c);
                this->State *= Multiplier;
            }

            return *this;
        }

        constexpr ValueType Finalize() const
        {
            return this->State;
        }

        [[nodiscard]] static constexpr ValueType FromString(std::string_view value)
        {
            return FNV1A32{}.Update(value).Finalize();
        }

        [[nodiscard]] static constexpr ValueType FromBuffer(std::span<std::byte const> value)
        {
            return FNV1A32{}.Update(value).Finalize();
        }
    };
}

namespace Anemone::Hash
{
    struct FNV1A64 final
    {
        using ValueType = uint64_t;

        static constexpr ValueType InitialValue{0xcbf29ce484222325u};
        static constexpr ValueType Multiplier{0x100000001B3u};

        ValueType State{InitialValue};

        constexpr FNV1A64& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->State ^= static_cast<unsigned char>(c);
                this->State *= Multiplier;
            }

            return *this;
        }

        constexpr FNV1A64& Update(std::span<std::byte const> value)
        {
            for (std::byte const c : value)
            {
                this->State ^= static_cast<unsigned char>(c);
                this->State *= Multiplier;
            }

            return *this;
        }

        constexpr ValueType Finalize() const
        {
            return this->State;
        }

        [[nodiscard]] static constexpr ValueType FromString(std::string_view value)
        {
            return FNV1A64{}.Update(value).Finalize();
        }

        [[nodiscard]] static constexpr ValueType FromBuffer(std::span<std::byte const> value)
        {
            return FNV1A64{}.Update(value).Finalize();
        }
    };
}

namespace Anemone::Hash
{
    struct FNV1A128 final
    {
        using ValueType = std::array<uint64_t, 2>;

        static constexpr ValueType InitialValue{0x62B821756295C58Du, 0x6C62272E07BB0142u};
        static constexpr ValueType Multiplier{0x000000000000013Bu, 0x0000000001000000u};

        ValueType State = InitialValue;

        static constexpr uint64_t BigMul64(uint64_t left, uint64_t right, uint64_t* lower)
        {
            uint64_t const ll = static_cast<uint32_t>(left);
            uint64_t const lu = left >> 32u;
            uint64_t const rl = static_cast<uint32_t>(right);
            uint64_t const ru = right >> 32u;
            uint64_t const ll_rl = ll * rl;
            uint64_t const ll_ru = ll * ru;
            uint64_t const lu_rl = lu * rl;
            uint64_t const lu_ru = lu * ru;
            uint64_t const m = (ll_rl >> 32u) + ll_ru + static_cast<uint32_t>(lu_rl);
            *lower = static_cast<uint32_t>(ll_rl) | (m << 32u);
            return (m >> 32u) + (lu_rl >> 32u) + lu_ru;
        }

        static constexpr ValueType Mul128(ValueType const& left, ValueType const& right)
        {
            uint64_t lower;
            uint64_t upper = BigMul64(left[0], right[0], &lower);
            upper += (left[0] * right[1]) + (left[1] * right[0]);
            return {lower, upper};
        }

        static constexpr void Xor128(ValueType& value, std::byte const byte)
        {
            value[0] ^= static_cast<unsigned char>(byte);
        }

    public:
        constexpr FNV1A128& Update(std::span<std::byte const> value)
        {
            for (std::byte const c : value)
            {
                Xor128(this->State, c);
                this->State = Mul128(this->State, Multiplier);
            }

            return *this;
        }

        constexpr FNV1A128& Update(std::string_view value)
        {
            for (char const c : value)
            {
                Xor128(this->State, static_cast<std::byte>(c));
                this->State = Mul128(this->State, Multiplier);
            }

            return *this;
        }

        constexpr ValueType Finalize() const
        {
            return this->State;
        }

        [[nodiscard]] static constexpr ValueType FromString(std::string_view value)
        {
            return FNV1A128{}.Update(value).Finalize();
        }

        [[nodiscard]] static constexpr ValueType FromBuffer(std::span<std::byte const> value)
        {
            return FNV1A128{}.Update(value).Finalize();
        }
    };
}
