#include "AnemoneRuntime/System/SystemAllocator.hxx"
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::SystemAllocator::Internal
{
    constexpr int ToPosixProtection(bool writable, bool executable)
    {
        int protection = PROT_READ;

        if (writable)
        {
            protection |= PROT_WRITE;
        }

        if (executable)
        {
            protection |= PROT_EXEC;
        }

        return protection;
    }

    inline void CommitVirtualMemory(void* address, size_t size)
    {
        while ((madvise(address, size, MADV_WILLNEED) == -1) and (errno == EAGAIN))
        {
        }
    }

    inline void DecommitVirtualMemory(void* address, size_t size)
    {
        while ((madvise(address, size, MADV_DONTNEED) == -1) and (errno == EAGAIN))
        {
        }
    }
}

namespace Anemone::SystemAllocator
{
    void* ReserveUncommitted(size_t size, bool writable, bool executable)
    {
        int const protection = Internal::ToPosixProtection(writable, executable);

        void* result = mmap(nullptr, size, protection, MAP_NORESERVE | MAP_PRIVATE | MAP_ANON, -1, 0);

        if (result == MAP_FAILED)
        {
            AE_PANIC("mmap: MAP_FAILED, errno = {}", errno);
        }

        Internal::DecommitVirtualMemory(result, size);
        return result;
    }

    void ReleaseDecommitted(void* address, size_t size)
    {
        if (munmap(address, size) == -1)
        {
            AE_PANIC("munmap: errno = {}", errno);
        }
    }

    void* Commit(void* address, size_t size, bool writable, bool executable)
    {
        int const protection = Internal::ToPosixProtection(writable, executable);

        if (mprotect(address, size, protection) != 0)
        {
            AE_PANIC("mprotect: errno = {}", errno);
        }

        Internal::CommitVirtualMemory(address, size);

        return address;
    }

    void Decommit(void* address, size_t size)
    {
        Internal::DecommitVirtualMemory(address, size);

        if (mprotect(address, size, PROT_NONE))
        {
            AE_PANIC("mprotect: errno = {}", errno);
        }
    }

    void* ReserveAndCommit(size_t size, bool writable, bool executable)
    {
        int const protection = Internal::ToPosixProtection(writable, executable);
        int const flags = MAP_PRIVATE | MAP_ANON;

        void* result = mmap(nullptr, size, protection, flags, -1, 0);

        if (result == MAP_FAILED)
        {
            AE_PANIC("mmap: MAP_FAILED, errno = {}", errno);
        }

        // N.B. Memory is commited by default on this platform. No additional work is needed.

        return result;
    }

    void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable)
    {
        void* result = ReserveUncommitted(reserve, writable, executable);
        Commit(result, commit, writable, executable);
        return result;
    }

    void DecommitAndRelease(void* address, size_t size)
    {
        if (munmap(address, size) == -1)
        {
            AE_PANIC("munmap: errno = {}", errno);
        }
    }

    void Reset(void* address, size_t size)
    {
        Internal::DecommitVirtualMemory(address, size);
    }
}
