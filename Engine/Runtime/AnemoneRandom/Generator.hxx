#pragma once
#include "AnemoneInterop/Headers.hxx"
#include <cstdint>
#include <array>
#include <concepts>

namespace Anemone
{
    template <typename T>
    concept RandomGenerator = requires(T generator)
    {
        { generator.Next() } -> std::same_as<uint64_t>;
        { generator.Randomize(uint64_t{0}) } -> std::same_as<void>;
    };

    struct RUNTIME_API Xorshiro256ss final
    {
        std::array<uint64_t, 4> State{};

        explicit Xorshiro256ss();

        explicit Xorshiro256ss(uint64_t seed);

        void Randomize(uint64_t seed);

        [[nodiscard]] uint64_t Next();

        void Forward();
    };

    struct RUNTIME_API Xorshiro512ss final
    {
        std::array<uint64_t, 8> State{};

        explicit Xorshiro512ss();

        explicit Xorshiro512ss(uint64_t seed);

        void Randomize(uint64_t seed);

        [[nodiscard]] uint64_t Next();

        void Forward();
    };

    using Random = Xorshiro512ss;
}
