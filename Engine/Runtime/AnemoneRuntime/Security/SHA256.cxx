#include "AnemoneRuntime/Security/SHA256.hxx"
#include "AnemoneRuntime/Security/SecureMemory.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"

namespace Anemone::Internal
{
    struct SHA256Impl final
    {
        static constexpr std::array<uint32_t, 64> K{
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

        static constexpr uint32_t Choose(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & y) ^ (~x & z);
        }

        static constexpr uint32_t Major(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & y) ^ (x & z) ^ (y & z);
        }

        static constexpr uint32_t Sigma_0(uint32_t x)
        {
            return (Bitwise::BitRotateRight(x, 7) ^ Bitwise::BitRotateRight(x, 18) ^ (x >> 3));
        }

        static constexpr uint32_t Sigma_1(uint32_t x)
        {
            return (Bitwise::BitRotateRight(x, 17) ^ Bitwise::BitRotateRight(x, 19) ^ (x >> 10));
        }

        static constexpr uint32_t Sum_A(uint32_t x)
        {
            return (Bitwise::BitRotateRight(x, 2) ^ Bitwise::BitRotateRight(x, 13) ^ Bitwise::BitRotateRight(x, 22));
        }

        static constexpr uint32_t Sum_E(uint32_t x)
        {
            return (Bitwise::BitRotateRight(x, 6) ^ Bitwise::BitRotateRight(x, 11) ^ Bitwise::BitRotateRight(x, 25));
        }

        static constexpr void Expand(std::array<uint32_t, 64>& x)
        {
            for (int i = 16; i < 64; i++)
            {
                x[i] = Sigma_1(x[i - 2]) + x[i - 7] + Sigma_0(x[i - 15]) + x[i - 16];
            }
        }

        static constexpr void CopyBytesToUInt32BigEndian(uint32_t* destination, size_t count, uint8_t const* source)
        {
            for (size_t i = 0, j = 0; i < count; ++i, j += 4)
            {
                destination[i] = Bitwise::LoadUnalignedBigEndian<uint32_t>(source + j);
            }
        }

        static void TransformBlock(std::array<uint32_t, 64>& w, uint32_t* state, uint8_t const* block)
        {
            uint32_t a = state[0];
            uint32_t b = state[1];
            uint32_t c = state[2];
            uint32_t d = state[3];
            uint32_t e = state[4];
            uint32_t f = state[5];
            uint32_t g = state[6];
            uint32_t h = state[7];

            CopyBytesToUInt32BigEndian(w.data(), 16, block);
            Expand(w);

            for (size_t j = 0; j < 64;)
            {
                uint32_t temp = h + Sum_E(e) + Choose(e, f, g) + K[j] + w[j];
                uint32_t const ee = d + temp;
                uint32_t const aa = temp + Sum_A(a) + Major(a, b, c);
                j++;

                temp = g + Sum_E(ee) + Choose(ee, e, f) + K[j] + w[j];
                uint32_t const ff = c + temp;
                uint32_t const bb = temp + Sum_A(aa) + Major(aa, a, b);
                j++;

                temp = f + Sum_E(ff) + Choose(ff, ee, e) + K[j] + w[j];
                uint32_t const gg = b + temp;
                uint32_t const cc = temp + Sum_A(bb) + Major(bb, aa, a);
                j++;

                temp = e + Sum_E(gg) + Choose(gg, ff, ee) + K[j] + w[j];
                uint32_t const hh = a + temp;
                uint32_t const dd = temp + Sum_A(cc) + Major(cc, bb, aa);
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
    std::expected<void, Error> SHA256::Initialize()
    {
        this->_count = 0;
        this->_buffer = {};
        this->_w = {};

        this->_state[0] = 0x6A09E667u;
        this->_state[1] = 0xBB67AE85u;
        this->_state[2] = 0x3C6EF372u;
        this->_state[3] = 0xA54FF53Au;
        this->_state[4] = 0x510E527Fu;
        this->_state[5] = 0x9B05688Cu;
        this->_state[6] = 0x1F83D9ABu;
        this->_state[7] = 0x5BE0CD19u;

        return {};
    }

    std::expected<void, Error> SHA256::Update(std::span<std::byte const> buffer)
    {
        if (not buffer.empty())
        {
            size_t remaining = this->_count & 0x3Fu;
            size_t fill = 64 - remaining;

            uint64_t totalLength = this->_count + buffer.size();
            this->_count = totalLength;

            if ((remaining > 0) and (buffer.size() >= fill))
            {
                std::memcpy(this->_buffer.data() + remaining, buffer.data(), fill);

                Internal::SHA256Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());

                buffer = buffer.subspan(fill);
                remaining = 0;
            }

            while (buffer.size() >= 64)
            {
                Internal::SHA256Impl::TransformBlock(this->_w, this->_state.data(), reinterpret_cast<uint8_t const*>(buffer.data()));
                buffer = buffer.subspan(64);
            }

            if (not buffer.empty())
            {
                std::memcpy(this->_buffer.data() + remaining, buffer.data(), buffer.size());
            }
        }

        return {};
    }

    std::expected<void, Error> SHA256::Finalize(std::span<std::byte> hash)
    {
        if (hash.size() < 32)
        {
            return std::unexpected(Error::InvalidArgument);
        }

        size_t used = this->_count & 0x3Fu;
        this->_buffer[used++] = 0x80u;

        if (used <= 56)
        {
            std::memset(this->_buffer.data() + used, 0, 56 - used);
        }
        else
        {
            std::memset(this->_buffer.data() + used, 0, 64 - used);
            Internal::SHA256Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());
            std::memset(this->_buffer.data(), 0, 56);
        }

        uint64_t const length = this->_count << 3;
        Bitwise::StoreUnalignedBigEndian<uint64_t>(this->_buffer.data() + 56, length);

        Internal::SHA256Impl::TransformBlock(this->_w, this->_state.data(), this->_buffer.data());

        for (size_t i = 0; i < 8; ++i)
        {
            Bitwise::StoreUnalignedBigEndian<uint32_t>(hash.data() + i * 4, this->_state[i]);
        }

        SecureMemory::Zero(std::span{this->_buffer});
        SecureMemory::Zero(std::span{this->_w});

        return {};
    }

    size_t SHA256::GetHashSize() const
    {
        return HashSize;
    }

    size_t SHA256::GetBlockSize() const
    {
        return BlockSize;
    }
}
