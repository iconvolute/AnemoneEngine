#include "AnemoneMemory/MemoryArena.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"

namespace Anemone
{
    void* RawMemoryArena::Segment::Allocate(size_t size, size_t alignment)
    {
        AE_ASSERT(size != 0);
        AE_ASSERT(alignment != 0);

        size_t const validSize = Bitwise::AlignUp(size, alignment);

        uintptr_t const segmentStart = std::bit_cast<uintptr_t>(this);
        uintptr_t const segmentEnd = segmentStart + this->Size;
        uintptr_t const lastAddress = segmentStart + this->Last;
        uintptr_t const allocationStart = Bitwise::AlignUp(lastAddress, alignment);
        uintptr_t const allocationEnd = allocationStart + validSize;

        void* result{};

        if (allocationEnd <= segmentEnd)
        {
            // Update the last allocated offset.
            this->Last = allocationEnd - segmentStart;

            result = std::bit_cast<std::byte*>(allocationStart);
        }

        return result;
    }

    RawMemoryArena::Segment* RawMemoryArena::AllocateSegment(size_t segmentSize)
    {
        void* const allocation = ::operator new(segmentSize);
        AE_ASSERT(allocation != nullptr);
        AE_ASSERT(Bitwise::IsAligned(allocation, alignof(Segment)));

        Segment* const header = std::construct_at(static_cast<Segment*>(allocation), segmentSize);
        this->_segments.PushFront(header);
        return header;
    }

    void* RawMemoryArena::AllocateOversized(size_t size, size_t alignment)
    {
        AE_ASSERT(size != 0);
        AE_ASSERT(alignment != 0);

        // Compute proper alignment for the allocation.
        size_t const totalAlignment = std::max(alignment, alignof(Oversized));

        // Compute total allocation size.
        size_t const allocationOffset = Bitwise::AlignUp(sizeof(Oversized), totalAlignment);
        size_t totalSize = Bitwise::AlignUp(allocationOffset + size, totalAlignment);

        void* const allocation = ::operator new(totalSize, std::align_val_t{totalAlignment});
        AE_ASSERT(allocation != nullptr);

        Oversized* const header = std::construct_at(static_cast<Oversized*>(allocation), totalSize, totalAlignment);
        this->_oversized.PushBack(header);

        void* const result = Bitwise::Adjust(allocation, static_cast<ptrdiff_t>(allocationOffset));
        return result;
    }

    void* RawMemoryArena::Allocate(size_t size, size_t alignment)
    {
        if (size > (this->_defaultSegmentSize / 4))
        {
            // Allocate oversized memory separately.
            return this->AllocateOversized(size, alignment);
        }

        void* result{};

        if (Segment* segment = this->_segments.PeekFront())
        {
            result = segment->Allocate(size, alignment);
        }

        if (not result)
        {
            Segment* const segment = this->AllocateSegment(this->_defaultSegmentSize);
            AE_ASSERT(segment != nullptr);

            result = segment->Allocate(size, alignment);
        }

        if (not result)
        {
            // If we still don't have a valid allocation, we are out of memory.
            // This should not happen unless the segment size is too small.
            result = this->AllocateOversized(size, alignment);
        }

        AE_ASSERT(result, "Out of memory");
        return result;
    }

    void RawMemoryArena::Reset()
    {
        while (Segment* current = this->_segments.PopFront())
        {
            ::operator delete(current);
        }

        while (Oversized* current = this->_oversized.PopFront())
        {
            ::operator delete(current, std::align_val_t{current->Alignment});
        }
    }

    void RawMemoryArena::QueryMemoryUsage(size_t& reserved, size_t& allocated) const
    {
        this->_segments.ForEach(
            [&](Segment const& segment)
        {
            reserved += segment.Size;
            allocated += segment.GetAllocated();
        });

        this->_oversized.ForEach(
            [&](Oversized const& oversized)
        {
            reserved += oversized.Size;
            allocated += oversized.Size; // Oversized allocations are always fully allocated.
        });
    }
}
