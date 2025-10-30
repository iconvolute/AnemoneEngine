#pragma once
#include "AnemoneInterop/Headers.hxx"

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
//
// #pragma once
// #include "AnemoneInterop/Headers.hxx"
//
// namespace Anemone
//{
//    struct SystemAllocator final
//    {
//        static RUNTIME_API void* ReserveUncommitted(size_t size, bool writable, bool executable);
//        static RUNTIME_API void ReleaseDecommitted(void* address, size_t size);
//
//        static RUNTIME_API void* Commit(void* address, size_t size, bool writable, bool executable);
//        static RUNTIME_API void Decommit(void* address, size_t size);
//
//        static RUNTIME_API void* ReserveAndCommit(size_t size, bool writable, bool executable);
//        static RUNTIME_API void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable);
//        static RUNTIME_API void DecommitAndRelease(void* address, size_t size);
//
//        static RUNTIME_API void Reset(void* address, size_t size);
//    };
//}
