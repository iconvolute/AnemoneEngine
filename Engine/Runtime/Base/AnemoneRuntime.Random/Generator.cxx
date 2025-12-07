#include "AnemoneRuntime.Random/Generator.hxx"
#include "AnemoneRuntime.Base/Bitwise.hxx"

namespace Anemone
{
    namespace
    {
        constexpr uint64_t SplitMix(uint64_t& seed)
        {
            // Based on: https://github.com/svaarala/duktape/blob/master/misc/splitmix64.c

            seed += 0x9E3779B97F4A7C15u;
            uint64_t const z0 = seed;
            uint64_t const z1 = (z0 ^ (z0 >> 30u)) * 0xBF58476D1CE4E5B9u;
            uint64_t const z2 = (z1 ^ (z1 >> 27u)) * 0x94D049BB133111EBu;
            uint64_t const z3 = (z2 ^ (z2 >> 31u));
            return z3;
        }
    }
}

namespace Anemone
{
    static_assert(RandomGenerator<Xorshiro256ss>);

    Xorshiro256ss::Xorshiro256ss()
        : Xorshiro256ss{0}
    {
    }

    Xorshiro256ss::Xorshiro256ss(uint64_t seed)
    {
        this->Randomize(seed);
    }

    void Xorshiro256ss::Randomize(uint64_t seed)
    {
        for (uint64_t& state : this->State)
        {
            state = SplitMix(seed);
        }
    }

    uint64_t Xorshiro256ss::Next()
    {
        uint64_t const result = BitRotateLeft<uint64_t>(this->State[1] * 5u, 7u) * 9u;
        uint64_t const t = this->State[1] << 17u;

        this->State[2] ^= this->State[0];
        this->State[3] ^= this->State[1];
        this->State[1] ^= this->State[2];
        this->State[0] ^= this->State[3];

        this->State[2] ^= t;
        this->State[3] = BitRotateLeft<uint64_t>(this->State[3], 45u);

        return result;
    }

    void Xorshiro256ss::Forward()
    {
        constexpr uint64_t indices[] = {
            0x76E15D3EFEFDCBBFu,
            0xC5004E441C522FB3u,
            0x77710069854EE241u,
            0x39109BB02ACBE635u,
        };

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        uint64_t s2 = 0;
        uint64_t s3 = 0;

        for (uint64_t const index : indices)
        {
            for (size_t b = 0; b < 64; ++b)
            {
                if (index & (uint64_t{1} << b))
                {
                    s0 ^= this->State[0];
                    s1 ^= this->State[1];
                    s2 ^= this->State[2];
                    s3 ^= this->State[3];
                }

                (void)this->Next();
            }
        }

        this->State[0] = s0;
        this->State[1] = s1;
        this->State[2] = s2;
        this->State[3] = s3;
    }
}

namespace Anemone
{
    Xorshiro512ss::Xorshiro512ss()
        : Xorshiro512ss{0}
    {
    }

    Xorshiro512ss::Xorshiro512ss(uint64_t seed)
    {
        this->Randomize(seed);
    }

    void Xorshiro512ss::Randomize(uint64_t seed)
    {
        for (uint64_t& state : this->State)
        {
            state = SplitMix(seed);
        }
    }

    uint64_t Xorshiro512ss::Next()
    {
        uint64_t const result = BitRotateLeft<uint64_t>(this->State[1] * 5u, 7u) * 9u;
        uint64_t const t = this->State[1] << 11u;

        this->State[2] ^= this->State[0];
        this->State[5] ^= this->State[1];
        this->State[1] ^= this->State[2];
        this->State[7] ^= this->State[3];
        this->State[3] ^= this->State[4];
        this->State[4] ^= this->State[5];
        this->State[0] ^= this->State[6];
        this->State[6] ^= this->State[7];

        this->State[6] ^= t;
        this->State[7] = BitRotateLeft<uint64_t>(this->State[7], 21u);

        return result;
    }

    void Xorshiro512ss::Forward()
    {
        static constexpr uint64_t indices[] = {
            0x33ED89B6E7A353F9u,
            0x760083D7955323BEu,
            0x2837F2FBB5F22FAEu,
            0x4B8C5674D309511Cu,
            0xB11AC47A7BA28C25u,
            0xF1BE7667092BCC1Cu,
            0x53851EFDB6DF0AAFu,
            0x1EBBC8B23EAF25DBu,
        };

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        uint64_t s2 = 0;
        uint64_t s3 = 0;
        uint64_t s4 = 0;
        uint64_t s5 = 0;
        uint64_t s6 = 0;
        uint64_t s7 = 0;

        for (uint64_t const index : indices)
        {
            for (size_t b = 0; b < 64u; ++b)
            {
                if (index & (uint64_t{1} << b))
                {
                    s0 ^= this->State[0];
                    s1 ^= this->State[1];
                    s2 ^= this->State[2];
                    s3 ^= this->State[3];
                    s4 ^= this->State[4];
                    s5 ^= this->State[5];
                    s6 ^= this->State[6];
                    s7 ^= this->State[7];
                }

                (void)this->Next();
            }
        }

        this->State[0] = s0;
        this->State[1] = s1;
        this->State[2] = s2;
        this->State[3] = s3;
        this->State[4] = s4;
        this->State[5] = s5;
        this->State[6] = s6;
        this->State[7] = s7;
    }

}
