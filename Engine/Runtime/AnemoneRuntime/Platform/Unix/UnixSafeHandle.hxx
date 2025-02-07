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
    using UnixSafeFdHandle = base_SafeHandle<int, UnixSafeFdHandleTraits>;

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
    using UnixSafeNamedSemaphoreHandle = base_SafeHandle<sem_t*, UnixSafeNamedSemaphoreHandleTraits>;

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
    using UnixSafePidHandle = base_SafeHandle<pid_t, UnixSafePidHandleTraits>;

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
    using UnixSafeSharedLibraryHandle = base_SafeHandle<void*, UnixSafeSharedLibraryHandleTraits>;
    
    struct UnixPthreadThreadHandleTraits final
    {
        static pthread_t Invalid()
        {
            return pthread_t{};
        }

        static bool IsValid(pthread_t value)
        {
            return value != Invalid();
        }

        static bool Reset(pthread_t value)
        {
            (void)value;
            return true;
        }
    };

    using UnixPthreadThreadHandle = base_SafeHandle<pthread_t, UnixPthreadThreadHandleTraits>;
}
