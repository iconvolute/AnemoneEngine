#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::System
{
    class Allocator
    {
    public:
        RUNTIME_API static void* ReserveUncommitted(size_t size, bool writable, bool executable);
        RUNTIME_API static void ReleaseDecommitted(void* address, size_t size);

        RUNTIME_API static void* Commit(void* address, size_t size, bool writable, bool executable);
        RUNTIME_API static void Decommit(void* address, size_t size);

        RUNTIME_API static void* ReserveAndCommit(size_t size, bool writable, bool executable);
        RUNTIME_API static void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable);
        RUNTIME_API static void DecommitAndRelease(void* address, size_t size);

        RUNTIME_API static void Reset(void* address, size_t size);
    };
}
