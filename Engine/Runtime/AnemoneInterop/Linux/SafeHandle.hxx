#pragma once
#include "AnemoneInterop/SafeHandle.hxx"
#include "AnemoneInterop/Linux/Headers.hxx"

#include <semaphore.h>
#include <dlfcn.h>

namespace Anemone::Interop::Linux
{
    struct SafeFdHandleTraits final
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
            int rv;

            while ((rv = close(value)) < 0)
            {
                if (errno != EINTR)
                {
                    break;
                }
            }

            return rv == 0;
        }
    };
    using SafeFdHandle = SafeHandleT<int, SafeFdHandleTraits>;

    struct SafeDirHandleTraits final
    {
        static DIR* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(DIR* value)
        {
            return value != nullptr;
        }

        static bool Reset(DIR* value)
        {
            return closedir(value) == 0;
        }
    };
    using SafeDirHandle = SafeHandleT<DIR*, SafeDirHandleTraits>;

    struct SafeNamedSemaphoreHandleTraits final
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
    using SafeNamedSemaphoreHandle = SafeHandleT<sem_t*, SafeNamedSemaphoreHandleTraits>;

    struct SafePidHandleTraits final
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
    using SafePidHandle = SafeHandleT<pid_t, SafePidHandleTraits>;

    struct SafePidFdHandleTraits final
    {
        static int Invalid()
        {
            return -1;
        }
        static bool IsValid(
            int value)
        {
            return value >= 0;
        }
        static bool Reset(
            int value)
        {
            return close(value) == 0;
        }
    };
    using SafePidFdHandle = SafeHandleT<int, SafePidFdHandleTraits>;

    struct SafeSharedLibraryHandleTraits
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
    using SafeSharedLibraryHandle = SafeHandleT<void*, SafeSharedLibraryHandleTraits>;

    struct SafePthreadThreadHandleTraits final
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

    using SafePthreadThreadHandle = SafeHandleT<pthread_t, SafePthreadThreadHandleTraits>;

    struct SafeMemoryMappedViewHandleTraits final
    {
        static void* Invalid()
        {
            return MAP_FAILED;
        }
        static bool IsValid(
            void* value
        )
        {
            return value != MAP_FAILED;
        }
        static bool Reset(
            void* value,
            size_t size
        )
        {
            return ::munmap(value, size) == 0;
        }
    };
    using SafeMemoryMappedViewHandle = SafeBufferT<void, SafeMemoryMappedViewHandleTraits>;
}
