#include "AnemoneRuntime/Crypto/Sha512.hxx"
#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/Memory.hxx"

#include <cstring>

namespace Anemone::Crypto::detail
{
    inline constexpr uint64_t sha512_k[80] = {
        0x428A2F98D728AE22u, 0x7137449123EF65CDu, 0xB5C0FBCFEC4D3B2Fu, 0xE9B5DBA58189DBBCu,
        0x3956C25BF348B538u, 0x59F111F1B605D019u, 0x923F82A4AF194F9Bu, 0xAB1C5ED5DA6D8118u,
        0xD807AA98A3030242u, 0x12835B0145706FBEu, 0x243185BE4EE4B28Cu, 0x550C7DC3D5FFB4E2u,
        0x72BE5D74F27B896Fu, 0x80DEB1FE3B1696B1u, 0x9BDC06A725C71235u, 0xC19BF174CF692694u,
        0xE49B69C19EF14AD2u, 0xEFBE4786384F25E3u, 0x0FC19DC68B8CD5B5u, 0x240CA1CC77AC9C65u,
        0x2DE92C6F592B0275u, 0x4A7484AA6EA6E483u, 0x5CB0A9DCBD41FBD4u, 0x76F988DA831153B5u,
        0x983E5152EE66DFABu, 0xA831C66D2DB43210u, 0xB00327C898FB213Fu, 0xBF597FC7BEEF0EE4u,
        0xC6E00BF33DA88FC2u, 0xD5A79147930AA725u, 0x06CA6351E003826Fu, 0x142929670A0E6E70u,
        0x27B70A8546D22FFCu, 0x2E1B21385C26C926u, 0x4D2C6DFC5AC42AEDu, 0x53380D139D95B3DFu,
        0x650A73548BAF63DEu, 0x766A0ABB3C77B2A8u, 0x81C2C92E47EDAEE6u, 0x92722C851482353Bu,
        0xA2BFE8A14CF10364u, 0xA81A664BBC423001u, 0xC24B8B70D0F89791u, 0xC76C51A30654BE30u,
        0xD192E819D6EF5218u, 0xD69906245565A910u, 0xF40E35855771202Au, 0x106AA07032BBD1B8u,
        0x19A4C116B8D2D0C8u, 0x1E376C085141AB53u, 0x2748774CDF8EEB99u, 0x34B0BCB5E19B48A8u,
        0x391C0CB3C5C95A63u, 0x4ED8AA4AE3418ACBu, 0x5B9CCA4F7763E373u, 0x682E6FF3D6B2B8A3u,
        0x748F82EE5DEFB2FCu, 0x78A5636F43172F60u, 0x84C87814A1F0AB72u, 0x8CC702081A6439ECu,
        0x90BEFFFA23631E28u, 0xA4506CEBDE82BDE9u, 0xBEF9A3F7B2C67915u, 0xC67178F2E372532Bu,
        0xCA273ECEEA26619Cu, 0xD186B8C721C0C207u, 0xEADA7DD6CDE0EB1Eu, 0xF57D4F7FEE6ED178u,
        0x06F067AA72176FBAu, 0x0A637DC5A2C898A6u, 0x113F9804BEF90DAEu, 0x1B710B35131C471Bu,
        0x28DB77F523047D84u, 0x32CAAB7B40C72493u, 0x3C9EBE0A15C9BEBCu, 0x431D67C49C100D4Cu,
        0x4CC5D4BECB3E42B6u, 0x597F299CFC657E2Au, 0x5FCB6FAB3AD6FAECu, 0x6C44198C4A475817u};


    constexpr uint64_t sha512_s0(uint64_t x)
    {
        return Bitwise::BitRotateRight(x, 1) ^ Bitwise::BitRotateRight(x, 8) ^ (x >> 7);
    }
    constexpr uint64_t sha512_s1(uint64_t x)
    {
        return Bitwise::BitRotateRight(x, 19) ^ Bitwise::BitRotateRight(x, 61) ^ (x >> 6);
    }

    constexpr uint64_t sha512_s2(uint64_t x)
    {
        return Bitwise::BitRotateRight(x, 28) ^ Bitwise::BitRotateRight(x, 34) ^ Bitwise::BitRotateRight(x, 39);
    }

    constexpr uint64_t sha512_s3(uint64_t x)
    {
        return Bitwise::BitRotateRight(x, 14) ^ Bitwise::BitRotateRight(x, 18) ^ Bitwise::BitRotateRight(x, 41);
    }

    constexpr uint64_t sha512_f0(uint64_t x, uint64_t y, uint64_t z)
    {
        return (x & y) | (z & (x | y));
    }

    constexpr uint64_t sha512_f1(uint64_t x, uint64_t y, uint64_t z)
    {
        return z ^ (x & (y ^ z));
    }

    constexpr void sha512_p(
        uint64_t a,
        uint64_t b,
        uint64_t c,
        uint64_t& d,
        uint64_t e,
        uint64_t f,
        uint64_t g,
        uint64_t& h,
        uint64_t w,
        uint64_t k)
    {
        uint64_t const t1 = h + sha512_s3(e) + sha512_f1(e, f, g) + k + w;
        uint64_t const t2 = sha512_s2(a) + sha512_f0(a, b, c);

        d += t1;
        h = t1 + t2;
    }

    void sha512_process_block(Sha512Context& context, std::byte const* buffer)
    {
        std::array<uint64_t, 80> w;

        for (size_t i = 0; i < 16; ++i)
        {
            w[i] = Bitwise::LoadUnalignedBigEndian<uint64_t>(buffer);
            buffer += sizeof(uint64_t);
        }

        for (size_t i = 16; i < 80; ++i)
        {
            w[i] = sha512_s1(w[i - 2]) + w[i - 7] + sha512_s0(w[i - 15]) + w[i - 16];
        }

        std::array<uint64_t, 8> s = context._state;

        for (size_t i = 0; i < 80; i += 8)
        {
            sha512_p(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], w[i + 0], sha512_k[i + 0]);
            sha512_p(s[7], s[0], s[1], s[2], s[3], s[4], s[5], s[6], w[i + 1], sha512_k[i + 1]);
            sha512_p(s[6], s[7], s[0], s[1], s[2], s[3], s[4], s[5], w[i + 2], sha512_k[i + 2]);
            sha512_p(s[5], s[6], s[7], s[0], s[1], s[2], s[3], s[4], w[i + 3], sha512_k[i + 3]);
            sha512_p(s[4], s[5], s[6], s[7], s[0], s[1], s[2], s[3], w[i + 4], sha512_k[i + 4]);
            sha512_p(s[3], s[4], s[5], s[6], s[7], s[0], s[1], s[2], w[i + 5], sha512_k[i + 5]);
            sha512_p(s[2], s[3], s[4], s[5], s[6], s[7], s[0], s[1], w[i + 6], sha512_k[i + 6]);
            sha512_p(s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[0], w[i + 7], sha512_k[i + 7]);
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
    void sha512_initialize(Sha512Context& context)
    {
        context._state[0] = 0x6A09E667F3BCC908u;
        context._state[1] = 0xBB67AE8584CAA73Bu;
        context._state[2] = 0x3C6EF372FE94F82Bu;
        context._state[3] = 0xA54FF53A5F1D36F1u;
        context._state[4] = 0x510E527FADE682D1u;
        context._state[5] = 0x9B05688C2B3E6C1Fu;
        context._state[6] = 0x1F83D9ABFB41BD6Bu;
        context._state[7] = 0x5BE0CD19137E2179u;

        context._total_length[0] = 0;
        context._total_length[1] = 0;
    }

    void sha512_update(Sha512Context& context, std::span<std::byte const> buffer)
    {
        if (not buffer.empty())
        {
            size_t remaining = context._total_length[0] & 0x7Fu;
            size_t const fill = 128 - remaining;

            context._total_length[0] += buffer.size();

            if (context._total_length[0] < buffer.size())
            {
                ++context._total_length[1];
            }

            if ((remaining != 0) and (buffer.size() >= fill))
            {
                std::memcpy(context._buffer.data() + remaining, buffer.data(), fill);

                detail::sha512_process_block(context, context._buffer.data());

                buffer = buffer.subspan(fill);
                remaining = 0;
            }

            while (buffer.size() >= 128)
            {
                detail::sha512_process_block(context, buffer.data());

                buffer = buffer.subspan(128);
            }

            if (not buffer.empty())
            {
                std::memcpy(context._buffer.data() + remaining, buffer.data(), buffer.size());
            }
        }
    }

    auto sha512_finalize(Sha512Context& context) -> Sha512Hash
    {
        size_t used = context._total_length[0] & 0x7Fu;
        context._buffer[used++] = std::byte{0x80};

        // Encode padding. Leave room for the 16 byte encoded length.
        if (used <= 112)
        {
            std::memset(context._buffer.data() + used, 0, 112 - used);
        }
        else
        {
            std::memset(context._buffer.data() + used, 0, 128 - used);

            detail::sha512_process_block(context, context._buffer.data());

            std::memset(context._buffer.data(), 0, 112);
        }

        // Encode length.
        uint64_t const hi = (context._total_length[0] >> 61u) | (context._total_length[1] << 3u);
        uint64_t const lo = (context._total_length[0] << 3u);

        Bitwise::StoreUnalignedBigEndian<uint64_t>(context._buffer.data() + 112, hi);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(context._buffer.data() + 120, lo);

        detail::sha512_process_block(context, context._buffer.data());

        Sha512Hash result;
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 0, context._state[0]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 8, context._state[1]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 16, context._state[2]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 24, context._state[3]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 32, context._state[4]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 40, context._state[5]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 48, context._state[6]);
        Bitwise::StoreUnalignedBigEndian<uint64_t>(result.data() + 56, context._state[7]);

        secure_clear_memory(&context, sizeof(context));

        return result;
    }

    auto sha512_from_buffer(std::span<std::byte const> buffer) -> Sha512Hash
    {
        Sha512Context context{};
        sha512_initialize(context);
        sha512_update(context, buffer);
        return sha512_finalize(context);
    }

    auto sha512_from_string(std::string_view string) -> Sha512Hash
    {
        return sha512_from_buffer(std::as_bytes(std::span(string)));
    }
}
