#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <semaphore.h>

namespace Anemone::Internal
{
    struct NativeNamedMutexTraits final
    {
        static sem_t* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(sem_t* value)
        {
            return value != nullptr;
        }

        static void Close(sem_t* value)
        {
            if (sem_close(value))
            {
                AE_TRACE(Debug, "Failed to close named mutex: handle={}, error={}", fmt::ptr(value), errno);
            }
        }
    };

    using NativeNamedMutex = Interop::base_SafeHandle<sem_t*, NativeNamedMutexTraits>;
}
