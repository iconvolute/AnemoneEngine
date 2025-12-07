#include "AnemoneRuntime.Security/SHA512.hxx"
#include "AnemoneRuntime.Security/SecureMemory.hxx"
#include "AnemoneRuntime.Base/Bitwise.hxx"

namespace Anemone::Internal
{
    struct SHA512Impl final
    {
        static constexpr std::array<uint64_t, 80> K{
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

        static constexpr uint64_t Choose(uint64_t x, uint64_t y, uint64_t z)
        {
            return (x & y) ^ (~x & z);
        }

        static constexpr uint64_t Major(uint64_t x, uint64_t y, uint64_t z)
        {
            return (x & y) ^ (x & z) ^ (y & z);
        }

        static constexpr uint64_t Sigma_0(uint64_t x)
        {
            return (BitRotateRight(x, 1) ^ BitRotateRight(x, 8) ^ (x >> 7));
        }

        static constexpr uint64_t Sigma_1(uint64_t x)
        {
            return (BitRotateRight(x, 19) ^ BitRotateRight(x, 61) ^ (x >> 6));
        }

        static constexpr uint64_t Sum_A(uint64_t x)
        {
            return (BitRotateRight(x, 28) ^ BitRotateRight(x, 34) ^ BitRotateRight(x, 39));
        }

        static constexpr uint64_t Sum_E(uint64_t x)
        {
            return (BitRotateRight(x, 14) ^ BitRotateRight(x, 18) ^ BitRotateRight(x, 41));
        }

        static constexpr void Expand(std::array<uint64_t, 80>& x)
        {
            for (int i = 16; i < 80; i++)
            {
                x[i] = Sigma_1(x[i - 2]) + x[i - 7] + Sigma_0(x[i - 15]) + x[i - 16];
            }
        }

        static constexpr void CopyBytesToUInt64BigEndian(uint64_t* destination, size_t count, uint8_t const* source)
        {
            for (size_t i = 0, j = 0; i < count; ++i, j += 8)
            {
                destination[i] = UnsafeLoadUnalignedBigEndian<uint64_t>(source + j);
            }
        }

        static void TransformBlock(std::array<uint64_t, 80>& w, uint64_t* state, uint8_t const* block)
        {
            uint64_t a = state[0];
            uint64_t b = state[1];
            uint64_t c = state[2];
            uint64_t d = state[3];
            uint64_t e = state[4];
            uint64_t f = state[5];
            uint64_t g = state[6];
            uint64_t h = state[7];

            CopyBytesToUInt64BigEndian(w.data(), 16, block);
            Expand(w);

            for (size_t j = 0; j < 80;)
            {
                uint64_t temp = h + Sum_E(e) + Choose(e, f, g) + K[j] + w[j];
                uint64_t ee = d + temp;
                uint64_t aa = temp + Sum_A(a) + Major(a, b, c);
                j++;

                temp = g + Sum_E(ee) + Choose(ee, e, f) + K[j] + w[j];
                uint64_t ff = c + temp;
                uint64_t bb = temp + Sum_A(aa) + Major(aa, a, b);
                j++;

                temp = f + Sum_E(ff) + Choose(ff, ee, e) + K[j] + w[j];
                uint64_t gg = b + temp;
                uint64_t cc = temp + Sum_A(bb) + Major(bb, aa, a);
                j++;

                temp = e + Sum_E(gg) + Choose(gg, ff, ee) + K[j] + w[j];
                uint64_t hh = a + temp;
                uint64_t dd = temp + Sum_A(cc) + Major(cc, bb, aa);
                j++;

                temp = ee + Sum_E(hh) + Choose(hh, gg, ff) + K[j] + w[j];
                h = aa + temp;
                d = temp + Sum_A(dd) + Major(dd, cc, bb);
                j++;

                temp = ff + Sum_E(h) + Choose(h, hh, gg) + K[j] + w[j];
                g = bb + temp;
                c = temp + Sum_A(d) + Major(d, dd, cc);
                j++;

                temp = gg + Sum_E(g) + Choose(g, h, hh) + K[j] + w[j];
                f = cc + temp;
                b = temp + Sum_A(c) + Major(c, d, dd);
                j++;

                temp = hh + Sum_E(f) + Choose(f, g, h) + K[j] + w[j];
                e = dd + temp;
                a = temp + Sum_A(b) + Major(b, c, d);
                j++;
            }

            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
            state[5] += f;
            state[6] += g;
            state[7] += h;
        }
    };
}

namespace Anemone
{
    std::expected<void, Error> SHA512::Initialize()
    {
        this->_count = {};
        this->_buffer = {};
        this->_w = {};

        this->_state[0] = 0x6a09e667f3bcc908u;
        this->_state[1] = 0xbb67ae8584caa73bu;
        this->_state[2] = 0x3c6ef372fe94f82bu;
        this->_state[3] = 0xa54ff53a5f1d36f1u;
        this->_state[4] = 0x510e527fade682d1u;
        this->_state[5] = 0x9b05688c2b3e6c1fu;
        this->_state[6] = 0x1f83d9abfb41bd6bu;
        this->_state[7] = 0x5be0cd19137e2179u;

        return {};
    }

    std::expected<void, Error> SHA512::Update(std::span<std::byte const> buffer)
    {
        if (not buffer.empty())
        {
            size_t remaining = this->_count & 0x7Fu;
            size_t fill = 128 - remaining;

            uint64_t totalLength = this->_count + buffer.size();
            this->_count = totalLength;

            if ((remaining > 0) and (buffer.size() >= fill))
            {
                std::memcpy(this->_buffer.data() + remaining, buffer.data(), fill);

                Internal::SHA512Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());

                buffer = buffer.subspan(fill);
                remaining = 0;
            }

            while (buffer.size() >= 128)
            {
                Internal::SHA512Impl::TransformBlock(this->_w, this->_state.data(), reinterpret_cast<uint8_t const*>(buffer.data()));
                buffer = buffer.subspan(128);
            }

            if (not buffer.empty())
            {
                std::memcpy(this->_buffer.data() + remaining, buffer.data(), buffer.size());
            }
        }

        return {};
    }

    std::expected<void, Error> SHA512::Finalize(std::span<std::byte> hash)
    {
        if (hash.size() < 64)
        {
            return std::unexpected(Error::InvalidArgument);
        }

        size_t used = this->_count & 0x7Fu;
        this->_buffer[used++] = 0x80u;

        if (used <= 120)
        {
            std::memset(this->_buffer.data() + used, 0, 120 - used);
        }
        else
        {
            std::memset(this->_buffer.data() + used, 0, 128 - used);
            Internal::SHA512Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());
            std::memset(this->_buffer.data(), 0, 120);
        }

        uint64_t const length = this->_count << 3;
        UnsafeStoreUnalignedBigEndian<uint64_t>(this->_buffer.data() + 120, length);

        Internal::SHA512Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());

        for (size_t i = 0; i < 8; ++i)
        {
            UnsafeStoreUnalignedBigEndian<uint64_t>(hash.data() + i * 8, this->_state[i]);
        }

        SecureMemory::Zero(std::span{this->_buffer});
        SecureMemory::Zero(std::span{this->_w});

        return {};
    }

    size_t SHA512::GetHashSize() const
    {
        return HashSize;
    }

    size_t SHA512::GetBlockSize() const
    {
        return BlockSize;
    }
}
