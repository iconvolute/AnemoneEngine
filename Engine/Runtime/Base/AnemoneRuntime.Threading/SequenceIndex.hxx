#pragma once
#include <atomic>

namespace Anemone
{
    class SequenceIndex final
    {
    private:
        struct Index
        {
            std::uint32_t Value{};
            std::uint32_t Sequence{};
        };

        alignas(uint64_t) Index m_Index{};

    public:
        constexpr SequenceIndex() = default;

        constexpr SequenceIndex(std::uint32_t value)
            : m_Index{value}
        {
        }

        constexpr std::uint32_t GetValue() const
        {
            return this->m_Index.Value;
        }

        constexpr void SetValue(std::uint32_t value)
        {
            this->m_Index.Value = value;
            ++this->m_Index.Sequence;
        }
    };

    static_assert(sizeof(SequenceIndex) == 8);
    static_assert(alignof(SequenceIndex) == 8);
    static_assert(std::atomic<SequenceIndex>::is_always_lock_free);
};
