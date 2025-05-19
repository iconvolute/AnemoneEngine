#pragma once
#include "AnemoneRuntime/Threading/Spinlock.hxx"

namespace Anemone
{
    template <typename T, size_t Capacity>
    class SynchronizedSpscQueue
    {
        static_assert(std::has_single_bit(Capacity));

    private:
        alignas(ANEMONE_CACHELINE_SIZE) Spinlock _writerLock;
        alignas(ANEMONE_CACHELINE_SIZE) Spinlock _readerLock;
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t _head{};
        alignas(ANEMONE_CACHELINE_SIZE) std::atomic_size_t _tail{};

        struct Element
        {
            alignas(T) std::byte Storage[sizeof(T)];
        };

        std::array<Element, Capacity> _elements{};

        static constexpr size_t NextIndex(size_t current)
        {
            return (current + 1uz) & Capacity;
        }

    public:
        SynchronizedSpscQueue() = default;

        SynchronizedSpscQueue(SynchronizedSpscQueue const&) = delete;

        SynchronizedSpscQueue(SynchronizedSpscQueue&&) = delete;

        SynchronizedSpscQueue& operator=(SynchronizedSpscQueue const&) = delete;

        SynchronizedSpscQueue& operator=(SynchronizedSpscQueue&&) = delete;

        ~SynchronizedSpscQueue() = default;

    public:
        template <typename... Args>
        bool TryEnqueue(Args&&... args)
        {
            UniqueLock scope(this->_writerLock);

            size_t const next = NextIndex(this->_head);

            if (next != this->_tail)
            {
                Element& element = this->_elements[this->_head];
                std::construct_at(reinterpret_cast<T*>(element.Storage), std::forward<Args>(args)...);

                this->_head = next;
                return true;
            }

            return false;
        }

        bool TryDequeue(T* result)
        {
            if (this->_head != this->_tail)
            {
                UniqueLock scope{this->_readerLock};

                if (this->_head != this->_tail)
                {
                    Element& element = this->_elements[this->_tail];

                    if (result)
                    {
                        *result = std::move(*reinterpret_cast<T*>(element.Storage));
                    }
                    std::destroy_at(reinterpret_cast<T*>(element.Storage));

                    this->_tail = NextIndex(this->_tail);
                    return true;
                }
            }

            return false;
        }
    };
}
