#pragma once
#include "AnemoneInterop/Headers.hxx"
#include <memory>
#include <atomic>

namespace Anemone
{
    template <typename T>
    class SpscQueue final
    {
    private:
        struct alignas(ANEMONE_CACHELINE_SIZE) Slot final
        {
            alignas(T) std::byte ValueStorage[sizeof(T)];
        };

    private:
        alignas(ANEMONE_CACHELINE_SIZE) std::unique_ptr<Slot[]> m_Slots{};

        size_t m_Capacity{};

        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Enqueue{};

        alignas(ANEMONE_CACHELINE_SIZE) std::size_t m_EnqueueCached{};

        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Dequeue{};

        alignas(ANEMONE_CACHELINE_SIZE) std::size_t m_DequeueCached{};

    public:
        explicit SpscQueue(size_t capacity) noexcept
            : m_Capacity{capacity}
        {
            if (this->m_Capacity < 1)
            {
                this->m_Capacity = 1;
            }

            // Needs one more element to distinct between full and empty queue.
            ++this->m_Capacity;

            if (this->m_Capacity > (SIZE_MAX - 2))
            {
                this->m_Capacity = SIZE_MAX - 2;
            }

            this->m_Slots = std::make_unique<Slot[]>(this->m_Capacity);

            static_assert(alignof(SpscQueue) == ANEMONE_CACHELINE_SIZE);
        }

        ~SpscQueue() noexcept
        {
            // Drain queue before exit.
            while (this->TryPop(nullptr))
            {
                // do nothing.
            }
        }

        SpscQueue(const SpscQueue&) = delete;

        SpscQueue& operator=(const SpscQueue&) = delete;

        SpscQueue(SpscQueue&&) = delete;

        SpscQueue& operator=(SpscQueue&&) = delete;

    public:
        template <typename... ArgsT>
        bool TryEmplace(ArgsT&&... args) noexcept
        {
            size_t const enqueueIndex = this->m_Enqueue.load(std::memory_order::relaxed);
            size_t nextEnqueueIndex = enqueueIndex + 1;

            if (nextEnqueueIndex == this->m_Capacity)
            {
                nextEnqueueIndex = 0;
            }

            if (nextEnqueueIndex == this->m_DequeueCached)
            {
                this->m_DequeueCached = this->m_Dequeue.load(std::memory_order::acquire);

                if (nextEnqueueIndex == this->m_DequeueCached)
                {
                    // Double check. Queue is full.
                    return false;
                }
            }

            // Cam enqueue. Write value.
            std::construct_at(reinterpret_cast<T*>(this->m_Slots[enqueueIndex].ValueStorage), std::forward<ArgsT>(args)...);
            this->m_Enqueue.store(nextEnqueueIndex, std::memory_order::release);
            return true;
        }

        bool TryPush(T const& value) noexcept
            requires(std::is_copy_constructible_v<T>)
        {
            return this->TryEmplace(value);
        }

        bool TryPush(T&& value) noexcept
            requires(std::is_move_constructible_v<T>)
        {
            return this->TryEmplace(std::move(value));
        }

        bool TryPop(T* result)
            requires(std::is_move_assignable_v<T>)
        {
            size_t const dequeueIndex = this->m_Dequeue.load(std::memory_order::relaxed);

            if (dequeueIndex == this->m_EnqueueCached)
            {
                this->m_EnqueueCached = this->m_Enqueue.load(std::memory_order::acquire);

                if (dequeueIndex == this->m_EnqueueCached)
                {
                    // Double checked. Queue is empty.
                    return false;
                }
            }

            Slot* const slot = &this->m_Slots[dequeueIndex];

            if (result != nullptr)
            {
                *result = std::move(*reinterpret_cast<T*>(slot->ValueStorage));
            }

            std::destroy_at(reinterpret_cast<T*>(slot->ValueStorage));

            size_t nextDequeueIndex = dequeueIndex + 1;

            if (nextDequeueIndex == this->m_Capacity)
            {
                nextDequeueIndex = 0;
            }

            this->m_Dequeue.store(nextDequeueIndex, std::memory_order::release);
            return true;
        }
    };
};
