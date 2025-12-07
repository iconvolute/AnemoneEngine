#pragma once
#include "AnemoneRuntime.Platform/Detect.hxx"

namespace Anemone::Memory
{
    class SlabAllocator
    {
    private:
        static constexpr uint64_t SlabHeaderSignature = 0x2172644862616C53;

        struct FreeEntry final
        {
            FreeEntry* m_next{};
        };

        struct SlabHeader : public IntrusiveListNode<SlabHeader, SlabAllocator>
        {
            friend class SlabAllocator;

            uint64_t m_signature{SlabHeaderSignature};
            SlabAllocator* m_owner{};
            FreeEntry* m_first{};
            size_t m_capacity{};
        };

    private:
        IntrusiveList<SlabHeader, SlabAllocator> m_partial{};
        IntrusiveList<SlabHeader, SlabAllocator> m_full{};
        Allocator* m_allocator{};
        size_t m_allocationSize{};
        size_t m_allocationAlignment{};
        size_t m_slabCapacity{};

        static constexpr size_t SlabSize = 64u << 10u;

    private:
        static SlabHeader* GetSlabHeaderFromPointer(void* pointer)
        {
            return static_cast<SlabHeader*>(Bitwise::AlignDown(pointer, SlabSize));
        }

    public:
        SlabAllocator(Allocator* allocator, size_t allocationSize, size_t allocationAlignment)
            : m_allocator{allocator}
            , m_allocationSize{allocationSize}
            , m_allocationAlignment{allocationAlignment}
        {
            AE_ASSERT(Bitwise::IsAligned(allocationSize, allocationAlignment));
            AE_ASSERT(Bitwise::IsPowerOf2(allocationAlignment));

            this->m_slabCapacity = (SlabSize - sizeof(SlabHeader)) / allocationSize;
        }

        void* Allocate()
        {
            SlabHeader* slab = this->m_partial.PeekFront();

            if (slab == nullptr)
                [[unlikely]]
            {
                // Allocate new slab.
                slab = this->NewSlab();
                // Push it to list of partial slabs.
                this->m_partial.PushFront(slab);
            }

            AE_ASSERT(slab != nullptr);
            AE_ASSERT(slab->m_capacity != 0);
            AE_ASSERT(slab->m_signature == SlabHeaderSignature);
            AE_ASSERT(slab->m_owner == this);

            // Get element from free list.
            FreeEntry* result = slab->m_first;

            // Set the first element to the next free slot.
            slab->m_first = result->m_next;

            // Update count of allocated elements.
            --slab->m_capacity;

            if (slab->m_capacity == 0)
            {
                // Move that slab to the full list.
                this->m_partial.Remove(slab);
                this->m_full.PushFront(slab);
            }

            // Return the allocated element.
            result->~FreeEntry();
            return result;
        }

        void Deallocate(void* ptr)
        {
            AE_ASSERT(ptr != nullptr);

            SlabHeader* slab = GetSlabHeaderFromPointer(ptr);
            AE_ASSERT(slab != nullptr);
            AE_ASSERT(slab->m_capacity != this->m_slabCapacity);
            AE_ASSERT(slab->m_signature == SlabHeaderSignature);
            AE_ASSERT(slab->m_owner == this);

            // Update capacity.
            ++slab->m_capacity;

            // Link item back to free list
            slab->m_first = new (ptr) FreeEntry{.m_next = slab->m_first};

            if (slab->m_capacity == 1)
            {
                // Slab was full. Move it back to the partial list.
                this->m_full.Remove(slab);
                this->m_partial.PushFront(slab);
            }
            else if (slab->m_capacity == this->m_slabCapacity)
            {
                // Slab was on the partial list and now became empty. Deallocate it.
                this->m_partial.Remove(slab);
                this->DeleteSlab(slab);
            }
        }


    private:
        SlabHeader* NewSlab()
        {
            Allocation buffer = this->m_allocator->Allocate(Layout{
                .Size = SlabSize,
                .Alignment = SlabSize,
            });

            std::byte* it = Bitwise::AlignUp(static_cast<std::byte*>(buffer.Address) + sizeof(SlabHeader), this->m_allocationAlignment);

            // First entry in free list.
            FreeEntry* const first = new (it) FreeEntry;

            // Last created entry.
            FreeEntry* last = first;

            // Initialize free list of objects placed in the slab.
            for (size_t i = 0; i < this->m_slabCapacity - 1; ++i)
            {
                // Advance to the next slot.
                it += this->m_allocationSize;

                // Create a new entry.
                FreeEntry* const next = new (it) FreeEntry;

                // Link to last initialized entry.
                last->m_next = next;

                // Move to the next entry.
                last = next;
            }

            // Last entry in the slab.
            last->m_next = nullptr;

            return new (buffer.Address) SlabHeader{
                .m_signature = SlabHeaderSignature,
                .m_owner = this,
                .m_first = first,
                .m_capacity = this->m_slabCapacity,
            };
        }

        void DeleteSlab(SlabHeader* slab)
        {
            AE_ASSERT(slab != nullptr);
            AE_ASSERT(slab->m_capacity == this->m_slabCapacity);
            AE_ASSERT(slab->m_signature == SlabHeaderSignature);
            AE_ASSERT(slab->m_owner == this);

            slab->~SlabHeader();

            this->m_allocator->Deallocate(Allocation{
                .Address = slab,
                .Size = SlabSize,
            });
        }
    };
}
