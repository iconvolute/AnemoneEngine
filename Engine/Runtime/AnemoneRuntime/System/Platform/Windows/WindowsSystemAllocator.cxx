#include "AnemoneRuntime/System/SystemAllocator.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::SystemAllocator::Internal
{
    constexpr DWORD ToVirtualMemoryProtectFlags(bool writable, bool executable)
    {
        if (executable)
        {
            if (writable)
            {
                return PAGE_EXECUTE_READWRITE;
            }
            else
            {
                return PAGE_EXECUTE_READ;
            }
        }
        else
        {
            if (writable)
            {
                return PAGE_READWRITE;
            }
            else
            {
                return PAGE_READONLY;
            }
        }
    }
}

namespace Anemone::SystemAllocator
{
    void* ReserveUncommitted(size_t size, bool writable, bool executable)
    {
        DWORD const dwProtect = Internal::ToVirtualMemoryProtectFlags(writable, executable);
        void* const result = VirtualAlloc(nullptr, size, MEM_RESERVE, dwProtect);
        return result;
    }

    void ReleaseDecommitted(void* address, size_t size)
    {
        if (size != 0)
        {
            if (not VirtualFree(address, size, MEM_RELEASE))
            {
                AE_PANIC("VirtualFree: {}", GetLastError());
            }
        }
    }

    void* Commit(void* address, size_t size, bool writable, bool executable)
    {
        DWORD const dwProtect = Internal::ToVirtualMemoryProtectFlags(writable, executable);
        void* const result = VirtualAlloc(address, size, MEM_COMMIT, dwProtect);
        return result;
    }

    void Decommit(void* address, size_t size)
    {
        if (size != 0)
        {
            if (not VirtualFree(address, size, MEM_DECOMMIT))
            {
                AE_PANIC("VirtualFree: {}", GetLastError());
            }
        }
    }

    void* ReserveAndCommit(size_t size, bool writable, bool executable)
    {
        DWORD const dwProtect = Internal::ToVirtualMemoryProtectFlags(writable, executable);
        void* const result = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, dwProtect);
        return result;
    }

    void* ReserveAndCommit(size_t reserve, size_t commit, bool writable, bool executable)
    {
        DWORD const dwProtect = Internal::ToVirtualMemoryProtectFlags(writable, executable);
        void* result = VirtualAlloc(nullptr, reserve, MEM_RESERVE, dwProtect);
        VirtualAlloc(result, commit, MEM_COMMIT, dwProtect);
        return result;
    }

    void DecommitAndRelease(void* address, size_t size)
    {
        if (size != 0)
        {
            if (not VirtualFree(address, 0, MEM_RELEASE))
            {
                AE_PANIC("VirtualFree: {}", GetLastError());
            }
        }
    }

    void Reset(void* address, size_t size)
    {
        if (size != 0)
        {
            if (not VirtualAlloc(address, size, MEM_RESET, PAGE_READONLY))
            {
                AE_PANIC("VirtualFree: {}", GetLastError());
            }
        }
    }
}
