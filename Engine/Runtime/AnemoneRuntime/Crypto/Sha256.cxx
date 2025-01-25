#include "AnemoneRuntime/Crypto/Sha256.hxx"
#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Memory.hxx"

#include <cstring>

namespace Anemone::Crypto::detail
{
    static constexpr uint32_t sha256_k[64] = {
        0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
        0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
        0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
        0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
        0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
        0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
        0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
        0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
        0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
        0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
        0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
        0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
        0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
        0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
        0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
        0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

    constexpr uint32_t sha256_s0(uint32_t x)
    {
        return Bitwise::BitRotateRight(x, 7) ^ Bitwise::BitRotateRight(x, 18) ^ (x >> 3);
    }

    constexpr uint32_t sha256_s1(uint32_t x)
    {
        return Bitwise::BitRotateRight(x, 17) ^ Bitwise::BitRotateRight(x, 19) ^ (x >> 10);
    }

    constexpr uint32_t sha256_s2(uint32_t x)
    {
        return Bitwise::BitRotateRight(x, 2) ^ Bitwise::BitRotateRight(x, 13) ^ Bitwise::BitRotateRight(x, 22);
    }

    constexpr uint32_t sha256_s3(uint32_t x)
    {
        return Bitwise::BitRotateRight(x, 6) ^ Bitwise::BitRotateRight(x, 11) ^ Bitwise::BitRotateRight(x, 25);
    }

    constexpr uint32_t sha256_f0(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x & y) | (z & (x | y));
    }

    constexpr uint32_t sha256_f1(uint32_t x, uint32_t y, uint32_t z)
    {
        return z ^ (x & (y ^ z));
    }

    constexpr uint32_t sha256_r(std::array<uint32_t, 80>& w, size_t t)
    {
        return w[t] = sha256_s1(w[t - 2]) + w[t - 7] + sha256_s0(w[t - 15]) + w[t - 16];
    }

    constexpr void sha256_p(
        uint32_t a,
        uint32_t b,
        uint32_t c,
        uint32_t& d,
        uint32_t e,
        uint32_t f,
        uint32_t g,
        uint32_t& h,
        uint32_t w,
        uint32_t k)
    {
        uint32_t const t1 = h + sha256_s3(e) + sha256_f1(e, f, g) + k + w;
        uint32_t const t2 = sha256_s2(a) + sha256_f0(a, b, c);
        d += t1;
        h = t1 + t2;
    }

    constexpr void sha256_process_block(Sha256Context& context, std::byte const* buffer)
    {
        std::array<uint32_t, 80> w;

        for (size_t i = 0; i < 16; ++i)
        {
            w[i] = Bitwise::LoadUnalignedBigEndian<uint32_t>(buffer);
            buffer += sizeof(uint32_t);
        }

        std::array<uint32_t, 8> s = context._state;

        for (size_t i = 0; i < 16; i += 8)
        {
            sha256_p(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], w[i + 0], sha256_k[i + 0]);
            sha256_p(s[7], s[0], s[1], s[2], s[3], s[4], s[5], s[6], w[i + 1], sha256_k[i + 1]);
            sha256_p(s[6], s[7], s[0], s[1], s[2], s[3], s[4], s[5], w[i + 2], sha256_k[i + 2]);
            sha256_p(s[5], s[6], s[7], s[0], s[1], s[2], s[3], s[4], w[i + 3], sha256_k[i + 3]);
            sha256_p(s[4], s[5], s[6], s[7], s[0], s[1], s[2], s[3], w[i + 4], sha256_k[i + 4]);
            sha256_p(s[3], s[4], s[5], s[6], s[7], s[0], s[1], s[2], w[i + 5], sha256_k[i + 5]);
            sha256_p(s[2], s[3], s[4], s[5], s[6], s[7], s[0], s[1], w[i + 6], sha256_k[i + 6]);
            sha256_p(s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[0], w[i + 7], sha256_k[i + 7]);
        }

        for (size_t i = 16; i < 64; i += 8)
        {
            sha256_p(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], sha256_r(w, i + 0), sha256_k[i + 0]);
            sha256_p(s[7], s[0], s[1], s[2], s[3], s[4], s[5], s[6], sha256_r(w, i + 1), sha256_k[i + 1]);
            sha256_p(s[6], s[7], s[0], s[1], s[2], s[3], s[4], s[5], sha256_r(w, i + 2), sha256_k[i + 2]);
            sha256_p(s[5], s[6], s[7], s[0], s[1], s[2], s[3], s[4], sha256_r(w, i + 3), sha256_k[i + 3]);
            sha256_p(s[4], s[5], s[6], s[7], s[0], s[1], s[2], s[3], sha256_r(w, i + 4), sha256_k[i + 4]);
            sha256_p(s[3], s[4], s[5], s[6], s[7], s[0], s[1], s[2], sha256_r(w, i + 5), sha256_k[i + 5]);
            sha256_p(s[2], s[3], s[4], s[5], s[6], s[7], s[0], s[1], sha256_r(w, i + 6), sha256_k[i + 6]);
            sha256_p(s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[0], sha256_r(w, i + 7), sha256_k[i + 7]);
        }

        context._state[0] += s[0];
        context._state[1] += s[1];
        context._state[2] += s[2];
        context._state[3] += s[3];
        context._state[4] += s[4];
        context._state[5] += s[5];
        context._state[6] += s[6];
        context._state[7] += s[7];
    }
}

namespace Anemone::Crypto
{
    void sha256_initialize(Sha256Context& context)
    {
        context._total_length[0] = 0;
        context._total_length[1] = 0;

        context._state[0] = 0x6A09E667u;
        context._state[1] = 0xBB67AE85u;
        context._state[2] = 0x3C6EF372u;
        context._state[3] = 0xA54FF53Au;
        context._state[4] = 0x510E527Fu;
        context._state[5] = 0x9B05688Cu;
        context._state[6] = 0x1F83D9ABu;
        context._state[7] = 0x5BE0CD19u;
    }

    void sha256_update(Sha256Context& context, std::span<std::byte const> buffer)
    {
        size_t remaining = context._total_length[0] & 0x3fu;
        size_t fill = 64 - remaining;

        uint64_t total_length = std::bit_cast<uint64_t>(context._total_length) + buffer.size();
        context._total_length = std::bit_cast<std::array<uint32_t, 2>>(total_length);

        if ((remaining != 0) and (buffer.size() >= fill))
        {
            std::memcpy(context._buffer.data() + remaining, buffer.data(), fill);

            detail::sha256_process_block(context, context._buffer.data());

            buffer = buffer.subspan(fill);
            remaining = 0;
        }

        while (buffer.size() >= 64)
        {
            detail::sha256_process_block(context, context._buffer.data());

            buffer = buffer.subspan(64);
        }

        if (not buffer.empty())
        {
            std::memcpy(context._buffer.data() + remaining, buffer.data(), buffer.size());
        }
    }

    auto sha256_finalize(Sha256Context& context) -> Sha256Hash
    {
        size_t used = context._total_length[0] & 0x3Fu;
        context._buffer[used++] = std::byte{0x80};

        // Encode padding. Leave room for the 16 byte encoded length.
        if (used <= 56)
        {
            std::memset(context._buffer.data() + used, 0, 56 - used);
        }
        else
        {
            std::memset(context._buffer.data() + used, 0, 64 - used);

            detail::sha256_process_block(context, context._buffer.data());

            std::memset(context._buffer.data(), 0, 56);
        }

        // Encode length.
        uint32_t const hi = (context._total_length[0] >> 29u) | (context._total_length[1] << 3u);
        uint32_t const lo = (context._total_length[0] << 3u);

        Bitwise::StoreUnalignedBigEndian<uint32_t>(context._buffer.data() + 56, hi);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(context._buffer.data() + 60, lo);

        detail::sha256_process_block(context, context._buffer.data());

        Sha256Hash result;
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 0, context._state[0]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 4, context._state[1]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 8, context._state[2]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 12, context._state[3]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 16, context._state[4]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 20, context._state[5]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 24, context._state[6]);
        Bitwise::StoreUnalignedBigEndian<uint32_t>(result.data() + 28, context._state[7]);

        secure_clear_memory(&context, sizeof(context));

        return result;
    }

    auto sha256_from_buffer(std::span<std::byte const> buffer) -> Sha256Hash
    {
        Sha256Context context{};
        sha256_initialize(context);
        sha256_update(context, buffer);
        return sha256_finalize(context);
    }

    auto sha256_from_string(std::string_view string) -> Sha256Hash
    {
        return sha256_from_buffer(std::as_bytes(std::span(string)));
    }
}
