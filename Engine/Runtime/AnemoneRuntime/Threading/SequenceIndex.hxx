#pragma once
#include <atomic>

namespace Anemone::Threading
{
    class alignas(uint64_t) SequenceIndex final
    {
    private:
        std::uint32_t m_Value{};
        std::uint32_t m_Sequence{};

    public:
        constexpr SequenceIndex() = default;

        constexpr SequenceIndex(std::uint32_t value)
            : m_Value{value}
        {
        }

        constexpr std::uint32_t GetValue() const
        {
            return this->m_Value;
        }

        constexpr void SetValue(std::uint32_t value)
        {
            this->m_Value = value;
            ++this->m_Sequence;
        }
    };

    static_assert(sizeof(SequenceIndex) == 8);
    static_assert(alignof(SequenceIndex) == 8);
    static_assert(std::atomic<SequenceIndex>::is_always_lock_free);
};
