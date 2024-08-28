#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Memory/Allocator.hxx"

namespace Anemone::Memory
{
    class RUNTIME_API SystemAllocator : public Allocator
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
