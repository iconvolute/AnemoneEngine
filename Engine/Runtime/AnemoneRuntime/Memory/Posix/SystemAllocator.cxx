#include "AnemoneRuntime/Memory/SystemAllocator.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <sys/mman.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Memory
{
    Allocation SystemAllocator::Allocate(Layout const& layout)
    {
        AE_ASSERT(layout.Size > 0);
        AE_ASSERT(layout.Alignment > 0);
        AE_ASSERT(Bitwise::IsPowerOf2(layout.Alignment));
        AE_ASSERT(Bitwise::IsAligned(layout.Size, layout.Alignment));
        AE_ASSERT(Bitwise::IsAligned(layout.Alignment, DefaultAlignment));

        size_t const alignedSize = Bitwise::AlignUp(layout.Size, layout.Alignment);

#if defined(MAP_ALIGNED)

        size_t const alignmentLog2 = std::countr_zero(layout.Alignment);

        void* result = mmap(nullptr, alignedSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_ALIGNED(alignmentLog2), -1, 0);

#else
        //
        // Let's assume that OS can allocate memory using somewhat "expected" alignment.
        //
        // Otherwise, try to overallocate and trim lead/tail parts. This will be slower and
        // might leave holes in virtual memory mapping.
        //

        int protectionFlags = PROT_READ | PROT_WRITE;
        int mappingFlags = MAP_PRIVATE | MAP_ANONYMOUS;

        if (void* p = mmap(nullptr, alignedSize, protectionFlags, mappingFlags, -1, 0))
        {
            if (Bitwise::IsAligned(p, layout.Alignment))
            {
                //
                // OS was kind enough to return pointer which meets our expectations.
                //

                return Allocation{
                    .Address = p,
                    .Size = alignedSize,
                };
            }

            //
            // Allocated buffer is not aligned. Return it to OS because it's too small for requested allocation.
            //

            AE_TRACE(Warning, "SystemAllocator: OS returned unaligned memory block, falling back to overallocation strategy.");
            munmap(p, alignedSize);
        }

        //
        // Fall back to overallocation strategy.
        //
        // Allocate a larger block than requested to ensure that we can align the result.
        //

        size_t const overallocatedSize = alignedSize + layout.Alignment;

        void* overallocated = mmap(nullptr, overallocatedSize, protectionFlags, mappingFlags, -1, 0);
        AE_ASSERT(overallocated != MAP_FAILED);

        void* result = Bitwise::AlignUp(overallocated, layout.Alignment);

        size_t suffixSize = layout.Alignment;

        if (overallocated != result)
        {
            //
            // Trim leading part of allocation.
            //
            // Note: if this branch is not taken, it means that OS was able to align allocation this time.
            //

            size_t const prefixSize = reinterpret_cast<size_t>(result) - reinterpret_cast<size_t>(overallocated);
            munmap(overallocated, prefixSize);

            AE_TRACE(Warning, "SystemAllocator: OS returned unaligned memory block, trimmed leading part (ptr: {}, size: {}).", fmt::ptr(overallocated), prefixSize);
            suffixSize -= prefixSize;
        }

        //
        // Trim trailing allocation part.
        //

        void* suffix = reinterpret_cast<void*>(reinterpret_cast<char*>(result) + alignedSize);
        munmap(suffix, suffixSize);

        AE_TRACE(Warning, "SystemAllocator: OS returned unaligned memory block, trimmed trailing part (ptr: {}, size: {}).", fmt::ptr(suffix), suffixSize);

#endif
        return Allocation{
            .Address = result,
            .Size = alignedSize,
        };
    }

    void SystemAllocator::Deallocate(Allocation const& allocation)
    {
        munmap(allocation.Address, allocation.Size);
    }

    Allocation SystemAllocator::Reallocate(Allocation const& allocation, Layout const& layout)
    {
        (void)allocation;
        (void)layout;
        AE_PANIC("Not supported");
    }
}
