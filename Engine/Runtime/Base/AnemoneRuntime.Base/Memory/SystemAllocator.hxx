#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Memory/Allocator.hxx"

namespace Anemone::Memory
{
    class ANEMONE_RUNTIME_BASE_API SystemAllocator : public Allocator
    {
    public:
        // 64 KiB
        static constexpr size_t DefaultAlignment = size_t{64} << 10u;

    public: // api v0
        Allocation Allocate(Layout const& layout) override;
        void Deallocate(Allocation const& allocation) override;
        Allocation Reallocate(Allocation const& allocation, Layout const& layout) override;
    };
}
