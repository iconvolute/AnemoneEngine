#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <memory>
#include <bit>
#include <atomic>

namespace Anemone
{
    template <typename T>
    class MpscQueue final
    {
    private:
        struct Slot final
        {
            std::atomic<T*> Element{};
        };

    private:
        alignas(ANEMONE_CACHELINE_SIZE) std::unique_ptr<Slot[]> m_Slots{};
        size_t m_Capacity{};
        size_t m_Tail{};
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Head{};
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Count{};

    public:
        MpscQueue(size_t capacity) noexcept
            : m_Capacity{std::bit_ceil(capacity)}
        {
            this->m_Slots = std::make_unique<Slot[]>(this->m_Capacity);
            std::atomic_thread_fence(std::memory_order::release);
        }

        ~MpscQueue() noexcept = default;

        MpscQueue(MpscQueue const&) = delete;

        MpscQueue(MpscQueue&&) = delete;

        MpscQueue& operator=(MpscQueue const&) = delete;

        MpscQueue& operator=(MpscQueue&&) = delete;

    public:
        bool TryPush(T* element) noexcept
        {
            size_t const count = this->m_Count.fetch_add(1, std::memory_order::acquire);

            if (count >= this->m_Capacity)
            {
                this->m_Count.fetch_sub(1, std::memory_order::release);
                return false;
            }

            size_t const head = this->m_Head.fetch_add(1, std::memory_order::acquire);
            ANEMONE_ASSERT(this->m_Slots[head % this->m_Capacity].Element == nullptr);

            [[maybe_unused]] T* const result = this->m_Slots[head % this->m_Capacity].Element.exchange(element, std::memory_order::release);
            ANEMONE_ASSERT(result == nullptr);
            return true;
        }

        T* TryPop() noexcept
        {
            T* result = this->m_Slots[this->m_Tail].Element.exchange(nullptr, std::memory_order::acquire);

            if (result == nullptr)
            {
                return nullptr;
            }

            if (++this->m_Tail >= this->m_Capacity)
            {
                // Wrap around
                this->m_Tail = 0;
            }

            [[maybe_unused]] size_t const count = this->m_Count.fetch_sub(1, std::memory_order::release);
            AE_ASSERT(count > 0);
            return result;
        }

        size_t GetSize() const noexcept
        {
            return this->m_Count.load(std::memory_order::relaxed);
        }

        size_t GetCapacity() const noexcept
        {
            return this->m_Capacity;
        }
    };
}
