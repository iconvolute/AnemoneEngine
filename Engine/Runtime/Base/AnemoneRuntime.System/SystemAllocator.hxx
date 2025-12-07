#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

namespace Anemone::SystemAllocator
{
    ANEMONE_RUNTIME_BASE_API void* ReserveUncommitted(size_t size, bool writable, bool executable);
    ANEMONE_RUNTIME_BASE_API void ReleaseDecommitted(void* address, size_t size);

    ANEMONE_RUNTIME_BASE_API void* Commit(void* address, size_t size, bool writable, bool executable);
    ANEMONE_RUNTIME_BASE_API void Decommit(void* address, size_t size);

    ANEMONE_RUNTIME_BASE_API void* ReserveAndCommit(size_t size, bool writable, bool executable);
    ANEMONE_RUNTIME_BASE_API void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable);
    ANEMONE_RUNTIME_BASE_API void DecommitAndRelease(void* address, size_t size);

    ANEMONE_RUNTIME_BASE_API void Reset(void* address, size_t size);
}
//
// #pragma once
// #include "AnemoneRuntime.Interop/Headers.hxx"
//
// namespace Anemone
//{
//    struct SystemAllocator final
//    {
//        static ANEMONE_RUNTIME_BASE_API void* ReserveUncommitted(size_t size, bool writable, bool executable);
//        static ANEMONE_RUNTIME_BASE_API void ReleaseDecommitted(void* address, size_t size);
//
//        static ANEMONE_RUNTIME_BASE_API void* Commit(void* address, size_t size, bool writable, bool executable);
//        static ANEMONE_RUNTIME_BASE_API void Decommit(void* address, size_t size);
//
//        static ANEMONE_RUNTIME_BASE_API void* ReserveAndCommit(size_t size, bool writable, bool executable);
//        static ANEMONE_RUNTIME_BASE_API void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable);
//        static ANEMONE_RUNTIME_BASE_API void DecommitAndRelease(void* address, size_t size);
//
//        static ANEMONE_RUNTIME_BASE_API void Reset(void* address, size_t size);
//    };
//}
