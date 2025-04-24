#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone::SystemAllocator
{
    RUNTIME_API void* ReserveUncommitted(size_t size, bool writable, bool executable);
    RUNTIME_API void ReleaseDecommitted(void* address, size_t size);

    RUNTIME_API void* Commit(void* address, size_t size, bool writable, bool executable);
    RUNTIME_API void Decommit(void* address, size_t size);

    RUNTIME_API void* ReserveAndCommit(size_t size, bool writable, bool executable);
    RUNTIME_API void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable);
    RUNTIME_API void DecommitAndRelease(void* address, size_t size);

    RUNTIME_API void Reset(void* address, size_t size);
}
