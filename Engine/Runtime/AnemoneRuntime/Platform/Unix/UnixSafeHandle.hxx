#pragma once
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

#include <semaphore.h>
#include <dlfcn.h>

namespace Anemone::Interop
{
    struct UnixSafeFdHandleTraits final
    {
        static int Invalid() noexcept
        {
            return -1;
        }

        static bool IsValid(int value)
        {
            return value >= 0;
        }

        static bool Reset(int value)
        {
            return close(value) == 0;
        }
    };
    using UnixSafeFdHandle = Interop::base_SafeHandle<int, UnixSafeFdHandleTraits>;

    struct UnixSafeNamedSemaphoreHandleTraits final
    {
        static sem_t* Invalid()
        {
            return SEM_FAILED;
        }

        static bool IsValid(sem_t* value)
        {
            return value != SEM_FAILED;
        }

        static bool Reset(sem_t* value)
        {
            return sem_close(value) == 0;
        };
    };
    using UnixSafeNamedSemaphoreHandle = Interop::base_SafeHandle<sem_t*, UnixSafeNamedSemaphoreHandleTraits>;

    struct UnixSafePidHandleTraits final
    {
        static pid_t Invalid()
        {
            return -1;
        }
        static bool IsValid(pid_t value)
        {
            return value >= 0;
        }
        static bool Reset(pid_t value)
        {
            // noop or kill?
            (void)value;
            return true;
            // return kill(value, SIGKILL) == 0;
        }
    };
    using UnixSafePidHandle = Interop::base_SafeHandle<pid_t, UnixSafePidHandleTraits>;

    struct UnixSafeSharedLibraryHandleTraits
    {
        static void* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(void* value)
        {
            return value != nullptr;
        }

        static bool Reset(void* value)
        {
            return dlclose(value) == 0;
        }
    };
    using UnixSafeSharedLibraryHandle = Interop::base_SafeHandle<void*, UnixSafeSharedLibraryHandleTraits>;
}
