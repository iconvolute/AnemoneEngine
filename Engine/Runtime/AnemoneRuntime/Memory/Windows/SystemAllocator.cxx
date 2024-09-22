#include "AnemoneRuntime/Memory/SystemAllocator.hxx"
#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

// #include <Windows.h>

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

        MEM_ADDRESS_REQUIREMENTS addressRequirements{};
        addressRequirements.Alignment = layout.Alignment;

        MEM_EXTENDED_PARAMETER extendedParameters{};
        extendedParameters.Type = MemExtendedParameterAddressRequirements;
        extendedParameters.Pointer = &addressRequirements;

        ULONG attributes = MEM_RESERVE | MEM_COMMIT | MEM_64K_PAGES;
        ULONG protection = PAGE_READWRITE;
        VOID* result = VirtualAlloc2FromApp(nullptr, nullptr, alignedSize, attributes, protection, &extendedParameters, 1);

        return Allocation{
            .Address = result,
            .Size = alignedSize,
        };
    }

    void SystemAllocator::Deallocate(Allocation const& allocation)
    {
        VirtualFree(allocation.Address, 0, MEM_RELEASE);
    }

    Allocation SystemAllocator::Reallocate(Allocation const& allocation, Layout const& layout)
    {
        (void)allocation;
        (void)layout;
        AE_PANIC("Not supported");
    }
}
