#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneRandom/Generator.hxx"

#include <bit>
#include <span>
#include <vector>

namespace Anemone
{
    void RandomBytes(RandomGenerator auto& generator, std::span<std::byte> buffer)
    {
        while (buffer.size() >= 8u)
        {
            auto bytes = std::bit_cast<std::array<std::byte, 8>>(generator.Next());

            buffer[0] = bytes[0];
            buffer[1] = bytes[1];
            buffer[2] = bytes[2];
            buffer[3] = bytes[3];
            buffer[4] = bytes[4];
            buffer[5] = bytes[5];
            buffer[6] = bytes[6];
            buffer[7] = bytes[7];

            buffer = buffer.subspan(8u);
        }

        if (not buffer.empty())
        {
            auto bytes = std::bit_cast<std::array<std::byte, 8>>(generator.Next());

            for (size_t i = 0; i < buffer.size(); ++i)
            {
                buffer[i] = bytes[i];
            }
        }
    }

    void RandomReadableString(RandomGenerator auto& generator, std::span<char> buffer)
    {
        static constexpr char s_Base32Letters[] = "0123456789abcdefghjknpqrstuvwxyz";

        while (not buffer.empty())
        {
            size_t const count = buffer.size() % sizeof(uint64_t);

            std::array<uint8_t, 8> const sample = std::bit_cast<std::array<uint8_t, 8>>(generator.GenerateBits());

            for (size_t i = 0; i < count; ++i)
            {
                buffer[i] = s_Base32Letters[sample[i] & 0x1F];
            }

            buffer = buffer.subspan(count);
        }
    }

    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<uint8_t const> data);

    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<float const> data, size_t buckets, float min, float max);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram, uint64_t total);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram);
}
