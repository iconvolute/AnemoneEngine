#pragma once
#include "AnemoneInterop/Headers.hxx"
#include <atomic>
#include <type_traits>

namespace Anemone
{
    template <typename T>
    class SequentialLock final
    {
        static_assert(std::is_copy_assignable_v<T>);
        static_assert(std::is_copy_constructible_v<T>);

    private:
        alignas(ANEMONE_CACHELINE_SIZE) T m_Value;
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Sequence;

    public:
        SequentialLock() = default;

        explicit SequentialLock(T const& value);

        ~SequentialLock() = default;

        SequentialLock(SequentialLock const&) = delete;

        SequentialLock(SequentialLock&&) = delete;

        SequentialLock& operator=(SequentialLock const&) = delete;

        SequentialLock& operator=(SequentialLock&&) = delete;

    public:
        T Read() const;

        void Write(T const& value);
    };

    template <typename T>
    SequentialLock<T>::SequentialLock(T const& value)
        : m_Value{value}
    {
    }

    template <typename T>
    T SequentialLock<T>::Read() const
    {
        T result{};

        std::size_t seq0;
        std::size_t seq1;

        do
        {
            seq0 = this->m_Sequence.load(std::memory_order::acquire);
            std::atomic_signal_fence(std::memory_order::acq_rel);
            result = this->m_Value;
            std::atomic_signal_fence(std::memory_order::acq_rel);
            seq1 = this->m_Sequence.load(std::memory_order::acquire);
        } while ((seq0 != seq1) || (seq0 & 1));

        return result;
    }

    template <typename T>
    void SequentialLock<T>::Write(T const& value)
    {
        std::size_t const seq = this->m_Sequence.load(std::memory_order::relaxed);
        this->m_Sequence.store(seq + 1, std::memory_order::release);
        std::atomic_signal_fence(std::memory_order::acq_rel);
        this->m_Value = value;
        std::atomic_signal_fence(std::memory_order::acq_rel);
        this->m_Sequence.store(seq + 2, std::memory_order::release);
    }
}
