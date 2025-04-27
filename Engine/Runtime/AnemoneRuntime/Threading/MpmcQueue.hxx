#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <atomic>
#include <memory>
#include <bit>

namespace Anemone
{
    // Implementation notes:
    // - capacity must be a power of 2, so we don't need to worry about overflow in sequence counters
    template <typename T>
    class MpmcQueue final
    {
    private:
        struct alignas(ANEMONE_CACHELINE_SIZE) Slot final
        {
            std::atomic_size_t Sequence{};
            alignas(T) std::byte ValueStorage[sizeof(T)];
        };

    private:
        alignas(ANEMONE_CACHELINE_SIZE) std::unique_ptr<Slot[]> m_Slots{};
        size_t m_SlotsMask{};

        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Enqueue{};
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t m_Dequeue{};

    public:
        explicit MpmcQueue(std::size_t capacity) noexcept
        {
            size_t const alignedCapacity = std::bit_ceil(capacity);

            this->m_SlotsMask = alignedCapacity - 1;

            this->m_Slots = std::make_unique<Slot[]>(alignedCapacity);

            for (std::size_t i = 0; i != alignedCapacity; ++i)
            {
                this->m_Slots[i].Sequence.store(i, std::memory_order::relaxed);
            }

            this->m_Enqueue.store(0, std::memory_order::relaxed);
            this->m_Dequeue.store(0, std::memory_order::relaxed);
        }

        MpmcQueue(MpmcQueue const&) = delete;

        MpmcQueue(MpmcQueue&&) = delete;

        MpmcQueue& operator=(MpmcQueue const&) = delete;

        MpmcQueue& operator=(MpmcQueue&&) = delete;

        ~MpmcQueue() noexcept
        {
            // Drain queue before exit.
            while (this->TryPop(nullptr))
            {
                // do nothing.
            }
        }

        template <typename... ArgsT>
        bool TryEmplace(ArgsT&&... args) noexcept
        {
            Slot* slot;

            std::size_t position = this->m_Enqueue.load(std::memory_order::relaxed);

            while (true)
            {
                slot = &this->m_Slots[position & this->m_SlotsMask];

                std::size_t const sequence = slot->Sequence.load(std::memory_order::acquire);

                std::ptrdiff_t const difference = static_cast<std::ptrdiff_t>(sequence) - static_cast<std::ptrdiff_t>(position);

                if (difference == 0)
                {
                    if (this->m_Enqueue.compare_exchange_weak(position, position + 1, std::memory_order::relaxed))
                    {
                        break;
                    }
                }
                else if (difference < 0)
                {
                    return false;
                }
                else
                {
                    position = this->m_Enqueue.load(std::memory_order::relaxed);
                }
            }

            std::construct_at(reinterpret_cast<T*>(slot->ValueStorage), std::forward<ArgsT>(args)...);
            slot->Sequence.store(position + 1, std::memory_order::release);
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

        bool TryPop(T* result) noexcept
        {
            Slot* slot;

            std::size_t position = this->m_Dequeue.load(std::memory_order::relaxed);

            while (true)
            {
                slot = &this->m_Slots[position & this->m_SlotsMask];
                std::size_t const sequence = slot->Sequence.load(std::memory_order::acquire);
                std::ptrdiff_t const difference = static_cast<std::ptrdiff_t>(sequence) - static_cast<std::ptrdiff_t>(position + 1);

                if (difference == 0)
                {
                    if (this->m_Dequeue.compare_exchange_weak(position, position + 1, std::memory_order::relaxed))
                    {
                        break;
                    }
                }
                else if (difference < 0)
                {
                    return false;
                }
                else
                {
                    position = this->m_Dequeue.load(std::memory_order::relaxed);
                }
            }

            if (result != nullptr)
            {
                (*result) = std::move(*reinterpret_cast<T*>(slot->ValueStorage));
            }

            std::destroy_at(reinterpret_cast<T*>(slot->ValueStorage));
            slot->Sequence.store(position + this->m_SlotsMask + 1, std::memory_order::release);
            return true;
        }
    };
}
