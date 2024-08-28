#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstddef>

namespace Anemone::Memory
{
    struct Layout
    {
        size_t Size;
        size_t Alignment;
    };

    struct Allocation
    {
        void* Address;
        size_t Size;
    };
}

namespace Anemone::Memory
{
    class RUNTIME_API Allocator
    {
    public:
        virtual ~Allocator() = default;

    public: // api v0
        virtual Allocation Allocate(Layout const& layout) = 0;
        virtual void Deallocate(Allocation const& allocation) = 0;
        virtual Allocation Reallocate(Allocation const& allocation, Layout const& layout) = 0;
    };
}
