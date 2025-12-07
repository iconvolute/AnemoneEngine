#pragma once
#include <cstdint>
#include <string_view>
#include <span>
#include <array>

#include "AnemoneRuntime.Base/Bitwise.hxx"

namespace Anemone
{
    struct XXHash64 final
    {
        static constexpr uint64_t Prime64_1 = 0x9E3779B185EBCA87u;
        static constexpr uint64_t Prime64_2 = 0xC2B2AE3D27D4EB4Fu;
        static constexpr uint64_t Prime64_3 = 0x165667B19E3779F9u;
        static constexpr uint64_t Prime64_4 = 0x85EBCA77C2B2AE63u;
        static constexpr uint64_t Prime64_5 = 0x27D4EB2F165667C5u;

        static constexpr size_t StripeSize = 32uz;

        uint64_t Acc1;
        uint64_t Acc2;
        uint64_t Acc3;
        uint64_t Acc4;
        uint64_t AccSmall;
        size_t TotalLength;
        std::array<std::byte, StripeSize> Buffer;

    public:
        constexpr explicit XXHash64(uint64_t seed = 0)
        {
            this->Initialize(seed);
        }

        constexpr void Initialize(uint64_t seed = 0)
        {
            this->Acc1 = seed + Prime64_1 + Prime64_2;
            this->Acc2 = seed + Prime64_2;
            this->Acc3 = seed + 0;
            this->Acc4 = seed - Prime64_1;
            this->AccSmall = seed + Prime64_5;
            this->TotalLength = 0;
        }

        constexpr void Update(std::span<std::byte const> source)
        {
            size_t remainder = this->TotalLength & 0x1Fuz;

            if (remainder != 0)
            {
                // Fill the internal buffer to form a full stripe.
                size_t needed = StripeSize - remainder;

                if (source.size() >= needed)
                {
                    // Process full stripe.
                    std::copy_n(
                        source.data(),
                        needed,
                        this->Buffer.data() + remainder);

                    this->ProcessStripe(this->Buffer.data());

                    source = source.subspan(needed);
                    this->TotalLength += needed;
                }
                else
                {
                    // Not enough data to fill the stripe.
                    std::copy_n(
                        source.data(),
                        source.size(),
                        this->Buffer.data() + remainder
                    );
                    this->TotalLength += source.size();
                    return;
                }
            }

            while (source.size() >= StripeSize)
            {
                this->ProcessStripe(source.data());
                source = source.subspan(StripeSize);
                this->TotalLength += StripeSize;
            }

            if (source.size() > 0)
            {
                std::copy_n(
                    source.data(),
                    source.size(),
                    this->Buffer.data()
                );

                this->TotalLength += source.size();
            }
        }

        constexpr uint64_t Finalize()
        {
            uint64_t acc = (this->TotalLength >= StripeSize)
                ? this->Converge()
                : this->AccSmall;

            acc += this->TotalLength;

            std::span remaining{this->Buffer.data(), this->TotalLength & 0x1Fuz};

            while (remaining.size() >= sizeof(uint64_t))
            {
                uint64_t const lane = LoadLittleEndian<uint64_t>(remaining.data());

                acc ^= ApplyRound(0, lane);
                acc = BitRotateLeft<uint64_t>(acc, 27);
                acc *= Prime64_1;
                acc += Prime64_4;
                remaining = remaining.subspan(sizeof(uint64_t));
            }

            if (remaining.size() >= sizeof(uint32_t))
            {
                uint64_t const lane = LoadLittleEndian<uint32_t>(remaining.data());

                acc ^= lane * Prime64_1;
                acc = BitRotateLeft<uint64_t>(acc, 23);
                acc *= Prime64_2;
                acc += Prime64_3;
                remaining = remaining.subspan(sizeof(uint32_t));
            }

            for (std::byte const& byte : remaining)
            {
                acc ^= static_cast<uint64_t>(byte) * Prime64_5;
                acc = BitRotateLeft<uint64_t>(acc, 11);
                acc *= Prime64_1;
            }

            return Avalanche(acc);
        }

        static constexpr uint64_t FromString(std::string_view data, uint64_t seed = 0)
        {
            XXHash64 hasher{seed};
            hasher.Update(std::as_bytes(std::span{data}));
            return hasher.Finalize();
        }

        static constexpr uint64_t FromBuffer(std::span<std::byte const> data, uint64_t seed = 0)
        {
            XXHash64 hasher{seed};
            hasher.Update(data);
            return hasher.Finalize();
        }
    private:
        static constexpr uint64_t Avalanche(uint64_t hash)
        {
            hash ^= hash >> 33u;
            hash *= Prime64_2;
            hash ^= hash >> 29u;
            hash *= Prime64_3;
            hash ^= hash >> 32u;
            return hash;
        }

        static constexpr uint64_t MergeAccumulator(uint64_t acc, uint64_t accN)
        {
            acc ^= ApplyRound(0, accN);
            acc *= Prime64_1;
            acc += Prime64_4;
            return acc;
        }

        constexpr void ProcessStripe(std::byte const* source)
        {
            this->Acc1 = ApplyRound(this->Acc1, LoadLittleEndian<uint64_t>(source + 0 * sizeof(uint64_t)));
            this->Acc2 = ApplyRound(this->Acc2, LoadLittleEndian<uint64_t>(source + 1 * sizeof(uint64_t)));
            this->Acc3 = ApplyRound(this->Acc3, LoadLittleEndian<uint64_t>(source + 2 * sizeof(uint64_t)));
            this->Acc4 = ApplyRound(this->Acc4, LoadLittleEndian<uint64_t>(source + 3 * sizeof(uint64_t)));
        }

        template <typename T>
        static constexpr T LoadLittleEndian(std::byte const* source)
        {
            std::byte temp[sizeof(T)];
            std::copy_n(source, sizeof(T), temp);
            return std::bit_cast<T>(temp);
        }

        constexpr uint64_t Converge()
        {
            uint64_t acc =
                BitRotateLeft<uint64_t>(this->Acc1, 1) +
                BitRotateLeft<uint64_t>(this->Acc2, 7) +
                BitRotateLeft<uint64_t>(this->Acc3, 12) +
                BitRotateLeft<uint64_t>(this->Acc4, 18);

            acc = MergeAccumulator(acc, this->Acc1);
            acc = MergeAccumulator(acc, this->Acc2);
            acc = MergeAccumulator(acc, this->Acc3);
            acc = MergeAccumulator(acc, this->Acc4);

            return acc;
        }

        static constexpr uint64_t ApplyRound(uint64_t acc, uint64_t lane)
        {
            acc += lane * Prime64_2;
            acc = BitRotateLeft<uint64_t>(acc, 31);
            acc *= Prime64_1;

            return acc;
        }
    };
}
